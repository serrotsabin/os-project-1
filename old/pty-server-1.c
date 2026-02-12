#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <util.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <termios.h>

#define SOCKET_PATH "/tmp/cis_test.sock"

struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);
    
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

double now_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

int main() {
    // 1. Create PTY
    int master;
    pid_t pid = forkpty(&master, NULL, NULL, NULL);
    
    if (pid < 0) {
        perror("forkpty");
        exit(1);
    }
    
    if (pid == 0) {
        // Child: run bash
        execl("/bin/bash", "bash", NULL);
        exit(1);
    }
    
    // 2. Create socket server
    int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    
    unlink(SOCKET_PATH);
    bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_sock, 1);
    
    printf("[Server] Waiting for 1 client on %s\r\n", SOCKET_PATH);
    
    // 3. Accept 1 client
    int client = accept(server_sock, NULL, NULL);
    if (client < 0) {
        perror("accept");
        exit(1);
    }
    
    printf("[Server] Client connected! Starting session...\r\n");
    
    // 4. Enable raw mode for controller input
    enable_raw_mode();
    
    // 5. Open log file
    FILE *log = fopen("appendix/broadcast_latency.txt", "w");
    fprintf(log, "pty_read_time,bytes,socket_write_time,latency_ms\n");
    fflush(log);
    
    char buf[4096];
    int measurement_count = 0;
    
    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(master, &readfds);
        
        int max_fd = (master > STDIN_FILENO) ? master : STDIN_FILENO;
        
        int ret = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (ret < 0) {
            perror("select");
            break;
        }
        
        // Controller input → PTY
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            int n = read(STDIN_FILENO, buf, sizeof(buf));
            if (n <= 0) break;
            
            // Check for Ctrl+Q to exit
            if (n == 1 && buf[0] == 17) {
                printf("\r\n[Server] Exiting...\r\n");
                break;
            }
            
            write(master, buf, n);
            write(STDOUT_FILENO, buf, n);  // Echo locally for controller
        }
        
        // PTY output → Broadcast to client
        if (FD_ISSET(master, &readfds)) {
            double t1 = now_ms();
            
            int n = read(master, buf, sizeof(buf));
            if (n <= 0) break;
            
            // Send to observer client
            write(client, buf, n);
            
            double t2 = now_ms();
            double latency = t2 - t1;
            
            // Write to controller's terminal too
            write(STDOUT_FILENO, buf, n);
            
            // Log latency (first 50 measurements)
            if (measurement_count < 50) {
                fprintf(log, "%.3f,%d,%.3f,%.3f\n", t1, n, t2, latency);
                fflush(log);
                measurement_count++;
            }
        }
    }
    
    fclose(log);
    close(client);
    close(server_sock);
    unlink(SOCKET_PATH);
    
    return 0;
}
