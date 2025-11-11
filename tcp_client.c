#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 10155
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];
    FILE *fp;
    char filename[256] = "sample.txt";  // File to send

    // Create TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(1);
    }
    printf("✅ Connected to server.\n");

    // Send filename first
    send(sockfd, filename, strlen(filename), 0);
    printf("📄 Sending file: %s\n", filename);

    // Open file
    fp = fopen(filename, "rb");
    if (!fp) {
        perror("File open failed");
        close(sockfd);
        exit(1);
    }

    // Send file contents
    ssize_t bytes;
    while ((bytes = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        send(sockfd, buffer, bytes, 0);
        buffer[bytes] = '\0';
        printf("%s", buffer); // Live print
    }

    // Send EOF signal
    send(sockfd, "EOF", 3, 0);

    printf("\n✅ File sent successfully.\n");

    fclose(fp);
    close(sockfd);
    return 0;
}
