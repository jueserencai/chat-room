
// 生产者-消费者 模型。通过有限缓冲区，来安排并发线程的处理工作

#ifndef SBUF_H
#define SBUF_H

#include <semaphore.h>

typedef struct {
    int *buf;
    int n;
    int front;
    int rear;
    sem_t mutex;
    sem_t slots;
    sem_t items;
} Sbuf;

void sbuf_init(Sbuf *sp, int n);
void sbuf_deinit(Sbuf *sp);
void sbuf_insert(Sbuf *sp, int item);
int sbuf_remove(Sbuf *sp);

#endif