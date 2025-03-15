#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 5000

void process1(int pipe1[], int pipe2[], const char *input_file, const char *output_file) {
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

    close(pipe1[0]);
    write(pipe1[1], buffer, bytes_read);
    close(pipe1[1]);

    close(pipe2[1]);
    ssize_t result_bytes = read(pipe2[0], buffer, BUFFER_SIZE);
    if (result_bytes == -1) {
        perror("read from pipe2");
        exit(EXIT_FAILURE);
    }

    close(pipe2[0]);

    fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open output file");
        exit(EXIT_FAILURE);
    }

    write(fd, buffer, result_bytes);
    close(fd);
}

void process2(int pipe1[], int pipe2[]) {
    close(pipe1[1]);
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(pipe1[0], buffer, BUFFER_SIZE);
    if (bytes_read == -1) {
        perror("read from pipe1");
        exit(EXIT_FAILURE);
    }

    close(pipe1[0]);

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

    close(pipe2[0]);
    write(pipe2[1], result, strlen(result) + 1);
    close(pipe2[1]);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    int pipe1[2], pipe2[2];

    if (pipe(pipe1) == -1) {
        perror("pipe1");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipe2) == -1) {
        perror("pipe2");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == 0) {
        process2(pipe1, pipe2);
        exit(EXIT_SUCCESS);
    } else if (pid > 0) {
        process1(pipe1, pipe2, input_file, output_file);
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    return 0;
}