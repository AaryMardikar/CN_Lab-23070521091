#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_FRAMES 50

void sender(int frames[], int size, int window_size);
void receiver(int frames[], int start, int window_size, int received[]);

int main() {
    int n, window_size;
    int frames[MAX_FRAMES];

    printf("Sliding Window Protocol Simulation (Selective Repeat)\n");
    printf("Enter number of frames to send (max %d): ", MAX_FRAMES);
    scanf("%d", &n);

    if (n > MAX_FRAMES || n <= 0) {
        printf("Invalid number of frames.\n");
        return 0;
    }

    printf("Enter window size: ");
    scanf("%d", &window_size);

    if (window_size <= 0 || window_size > n) {
        printf("Invalid window size.\n");
        return 0;
    }

    for (int i = 0; i < n; i++) {
        frames[i] = i;
    }

    sender(frames, n, window_size);
    return 0;
}

void sender(int frames[], int size, int window_size) {
    int base = 0;
    int received[MAX_FRAMES] = {0};
    char choice;

    srand(time(NULL));

    while (base < size) {
        printf("\nSender window: ");
        for (int i = base; i < base + window_size && i < size; i++) {
            printf("[%d] ", frames[i]);
        }

        printf("\nSending frames...\n");
        for (int i = base; i < base + window_size && i < size; i++) {
            if (!received[i]) {
                printf("Sent frame: %d\n", frames[i]);
            }
        }

        printf("\nSimulating receiver...\n");
        receiver(frames, base, window_size, received);

        printf("\nAny frame lost or corrupted? (y/n): ");
        scanf(" %c", &choice);

        if (choice == 'y' || choice == 'Y') {
            int lost_frame;
            printf("Enter lost frame number: ");
            scanf("%d", &lost_frame);

            if (lost_frame >= base && lost_frame < size) {
                printf("Resending only frame %d...\n", lost_frame);
                received[lost_frame] = 1;  // Assume successful after resend
            } else {
                printf("Invalid frame number!\n");
            }
        } else {
            for (int i = base; i < base + window_size && i < size; i++) {
                received[i] = 1;
            }
        }

        // Slide window only past acknowledged frames
        while (base < size && received[base]) {
            base++;
        }
    }

    printf("\nAll frames sent successfully using Selective Repeat ARQ!\n");
}

void receiver(int frames[], int start, int window_size, int received[]) {
    for (int i = start; i < start + window_size && !received[i]; i++) {
        printf("Received frame: %d, sending ACK: %d\n", frames[i], frames[i] + 1);
    }
}
