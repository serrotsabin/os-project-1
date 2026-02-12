#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#define SOCKET_PATH "/tmp/cis_test.sock"

int main() {
    // 1. Create socket
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }
    
    // 2. Setup address
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    
    // 3. Connect to server
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(1);
    }
    
    printf("Connected to server!\n");
    printf("Type messages (Ctrl+D to exit):\n");
    
    // 4. Send/receive loop
    char buf[256];
    while (1) {
        // Read from stdin
        printf("> ");
        fflush(stdout);
        
        int n = read(STDIN_FILENO, buf, sizeof(buf));
        if (n <= 0) break;
        
        // Send to server
        write(sock, buf, n);
        
        // Read echo from server
        n = read(sock, buf, sizeof(buf));
        if (n <= 0) break;
        
        printf("Echo: %.*s", n, buf);
    }
    
    close(sock);
    return 0;
}