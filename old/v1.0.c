// Basic Terminal

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <util.h>
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

int main() {
    int master;
    pid_t pid = forkpty(&master, NULL, NULL, NULL);
    
    if (pid < 0) {
        perror("forkpty");
        exit(1);
    }
    
    if (pid == 0) {
        // Child: become bash
        execl("/bin/bash", "bash", NULL);
        exit(1);
    }
    
    // Parent: relay I/O
    fd_set readfds;
    char buf[4096];
    
    printf("Minimal PTY started. Type commands (Ctrl+D to exit)\n");
    
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(master, &readfds);
        
        int ret = select(master + 1, &readfds, NULL, NULL, NULL);
        if (ret < 0) {
            perror("select");
            break;
        }
        
        // User input -> shell
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            int n = read(STDIN_FILENO, buf, sizeof(buf));
            if (n <= 0) break;
            write(master, buf, n);
        }
        
        // Shell output -> user
        if (FD_ISSET(master, &readfds)) {
            int n = read(master, buf, sizeof(buf));
            if (n <= 0) break;
            write(STDOUT_FILENO, buf, n);
        }
    }
    
    return 0;
}