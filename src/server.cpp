#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#define BUFF_SIZE 1024

int clientHandler(int connfd) {
    char receive_buffer[BUFF_SIZE];
    if (read(connfd, receive_buffer, sizeof(receive_buffer)) < 0) {
        std::cerr << "Failed to receive\n";
        return 1;
    }
    char send_buffer[BUFF_SIZE];
    
    if (receive_buffer[4] == '/' && receive_buffer[5] == ' ') {
        strcpy(send_buffer, "HTTP/1.1 200 OK\r\n\r\n");
    } else if (receive_buffer[4] == '/' && strstr(receive_buffer, "/echo/") != NULL) {
        strcpy(send_buffer, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ");
        char* messg = strchr(receive_buffer, 'o');
        messg += 2;
        messg = strtok(messg, " ");
        
        char* messg_size;
        snprintf(messg_size, 100, "%d", (int)strlen(messg));
        std::cout << messg << ", " << messg_size << std::endl;
        strcat(send_buffer, messg_size);
        strcat(send_buffer, "\n\n");
        strcat(send_buffer, messg);
        strcat(send_buffer, "\r\n\r\n");
    } else {
        strcpy(send_buffer, "HTTP/1.1 404 Not Found\r\n\r\n" );
    }
    
    if (write(connfd, send_buffer, sizeof(send_buffer)) < 0) {
        return 1;
    }
    
    close(connfd);
    return 0;
}

int main(int argc, char **argv) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Failed to create server socket\n";
        return 1;
    }

    //ensures that we don't run into 'Address already in use' errors
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
        std::cerr << "setsockopt failed\n";
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(4221);

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        std::cerr << "Failed to bind to port 4221\n";
        return 1;
    }

    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0) {
        std::cerr << "listen failed\n";
        return 1;
    }

    int connfd = accept(server_fd, NULL, NULL);
    if (connfd < 0) {
        std::cerr << "Failed to accept connection\n";
        return 1;
    }
    
    return clientHandler(connfd);
}
