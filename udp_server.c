#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 10155
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);
    char buffer[BUFFER_SIZE];
    FILE *fp;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind socket
    if (bind(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    printf("📡 UDP Server listening on port %d...\n", PORT);

    // Receive filename first
    int name_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                            (struct sockaddr*)&client, &client_len);
    buffer[name_len] = '\0';
    printf("📄 Receiving file: %s\n", buffer);

    fp = fopen(buffer, "wb");
    if (!fp) {
        perror("File open failed");
        exit(1);
    }

    // Receive file contents
    ssize_t bytes;
    while ((bytes = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                             (struct sockaddr*)&client, &client_len)) > 0) {
        if (strcmp(buffer, "EOF") == 0) break;  // End of file signal
        fwrite(buffer, 1, bytes, fp);
        buffer[bytes] = '\0';
        printf("%s", buffer); // Print received chunk
    }

    printf("\n✅ File received successfully.\n");

    fclose(fp);
    close(sockfd);
    return 0;
}
