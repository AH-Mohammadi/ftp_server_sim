#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

void uploadFile(int sock, const char* filename) {
    // Send upload request
    send(sock, "UPLOAD", strlen("UPLOAD"), 0);

    // Send the filename to the server
    send(sock, filename, strlen(filename), 0);

    // Open the file to be uploaded
    std::ifstream infile(filename, std::ios::binary);

    // Read the file and send it to the server
    char file_buffer[1024];
    while (!infile.eof()) {
        infile.read(file_buffer, sizeof(file_buffer));
        int bytes_read = infile.gcount();
        send(sock, file_buffer, bytes_read, 0);
    }

    infile.close();
}

void downloadFile(int sock, const char* filename) {
    // Send download request
    send(sock, "DOWNLOAD", strlen("DOWNLOAD"), 0);

    // Send the filename to download
    send(sock, filename, strlen(filename), 0);

    // Prepare the new filename for download (with a different name)
    std::string newFilename = "downloaded_" + std::string(filename);

    // Open a file to save the downloaded content
    std::ofstream outfile(newFilename, std::ios::binary);

    // Receive data from the server and write it to the file
    char file_buffer[1024];
    int bytes = 0;
    while ((bytes = read(sock, file_buffer, sizeof(file_buffer))) > 0) {
        outfile.write(file_buffer, bytes);
    }

    outfile.close();
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // Configure the server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // Connect to the server
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // Upload a file
    uploadFile(sock, "example.txt");

    // Reconnect for download (because the upload closes the socket)
    close(sock);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // Download the file (it will be saved as downloaded_example.txt)
    downloadFile(sock, "example.txt");

    // Close the socket
    close(sock);

    return 0;
}
