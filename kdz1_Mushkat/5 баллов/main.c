#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 5000

void process1(const char *input_file, const char *fifo1) {
    int fd = open(input_file, O_RDONLY);
    if (fd == -1) {
        perror("open input file");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE);
    if (bytes_read == -1) {
        perror("read input file");
        exit(EXIT_FAILURE);
    }

    close(fd);

    int fifo_fd = open(fifo1, O_WRONLY);
    if (fifo_fd == -1) {
        perror("open fifo1");
        exit(EXIT_FAILURE);
    }

    write(fifo_fd, buffer, bytes_read);
    close(fifo_fd);
}

void process2(const char *fifo1, const char *fifo2) {
    int fifo1_fd = open(fifo1, O_RDONLY);
    if (fifo1_fd == -1) {
        perror("open fifo1");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(fifo1_fd, buffer, BUFFER_SIZE);
    if (bytes_read == -1) {
        perror("read from fifo1");
        exit(EXIT_FAILURE);
    }

    close(fifo1_fd);

    int counts[10] = {0};
    for (int i = 0; i < bytes_read; i++) {
        if (buffer[i] >= '0' && buffer[i] <= '9') {
            counts[buffer[i] - '0']++;
        }
    }

    char result[BUFFER_SIZE] = {0};
    for (int i = 0; i < 10; i++) {
        char temp[64];
        snprintf(temp, sizeof(temp), "%d: %d\n", i, counts[i]);
        strcat(result, temp);
    }

    int fifo2_fd = open(fifo2, O_WRONLY);
    if (fifo2_fd == -1) {
        perror("open fifo2");
        exit(EXIT_FAILURE);
    }

    write(fifo2_fd, result, strlen(result) + 1);
    close(fifo2_fd);
}

void process3(const char *fifo2, const char *output_file) {
    int fifo2_fd = open(fifo2, O_RDONLY);
    if (fifo2_fd == -1) {
        perror("open fifo2");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(fifo2_fd, buffer, BUFFER_SIZE);
    if (bytes_read == -1) {
        perror("read from fifo2");
        exit(EXIT_FAILURE);
    }

    close(fifo2_fd);

    int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open output file");
        exit(EXIT_FAILURE);
    }

    write(fd, buffer, bytes_read);
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    const char *fifo1 = "/tmp/fifo1";
    const char *fifo2 = "/tmp/fifo2";

    mkfifo(fifo1, 0666);
    mkfifo(fifo2, 0666);

    pid_t pid1 = fork();
    if (pid1 == 0) {
        process1(input_file, fifo1);
        exit(EXIT_SUCCESS);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        process2(fifo1, fifo2);
        exit(EXIT_SUCCESS);
    }

    pid_t pid3 = fork();
    if (pid3 == 0) {
        process3(fifo2, output_file);
        exit(EXIT_SUCCESS);
    }

    wait(NULL);
    wait(NULL);
    wait(NULL);

    unlink(fifo1);
    unlink(fifo2);

    return 0;
}