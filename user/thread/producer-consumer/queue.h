#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

#define QUEUE_SZ 5

struct queue {
  void *elem[QUEUE_SZ];
  unsigned int front;
  unsigned int rear;
  unsigned int cnt;
  pthread_mutex_t mutex;

  /*
   * 뮤텍스는 항상 자원의 속성이지만 조건 변수는 자원의 속성 또는 스레드의 속성일 수 있다.
   * 자원 조건 변수 vs. 스레드 조건 변수
   * 1. 스레드가 동일한 조건 변수를 공유하는 경우 해당 조건 변수는 자원의 속성이다.
   * 2. 스레드가 각자 고유한 조건 변수를 선택하는 경우 해당 조건 변수는 각 스레드의 속성이다.
   */
  pthread_cond_t cv;
};

struct queue *
init_queue(void);

void
destroy_queue(struct queue *q);

bool
is_queue_empty(struct queue *q);

bool
is_queue_full(struct queue *q);

bool
enqueue(struct queue *q, void *ptr);

void *
dequeue(struct queue *q);

#endif
