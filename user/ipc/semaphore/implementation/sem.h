#ifndef _SEM_H
#define _SEM_H

#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

struct sem {
  int cnt;
  pthread_cond_t cv;
  pthread_mutex_t mutex;
};

struct sem *
get_sem();

void
init_sem(struct sem *sem, int count);

void
wait_sem(struct sem *sem);

void
post_sem(struct sem *sem);

void
destroy_sem(struct sem *sem);

int
get_cnt(struct sem *sem);

#define V(sem)    post_sem(sem)
#define P(sem)    wait_sem(sem)

#define UP(sem)   post_sem(sem)
#define DOWN(sem) wait_sem(sem)

#endif
