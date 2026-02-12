#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#define SOCKET_PATH "/tmp/cis_test.sock"

int main() {
    // 1. Create socket
    int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        exit(1);
    }
    
    // 2. Setup address
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    
    // 3. Remove old socket file if exists
    unlink(SOCKET_PATH);
    
    // 4. Bind socket to path
    if (bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }
    
    // 5. Listen for connections
    if (listen(server_sock, 5) < 0) {
        perror("listen");
        exit(1);
    }
    
    printf("Server listening on %s\n", SOCKET_PATH);
    printf("Waiting for clients...\n");
    
    // 6. Accept ONE client for now
    int client_sock = accept(server_sock, NULL, NULL);
    if (client_sock < 0) {
        perror("accept");
        exit(1);
    }
    
    printf("Client connected!\n");
    
    // 7. Echo loop
    char buf[256];
    while (1) {
        int n = read(client_sock, buf, sizeof(buf));
        if (n <= 0) {
            printf("Client disconnected\n");
            break;
        }
        
        printf("Received: %.*s", n, buf);
        write(client_sock, buf, n);  // Echo back
    }
    
    close(client_sock);
    close(server_sock);
    unlink(SOCKET_PATH);
    return 0;
}