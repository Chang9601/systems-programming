#ifndef _RW_LOCK
#define _RW_LOCK

#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

struct rw_lock {
  /* 상호 배제적인 방식으로 읽기/쓰기 잠금의 상태를 조작 및 검사하는 뮤텍스. */
  pthread_mutex_t mutex;
  /* 잠금을 사용할 수 없을 때 스레드를 차단하는 조건 변수. */
  pthread_cond_t cv;
  /* 임계 영역 내에서 동시에 실행 중인 스레드 수. */
  uint16_t n_locks;
  /* 잠금 부여를 기다리는 읽기 스레드의 수. */
  uint16_t n_reader_wait;
  /* 잠금 부여를 기다리는 쓰기 스레드의 수. */
  uint16_t n_writer_wait;
  /* 읽기 스레드에 의해 잠겨진 여부. */
  bool is_locked_by_reader;
  /* 쓰기 스레드에 의해 잠겨진 여부. */
  bool is_locked_by_writer;
  /* 잠금을 가진 스레드가 쓰기 스레드인 경우 스레드의 아이디. */
  pthread_t writer_tid;
};

void
init_rw_lock(struct rw_lock *rw_lock);

void
lock_rd_lock(struct rw_lock *rw_lock);

void
lock_wr_lock(struct rw_lock *rw_lock);

void
unlock_rw_lock(struct rw_lock *rw_lock);

void
destroy_rw_lock(struct rw_lock *rw_lock);

#endif