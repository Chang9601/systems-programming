#include <assert.h>
#include "rec-mutex.h"

void
init_rec_mutex(struct rec_mutex *rec_mutex)
{
  rec_mutex->n_locked = rec_mutex->tid = rec_mutex->n_blocked = 0;
  pthread_cond_iniit(&rec_mutex->cv, NULL);
  pthread_mutex_init(&rec_mutex->mutex, NULL);
}

void
lock_rec_mutex(struct rec_mutex *rec_mutex)
{
  /*
   * 1. 재귀 뮤텍스가 잠기지 않은 경우(즉, 함수를 호출한 스레드가 잠금을 얻는 경우.).
   * 2. 재귀 뮤텍스가 이미 잠긴 경우(즉, 함수를 호출한 스레드가 재귀 뮤텍스를 이미 잠근 경우.).
   * 3. 재귀 뮤텍스가 이미 다른 스레드에 의해 잠긴 경우(즉, 함수를 호출한 스레드가 아니라 다른 스레드가 재귀 뮤텍스를 이미 잠근 경우.).
   */
  pthread_mutex_lock(&rec_mutex->mutex);

  /* 경우 1. */
  if (rec_mutex->n_locked == 0) {
    assert(rec_mutex->tid == 0);
    rec_mutex->n_locked = 1;
    rec_mutex->tid = pthread_self();
    pthread_mutex_unlock(&rec_mutex->mutex);

    return;
  }

  /* 경우 2. */
  if (rec_mutex->tid == pthread_self()) {
    assert(rec_mutex->n_locked != 0);
    rec_mutex->n_locked++;
    pthread_mutex_unlock(&rec_mutex->mutex);

    return;
  }

  /* 경우 3. */
  while (rec_mutex->tid != 0 && rec_mutex->tid != pthread_self()) {
    rec_mutex->n_blocked++;
    pthread_cond_wait(&rec_mutex->cv, &rec_mutex->mutex);
    rec_mutex->n_blocked--;
  }

  /* 함수를 실행하는 스레드가 재귀 뮤텍스에 대한 잠금을 받는다. */
  assert(rec_mutex->n_locked == 0);
  assert(rec_mutex->tid == 0);

  rec_mutex->n_locked = 1;
  rec_mutex->tid = pthread_self();
  pthread_mutex_unlock(&rec_mutex->mutex);
}

void
unlock_rec_mutex(struct rec_mutex *rec_mutex)
{
  /*
   * 1. 재귀 뮤텍스가 잠기지 않은 경우(즉, 함수를 호출한 스레드가 잠금을 얻는 경우.).
   * 2. 재귀 뮤텍스가 이미 잠긴 경우(즉, 함수를 호출한 스레드가 재귀 뮤텍스를 이미 잠근 경우.).
   * 3. 재귀 뮤텍스가 이미 다른 스레드에 의해 잠긴 경우(즉, 함수를 호출한 스레드가 아니라 다른 스레드가 재귀 뮤텍스를 이미 잠근 경우.).
  */

  pthread_mutex_lock(&rec_mutex->mutex);

  /* 경우 1. */
  if (rec_mutex->n_locked == 0) {
    assert(rec_mutex->tid == 0);
    /* 잠겨있지 않은 재귀 뮤텍스를 해제하려고 하면 프로그램이 고장난다. */
    assert(0); 
  }

  /* 경우 2. */
  if (rec_mutex->tid == pthread_self()) {
    assert(rec_mutex->n_locked != 0);

    rec_mutex->n_locked--;

    /* 만약 잠금 횟수가 0보다 클 경우, 스레드가 재귀 뮤텍스에 대한 잠금을 보유하고 있다는 것을 의미한다. 따라서 뮤텍스를 해제한다. */
    if (rec_mutex->n_locked > 0) {
      pthread_mutex_unlock(&rec_mutex->mutex);
      
      return;
    }

    /*
     * 스레드가 재귀 뮤텍스에 대한 모든 잠금을 해제한 경우, 재귀 뮤텍스를 잠그기 위해 대기된 다른 스레드가 있는지 여부를 확인해야 한다. 
     * 그러한 경우 스레드는 신호를 발생시켜 조건 변수에 블록된 스레드 중 정확히 하나의 스레드가 실행을 시작할 수 있도록 한다.
     */
    if (rec_mutex->n_blocked != 0)
      pthread_cond_signal(&rec_mutex->cv);

    /* 스레드의 작업이 완료되었기에 재귀 뮤텍스를 해제된 것으로 표시한다. */
    rec_mutex->tid = 0;
    pthread_mutex_unlock(&rec_mutex->mutex);

    return;
  }  

  /* 경우 3. */
  while (rec_mutex->tid != 0 && rec_mutex->tid != pthread_self())
    assert(0);
}

void
destroy_rec_mutex(struct rec_mutex *rec_mutex)
{
  assert(rec_mutex->n_locked == 0);
  assert(rec_mutex->tid == 0);
  assert(rec_mutex->n_blocked == 0);
  /*
   * 3개의 모든 조건이 만족되는 경우에만 조건 변수와 뮤텍스를 파괴할 수 있다. 
   * 즉, 뮤텍스와 조건 변수를 아무런 스레드에 의해 사용되지 않을 경우에만 파괴해야 하며 그렇지 않으면 정의되지 않은 동작으로 이어질 수 있다.
   */
  pthread_mutex_destroy(&rec_mutex->mutex);
  pthread_cond_destroy(&rec_mutex->cv);
}