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
    std::string receive(receive_buffer);
    std::cout << "Input:\n" << receive << std::endl;
    std::string send_buff;
    char send_buffer[BUFF_SIZE];
    
    if (receive.find("/ ") != std::string::npos) {
        send_buff = "HTTP/1.1 200 OK\r\n\r\n";
    } else if (receive.find("/echo/") != std::string::npos) {
        send_buff = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: ";
        std::string::size_type pos1 = receive.find("echo") + 5;
        std::string::size_type pos2 = receive.find(" ", pos1);
        send_buff += std::to_string(pos2-pos1) + "\n\n";
        send_buff += receive.substr(pos1, pos2-pos1) + "\r\n\r\n";
    } else if (receive.find("/user-agent") != std::string::npos) {
        send_buff = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: ";
        std::string::size_type pos1 = receive.rfind("User-Agent") + 12;
        std::string::size_type pos2 = receive.find("\n", pos1);
        send_buff += std::to_string(pos2-pos1-1) + "\n\n";
        send_buff += receive.substr(pos1, pos2-pos1) + "\r\n\r\n";
    } else {
        send_buff = "HTTP/1.1 404 Not Found\r\n\r\n";
    }
    std::strcpy(send_buffer, send_buff.c_str()); 
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
