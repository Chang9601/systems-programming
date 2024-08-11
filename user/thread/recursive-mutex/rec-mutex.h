#ifndef _REC_MUTEX_H
#define _REC_MUTEX_H

#include <stdint.h>
#include <pthread.h>

struct rec_mutex {
  /* 스레드가 뮤텍스를 잠근 횟수(오직 하나의 스레드만이 재귀 뮤텍스를 여러 번 잠글 수 있다.). */
  uint16_t n_locked;
  /* 뮤텍스를 잠근 스레드의 아이디. */
  pthread_t tid;
  /* 잠근 뮤텍스를 다른 스레드가 잠그려고 할 경우 대기하는 조건 변수. */
  pthread_cond_t cv;
  /* 재귀 뮤텍스의 상태를 상호 배제적으로 변경하는 뮤텍스. */
  pthread_mutex_t mutex;
  /* 뮤텍스에 대기되어 있는 스레드의 개수. */
  uint16_t n_blocked;
};

void
init_rec_mutex(struct rec_mutex *rec_mutex);

void
lock_rec_mutex(struct rec_mutex *rec_mutex);

void
unlock_rec_mutex(struct rec_mutex *rec_mutex);

void
destroy_rec_mutex(struct rec_mutex *rec_mutex);

#endif