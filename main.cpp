#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

constexpr int PORT = 8080;
constexpr int BACKLOG = 5;
constexpr int BUFFER_SIZE = 1024;

int main() {
    int server_fd, client_fd;
    sockaddr_in server_addr{}, client_addr{};
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        perror("bind failed");
        return 1;
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen failed");
        return 1;
    }

    std::cout << "Listening on port " << PORT << "...\n";

    while (true) {
        client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        std::cout << "Connection accepted from " << inet_ntoa(client_addr.sin_addr) << "\n";

        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            std::cout << "Received:\n" << buffer << "\n";
        }

        const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello world";
        send(client_fd, response, strlen(response), 0);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
