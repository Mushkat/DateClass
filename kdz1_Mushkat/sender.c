#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_BUF_SIZE 128
#define REQUEST_KEY_PATH "/tmp"
#define REQUEST_KEY_PROJ 'R'
#define RESPONSE_KEY_PATH "/tmp"
#define RESPONSE_KEY_PROJ 'S'

typedef struct {
    long mtype;
    char data[MAX_BUF_SIZE];
    int data_size;
} data_message;

typedef struct {
    long mtype;
    int counters[10];
} result_message;

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s <input_file> <output_file>\n", prog_name);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage(argv[0]);
    }

    key_t key_request = ftok(REQUEST_KEY_PATH, REQUEST_KEY_PROJ);
    key_t key_response = ftok(RESPONSE_KEY_PATH, RESPONSE_KEY_PROJ);

    if (key_request == -1 || key_response == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int request_qid = msgget(key_request, IPC_CREAT | 0666);
    int response_qid = msgget(key_response, IPC_CREAT | 0666);

    if (request_qid == -1 || response_qid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
        perror("open input file");
        exit(EXIT_FAILURE);
    }

    data_message d_msg;
    d_msg.mtype = 1;

    ssize_t bytes_read;
    while ((bytes_read = read(input_fd, d_msg.data, MAX_BUF_SIZE)) > 0) {
        d_msg.data_size = bytes_read;
        if (msgsnd(request_qid, &d_msg, sizeof(d_msg.data) + sizeof(d_msg.data_size), 0) == -1) {
            perror("msgsnd data");
            exit(EXIT_FAILURE);
        }
    }

    d_msg.mtype = 2;
    d_msg.data_size = 0;
    if (msgsnd(request_qid, &d_msg, sizeof(d_msg.data) + sizeof(d_msg.data_size), 0) == -1) {
        perror("msgsnd finish");
        exit(EXIT_FAILURE);
    }

    close(input_fd);

    result_message r_msg;
    if (msgrcv(response_qid, &r_msg, sizeof(r_msg.counters), 2, 0) == -1) {
        perror("msgrcv result");
        exit(EXIT_FAILURE);
    }

    int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd == -1) {
        perror("open output file");
        exit(EXIT_FAILURE);
    }

    char buffer[128];
    for (int i = 0; i < 10; i++) {
        int len = snprintf(buffer, sizeof(buffer), "Digit %d: %d\n", i, r_msg.counters[i]);
        write(output_fd, buffer, len);
    }

    close(output_fd);

    msgctl(request_qid, IPC_RMID, NULL);
    msgctl(response_qid, IPC_RMID, NULL);

    return 0;
}