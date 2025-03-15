#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER_SIZE 128

int main() {
    const char *fifo1 = "/tmp/fifo1";
    const char *fifo2 = "/tmp/fifo2";

    int fifo1_fd = open(fifo1, O_RDONLY);
    if (fifo1_fd == -1) {
        perror("open fifo1");
        exit(EXIT_FAILURE);
    }

    int fifo2_fd = open(fifo2, O_WRONLY);
    if (fifo2_fd == -1) {
        perror("open fifo2");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    int counts[10] = {0};

    while ((bytes_read = read(fifo1_fd, buffer, BUFFER_SIZE))) {
        if (bytes_read == -1) {
            perror("read from fifo1");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < bytes_read; i++) {
            if (buffer[i] >= '0' && buffer[i] <= '9') {
                counts[buffer[i] - '0']++;
            }
        }
    }

    char result[BUFFER_SIZE] = {0};
    for (int i = 0; i < 10; i++) {
        char temp[64];
        snprintf(temp, sizeof(temp), "%d: %d\n", i, counts[i]);
        strcat(result, temp);
    }

    if (write(fifo2_fd, result, strlen(result) + 1) == -1) {
        perror("write to fifo2");
        exit(EXIT_FAILURE);
    }

    close(fifo1_fd);
    close(fifo2_fd);

    return 0;
}