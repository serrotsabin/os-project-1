#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#define SOCKET_PATH "/tmp/cis_test.sock"
#define MAX_CLIENTS 10

int main() {
    int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    
    unlink(SOCKET_PATH);
    bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_sock, 5);
    
    printf("Server listening on %s\n", SOCKET_PATH);
    printf("Type messages to broadcast to all clients\n");
    
    int clients[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i] = -1;
    }
    
    char buf[256];
    
    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        
        // Add STDIN so server can type messages
        FD_SET(STDIN_FILENO, &readfds);
        int max_fd = STDIN_FILENO;
        
        // Add server socket
        FD_SET(server_sock, &readfds);
        if (server_sock > max_fd) max_fd = server_sock;
        
        // Add all clients
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] > 0) {
                FD_SET(clients[i], &readfds);
                if (clients[i] > max_fd) max_fd = clients[i];
            }
        }
        
        select(max_fd + 1, &readfds, NULL, NULL, NULL);
        
        // Check if server typed something
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            int n = read(STDIN_FILENO, buf, sizeof(buf));
            if (n > 0) {
                printf("Broadcasting to all clients: %.*s", n, buf);
                
                // Send to ALL clients
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i] > 0) {
                        write(clients[i], buf, n);
                    }
                }
            }
        }
        
        // Check for new connection
        if (FD_ISSET(server_sock, &readfds)) {
            int new_client = accept(server_sock, NULL, NULL);
            printf("Client connected (fd=%d)\n", new_client);
            
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] == -1) {
                    clients[i] = new_client;
                    break;
                }
            }
        }
        
        // Check clients for data
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] > 0 && FD_ISSET(clients[i], &readfds)) {
                int n = read(clients[i], buf, sizeof(buf));
                
                if (n <= 0) {
                    printf("Client disconnected (fd=%d)\n", clients[i]);
                    close(clients[i]);
                    clients[i] = -1;
                } else {
                    printf("From client %d: %.*s", clients[i], n, buf);
                    // Echo back to this client
                    write(clients[i], buf, n);
                }
            }
        }
    }
    
    return 0;
}