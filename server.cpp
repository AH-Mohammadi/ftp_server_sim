#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

void handleUpload(int new_socket) {
    char filename[1024] = {0};
    read(new_socket, filename, 1024);  // Receive filename from client

    std::ofstream outfile(filename, std::ios::binary);

    char file_buffer[1024];
    int bytes = 0;
    while ((bytes = read(new_socket, file_buffer, sizeof(file_buffer))) > 0) {
        outfile.write(file_buffer, bytes);
    }

    outfile.close();
}

void handleDownload(int new_socket) {
    char filename[1024] = {0};
    read(new_socket, filename, 1024);  // Receive filename from client

    std::ifstream infile(filename, std::ios::binary);

    char file_buffer[1024];
    int bytes = 0;
    while ((bytes = infile.readsome(file_buffer, sizeof(file_buffer))) > 0) {
        send(new_socket, file_buffer, bytes, 0);
    }

    infile.close();
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create a socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Forcefully attach socket to the port
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    // Configure server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));

    // Start listening for incoming connections
    listen(server_fd, 3);

    while (true) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

        char command[1024] = {0};
        read(new_socket, command, 1024);  // Read the command (UPLOAD or DOWNLOAD)

        if (strcmp(command, "UPLOAD") == 0) {
            handleUpload(new_socket);  // Handle file upload
        } else if (strcmp(command, "DOWNLOAD") == 0) {
            handleDownload(new_socket);  // Handle file download
        }

        close(new_socket);  // Close the client connection after each operation
    }

    return 0;
}
