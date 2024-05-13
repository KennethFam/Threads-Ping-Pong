#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

volatile sig_atomic_t keep_running = 1;
static volatile int error_o = 0, turn = 1;
pthread_mutex_t mutex, mutex2;
pthread_cond_t cond1, cond2;

void error_msg() {
    error_o = 1;
}

void sig_handler(int num) {
    keep_running = 0;
}

void* routine1() {
    while (true) {
        if (keep_running == 1) fprintf(stdout, "thread 1: ping thread 2\n");
        if (fflush(stdout) != 0) {
            fprintf(stderr, "stdout flush failed! Error #%d: %s\n", errno, strerror(errno));
            error_msg();
            break;
        }
        while (turn == 1 && keep_running == 1) {
            if (pthread_cond_signal(&cond2) != 0) {
                fprintf(stderr, "cond2 conditional signal failed! Error #%d: %s\n", errno, strerror(errno));
                error_msg();
                break;
            }
        }
        if (keep_running == 0) {
            if (pthread_cond_signal(&cond2) != 0) {
                fprintf(stderr, "cond2 conditional signal failed! Error #%d: %s\n", errno, strerror(errno));
                error_msg();
            }
            break;
        }
        if (pthread_cond_wait(&cond1, &mutex) != 0) {
            fprintf(stderr, "cond1 conditional wait failed! Error #%d: %s\n", errno, strerror(errno));
            error_msg();
            break;
        }
        turn = 1;
        if (keep_running == 0) {
            if (pthread_cond_signal(&cond2) != 0) {
                fprintf(stderr, "cond2 conditional signal failed! Error #%d: %s\n", errno, strerror(errno));
                error_msg();
            }
            break;
        }
        if (keep_running == 1) fprintf(stdout, "thread 1: pong! thread 2 ping received\n");
        if (fflush(stdout) != 0) {
            fprintf(stderr, "stdout flush failed! Error #%d: %s\n", errno, strerror(errno));
            error_msg();
            break;
        }
    }
    pthread_exit(NULL);
}

void* routine2() {
    while (true) {
        if (pthread_cond_wait(&cond2, &mutex2) != 0) {
            fprintf(stderr, "cond2 conditional wait failed! Error #%d: %s\n", errno, strerror(errno));
            error_msg();
            break;
        }
        turn = 2;
        if (keep_running == 0) {
            if (pthread_cond_signal(&cond1) != 0) {
                fprintf(stderr, "cond1 conditional signal failed! Error #%d: %s\n", errno, strerror(errno));
                error_msg();
            }
            break;
        }
        if (keep_running == 1) fprintf(stdout, "thread 2: pong! thread 1 ping received\n");
        if (fflush(stdout) != 0) {
            fprintf(stderr, "stdout flush failed! Error #%d: %s\n", errno, strerror(errno));
            error_msg();
            break;
        }
        if (keep_running == 1) fprintf(stdout, "thread 2: ping thread 1\n");
        if (fflush(stdout) != 0) {
            fprintf(stderr, "stdout flush failed! Error #%d: %s\n", errno, strerror(errno));
            error_msg();
            break;
        }
        if (fflush(stdout) != 0) {
            fprintf(stderr, "stdout flush failed! Error #%d: %s\n", errno, strerror(errno));
            error_msg();
            break;
        }
        while (turn == 2 && keep_running == 1) {
            if (pthread_cond_signal(&cond1) != 0) {
                fprintf(stderr, "cond1 conditional signal failed! Error #%d: %s\n", errno, strerror(errno));
                error_msg();
                break;
            }
        }
        if (keep_running == 0) {
            if (pthread_cond_signal(&cond1) != 0) {
                fprintf(stderr, "cond1 conditional signal failed! Error #%d: %s\n", errno, strerror(errno));
                error_msg();
            }
            break;
        }
    }
    pthread_exit(NULL);
}

int main() {
    signal(SIGINT, sig_handler);
    pthread_t t1, t2;
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        fprintf(stderr, "mutex initialization failed! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    if (pthread_mutex_init(&mutex2, NULL) != 0) {
        fprintf(stderr, "mutex2 initialization failed! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    if (pthread_cond_init(&cond1, NULL) != 0) {
        fprintf(stderr, "cond1 initialization failed! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    if (pthread_cond_init(&cond2, NULL) != 0) {
        fprintf(stderr, "cond2 initialization failed! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    if (pthread_create(&t1, NULL, routine1, NULL) != 0) {
        fprintf(stderr, "t1 thread creation failed! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    if (pthread_create(&t2, NULL, routine2, NULL) != 0) {
        fprintf(stderr, "t2 thread creation failed! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    if (pthread_join(t1, NULL) != 0) {
        fprintf(stderr, "t1 thread join failed! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    if (pthread_join(t2, NULL) != 0) {
        fprintf(stderr, "t2 thread creation failed! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    if (pthread_cond_destroy(&cond1) != 0) {
        fprintf(stderr, "Destroying cond1 failed! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    if (pthread_cond_destroy(&cond2) != 0) {
        fprintf(stderr, "Destroying cond2 failed! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    if (pthread_mutex_destroy(&mutex) < 0) {
        fprintf(stderr, "Destroying mutex failed! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    if (pthread_mutex_destroy(&mutex2) < 0) {
        fprintf(stderr, "Destroying mutex2 failed! Error #%d: %s\n", errno, strerror(errno));
        return 1;
    }
    if (error_o == 1) {
        return 1;
    }
    return 0;
}