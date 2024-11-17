#ifndef _THR_BARRIER_H
#define _THR_BARRIER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

struct thr_barrier {
  uint32_t threshold_cnt; /* 임계치 개수 */
  uint32_t curr_wait_cnt; /* 현재 대기 개수는 스레드 장벽에서 대기하는 스레드의 수로 값의 범위는 [0, threshold_cnt-1]. */
  pthread_mutex_t mutex; /* 스레드 장벽은 여러 스레드가 공유하는 자료구조이기 때문에 스레드 장벽에 대한 모든 작업을 상호 배제적인 방식으로 수행해야 한다. */
  pthread_cond_t cv; /* 스레드 장벽의 주요 기능은 스레드를 대기하도록 하는 것이므로 스레드가 대기될 조건 변수가 필요하다. */
  bool is_ready; /* 스레드 장벽 처리가 진행 중인지 여부를 추적하며 기본적으로 true이고 처리가 진행 중일 때는 false로 설정되며 처리가 완료되면 다시 true로 설정된다. */
  pthread_cond_t busy_cv; // 스레드 장벽이 처리 중인 경우 즉, 진행 단계에 있는 경우에 어떤 스레드도 스레드 장벽을 사용하지 못하도록하기 위한 조건 변수이다. */
};

void
thr_barrier_init(struct thr_barrier *barrier, uint32_t threshold_cnt);

void
thr_barrier_wait(struct thr_barrier *barrier);

void
thr_barrier_destroy(struct thr_barrier *barrier);

void
thr_barrier_print(struct thr_barrier *barrier);

#endif