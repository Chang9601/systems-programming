#include <assert.h>
#include "rw-lock.h"


void
init_rw_lock(struct rw_lock *rw_lock)
{
  pthread_mutex_init(&rw_lock->mutex, NULL);
  pthread_cond_init(&rw_lock->cv, NULL);
  rw_lock->n_locks = rw_lock->n_reader_wait = rw_lock->n_writer_wait = 0;
  rw_lock->is_locked_by_reader = rw_lock->is_locked_by_writer = false;
}

void
lock_rd_lock(struct rw_lock *rw_lock)
{
  pthread_mutex_lock(&rw_lock->mutex);

  /*
   * 1. 읽기-쓰기 잠금이 사용 가능한 경우.
   * 2. 읽기 쓰레드가 읽기-쓰기 잠금을 잠금 경우.
   * 3. 쓰기 쓰레드가 읽기-쓰기 잠금을 잠금 경우.
   */

  /* 경우 1. */
  if (!rw_lock->is_locked_by_reader && !rw_lock->is_locked_by_writer) {
    assert(rw_lock->n_locks == 0);
    assert(rw_lock->is_locked_by_reader == false);
    assert(rw_lock->is_locked_by_writer == false);
    assert(rw_lock->writer_tid == 0);

    rw_lock->n_locks++;
    rw_lock->is_locked_by_reader = true;

    pthread_mutex_unlock(&rw_lock->mutex);
    return;
  }

  /* 경우 2. */
  if (rw_lock->is_locked_by_reader) {
    assert(rw_lock->is_locked_by_writer == false);
    assert(rw_lock->n_locks != 0);
    assert(rw_lock->writer_tid == 0);

    rw_lock->n_locks++;

    pthread_mutex_unlock(&rw_lock->mutex);
    return;

  }

  /* 경우 3. */
  while (rw_lock->is_locked_by_writer) {
    assert(rw_lock->n_locks != 0);
    assert(rw_lock->is_locked_by_reader == false);
    assert(rw_lock->writer_tid != 0);

    rw_lock->n_reader_wait++;
    pthread_cond_wait(&rw_lock->cv, &rw_lock->mutex);
    rw_lock->n_reader_wait--;
  }

  assert(rw_lock->is_locked_by_writer == false);
  assert(rw_lock->writer_tid != 0);
  
  if (!rw_lock->n_locks) 
    rw_lock->is_locked_by_reader = true;
  
  rw_lock->n_locks++;

  pthread_mutex_unlock(&rw_lock->mutex);
}

void
lock_wr_lock(struct rw_lock *rw_lock)
{
  pthread_mutex_lock(&rw_lock->mutex);

  /*
   * 1. 읽기-쓰기 잠금이 사용 가능한 경우.
   * 2. 같은 쓰기 쓰레드가 읽기-쓰기 잠금을 잠금 경우.
   * 3. 다른 쓰레드가 읽기-쓰기 잠금을 잠금 경우.
   */

  /* 경우 1. */
  if (!rw_lock->is_locked_by_reader && !rw_lock->is_locked_by_writer) {
    assert(rw_lock->n_locks == 0);
    assert(rw_lock->is_locked_by_reader == false);
    assert(rw_lock->is_locked_by_writer == false);
    assert(rw_lock->writer_tid == 0);

    rw_lock->n_locks++;
    rw_lock->is_locked_by_writer = true;
    rw_lock->writer_tid = pthread_self();

    pthread_mutex_unlock(&rw_lock->mutex);
    return;
  }

  /* 경우 2. */
  if (rw_lock->is_locked_by_writer && rw_lock->writer_tid == pthread_self()) {
    assert(rw_lock->is_locked_by_reader == false);
    assert(rw_lock->n_locks != 0);

    rw_lock->n_locks++;

    pthread_mutex_unlock(&rw_lock->mutex);
    return;
  }

  /* 경우 3. */
  while (rw_lock->is_locked_by_reader || rw_lock->is_locked_by_writer) {
    if (rw_lock->is_locked_by_reader) {
      assert(rw_lock->is_locked_by_writer == false);
      assert(rw_lock->n_locks != 0);
      assert(rw_lock->writer_tid == 0);

    } else if (rw_lock->is_locked_by_writer) {
      assert(rw_lock->is_locked_by_reader == false);
      assert(rw_lock->n_locks != 0);
      assert(rw_lock->writer_tid != 0);
    }

    rw_lock->n_writer_wait++;
    pthread_cond_wait(&rw_lock->cv, &rw_lock->mutex);
    rw_lock->n_writer_wait--;
  }
  
  assert(rw_lock->n_locks == 0);
  assert(rw_lock->is_locked_by_reader == false);
  assert(rw_lock->is_locked_by_writer == false);
  assert(rw_lock->writer_tid == 0);

  rw_lock->is_locked_by_writer = true;
  rw_lock->n_locks++;
  rw_lock->writer_tid = pthread_self();

  pthread_mutex_unlock(&rw_lock->mutex);
}

void
unlock_rw_lock(struct rw_lock *rw_lock)
{
  /*
   * 1. 풀린 잠금을 푸는 경우
   * 2. 쓰기 스레드가 읽기-쓰기 잠금을 푸는 경우
   * 3. 읽기 스레드가 읽기-쓰기 잠금을 푸는 경우
   */

  /* 경우 1. */
  assert(rw_lock->n_locks);

  /* 경우 2. */
  if (rw_lock->is_locked_by_writer) {
    assert(pthread_self() == rw_lock->writer_tid);

    assert(rw_lock->is_locked_by_reader == false);

    rw_lock->n_locks--;
    
    if (rw_lock->n_locks) {
      pthread_mutex_unlock(&rw_lock->mutex);
      return;
    }

    if (rw_lock->n_reader_wait || rw_lock->n_writer_wait) {
      pthread_cond_broadcast(&rw_lock->cv);
    }

    rw_lock->is_locked_by_writer = false;
    rw_lock->writer_tid = 0;
    pthread_mutex_unlock(&rw_lock->mutex);
    return;    
  }

  /* 경우 3. */
  if (rw_lock -> is_locked_by_reader) {
    assert(rw_lock->is_locked_by_writer == false);
    assert(rw_lock->writer_tid == 0);

    rw_lock->n_locks--;

    if (rw_lock->n_locks) {
      pthread_mutex_unlock(&rw_lock->mutex);
      return;
    }

    if (rw_lock->n_reader_wait || rw_lock->n_writer_wait) {
      pthread_cond_broadcast(&rw_lock->cv);
    }

    rw_lock->is_locked_by_reader = false;
    pthread_mutex_unlock(&rw_lock->mutex);
  }
  
}

void
destroy_rw_lock(struct rw_lock *rw_lock)
{
  assert(rw_lock->n_locks == 0);
  assert(rw_lock->n_reader_wait == 0);
  assert(rw_lock->n_writer_wait == 0);
  assert(rw_lock->is_locked_by_reader == false);
  assert(rw_lock->is_locked_by_writer == false);
  pthread_mutex_destroy(&rw_lock->mutex);
  pthread_cond_destroy(&rw_lock->cv);
}