#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 10155
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    char filename[256];
    FILE *fp;

    // Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    // Bind
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(1);
    }

    // Listen
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(1);
    }
    printf("Server listening on port %d...\n", PORT);

    socklen_t addrlen = sizeof(address);
    new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(1);
    }
    printf("Connection established with client.\n");

    // Receive filename first
    ssize_t filename_bytes;
    // Reserve 1 byte for null terminator
    filename_bytes = recv(new_socket, filename, sizeof(filename) - 1, 0); 
    if (filename_bytes <= 0) {
        perror("Failed to receive filename");
        close(new_socket);
        close(server_fd);
        exit(1);
    }
    
    // **FIX: Add null terminator to fix the 'sample.txt0' bug**
    filename[filename_bytes] = '\0'; 
    
    printf("Receiving file: %s\n", filename);

    // Open file for writing
    fp = fopen(filename, "wb"); // "wb" = write binary
    if (fp == NULL) {
        perror("File open failed");
        close(new_socket);
        close(server_fd);
        exit(1);
    }

    // Receive file data
    ssize_t bytes_received;
    while ((bytes_received = recv(new_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, fp);
    }

    if (bytes_received < 0)
        perror("Receive failed");
    else
        printf("File received successfully.\n");

    fclose(fp); // Close the file after writing

    // --- NEW CODE: Read and print file content ---
    printf("\n--- Displaying content of %s ---\n", filename);
    fp = fopen(filename, "r"); // Re-open in read mode ("r")
    if (fp == NULL) {
        perror("Failed to re-open file for reading");
    } else {
        // Read and print contents line by line
        // We can re-use the 'buffer'
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            printf("%s", buffer); // Print the line
        }
        fclose(fp); // Close the file after reading
        
        // Add a newline if the file didn't end with one, for cleaner output
        if (buffer[strlen(buffer) - 1] != '\n') {
            printf("\n");
        }
        
    }
    // --- End of new code ---

    close(new_socket);
    close(server_fd);

    return 0;
}
