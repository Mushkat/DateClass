#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

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

int main() {
    key_t key_request = ftok(REQUEST_KEY_PATH, REQUEST_KEY_PROJ);
    if (key_request == -1) {
        perror("ftok request key");
        exit(EXIT_FAILURE);
    }

    key_t key_response = ftok(RESPONSE_KEY_PATH, RESPONSE_KEY_PROJ);
    if (key_response == -1) {
        perror("ftok response key");
        exit(EXIT_FAILURE);
    }

    int request_qid = msgget(key_request, IPC_CREAT | 0666);
    if (request_qid == -1) {
        perror("msgget request queue");
        exit(EXIT_FAILURE);
    } else {
        printf("Request queue created with ID: %d\n", request_qid);
    }

    int response_qid = msgget(key_response, IPC_CREAT | 0666);
    if (response_qid == -1) {
        perror("msgget response queue");
        exit(EXIT_FAILURE);
    } else {
        printf("Response queue created with ID: %d\n", response_qid);
    }

    int counters[10] = {0};
    data_message d_msg;
    result_message r_msg;
    r_msg.mtype = 2;

    while (1) {
        if (msgrcv(request_qid, &d_msg, sizeof(d_msg.data) + sizeof(d_msg.data_size), 0, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        if (d_msg.mtype == 2) {
            break;
        } else if (d_msg.mtype == 1) {
            for (int i = 0; i < d_msg.data_size; i++) {
                char c = d_msg.data[i];
                if (c >= '0' && c <= '9') {
                    counters[c - '0']++;
                }
            }
        }
    }

    memcpy(r_msg.counters, counters, sizeof(counters));
    if (msgsnd(response_qid, &r_msg, sizeof(r_msg.counters), 0) == -1) {
        perror("msgsnd result");
        exit(EXIT_FAILURE);
    }

    return 0;
}