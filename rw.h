#ifndef RW_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

#define FALSE 0
#define TRUE  1
#define SIZE  100

#define WRITE_ITR 100000
#define READ_ITR  100000

#define MAX_BALANCE 1000000

/* global variables */
typedef struct {
        int accno;
        float balance;
} account;


/* number of reader and writer threads */
#define READ_THREADS  7
#define WRITE_THREADS 3

#endif
