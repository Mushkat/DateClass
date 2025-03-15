#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER_SIZE 5000

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

    int fifo1_fd = open(fifo1, O_WRONLY);
    if (fifo1_fd == -1) {
        perror("open fifo1");
        exit(EXIT_FAILURE);
    }

    write(fifo1_fd, buffer, bytes_read);
    close(fifo1_fd);

    int fifo2_fd = open(fifo2, O_RDONLY);
    if (fifo2_fd == -1) {
        perror("open fifo2");
        exit(EXIT_FAILURE);
    }

    ssize_t result_bytes = read(fifo2_fd, buffer, BUFFER_SIZE);
    if (result_bytes == -1) {
        perror("read from fifo2");
        exit(EXIT_FAILURE);
    }

    close(fifo2_fd);

    fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open output file");
        exit(EXIT_FAILURE);
    }

    write(fd, buffer, result_bytes);
    close(fd);

    unlink(fifo1);
    unlink(fifo2);

    return 0;
}