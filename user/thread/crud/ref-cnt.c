#include <stdbool.h>
#include <pthread.h>
#include "ref-cnt.h"

void
ref_cnt_init(struct ref_cnt *ref_cnt)
{
  ref_cnt->cnt = 0;
  pthread_spin_init(&ref_cnt->spinlock, PTHREAD_PROCESS_PRIVATE);
}

void
ref_cnt_inc(struct ref_cnt *ref_cnt)
{
  pthread_spin_lock(&ref_cnt->spinlock);
  ref_cnt->cnt++;
  pthread_spin_unlock(&ref_cnt->spinlock);
}

bool
ref_cnt_dec(struct ref_cnt *ref_cnt)
{
  bool zero;

  pthread_spin_lock(&ref_cnt->spinlock);
  ref_cnt->cnt--;
  zero = (ref_cnt == 0) ? true : false;
  pthread_spin_unlock(&ref_cnt->spinlock);

  return zero;
}

void
ref_cnt_destroy(struct ref_cnt *ref_cnt)
{
  assert(ref_cnt->cnt == 0);
  pthread_spin_destroy(&ref_cnt->spinlock);
}

void
thr_running(struct ref_cnt *ref_cnt)
{
  ref_cnt_inc(ref_cnt);
}

bool
thr_done(struct ref_cnt *ref_cnt)
{
  return ref_cnt_dec(ref_cnt);
}