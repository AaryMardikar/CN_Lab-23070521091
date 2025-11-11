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

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    socklen_t server_len = sizeof(server);

    // Send filename first
    sendto(sockfd, filename, strlen(filename), 0,
           (struct sockaddr*)&server, server_len);
    printf("📄 Sending file: %s\n", filename);

    // Open file
    fp = fopen(filename, "rb");
    if (!fp) {
        perror("File open failed");
        exit(1);
    }

    // Send file contents
    ssize_t bytes;
    while ((bytes = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        sendto(sockfd, buffer, bytes, 0,
               (struct sockaddr*)&server, server_len);
        buffer[bytes] = '\0';
        printf("%s", buffer); // Live print
    }

    // Send EOF signal
    sendto(sockfd, "EOF", 3, 0, (struct sockaddr*)&server, server_len);

    printf("\n✅ File sent successfully.\n");

    fclose(fp);
    close(sockfd);
    return 0;
}
