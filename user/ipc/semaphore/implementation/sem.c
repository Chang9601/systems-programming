#include "sem.h"

struct sem *
get_sem()
{
  struct sem *sem = calloc(1, sizeof(*sem));

  return sem;
}

void
init_sem(struct sem *sem, int cnt)
{
  sem->cnt = cnt;
  pthread_mutex_init(&sem->mutex, NULL);
  pthread_cond_init(&sem->cv, NULL);
}

void
wait_sem(struct sem *sem)
{
  pthread_mutex_lock(&sem->mutex);

  if (sem->cnt <= 0)
    pthread_cond_wait(&sem->cnt, &sem->mutex);

  sem->cnt--;

  pthread_mutex_unlock(&sem->mutex);
}

void
post_sem(struct sem *sem)
{
  bool blocked_thread;

  pthread_mutex_lock(&sem->mutex);

  blocked_thread = sem->cnt <= 0 ? true : false;
  sem->cnt++;

  if (blocked_thread)
    pthread_cond_signal(&sem->cv);

  pthread_mutex_unlock(&sem->mutex);
}

void
destroy_sem(struct sem *sem)
{
  sem->cnt = 0;
  pthread_mutex_unlock(&sem->mutex);
  pthread_mutex_destroy(&sem->mutex);
  pthread_cond_destroy(&sem->cv);
}

int
get_cnt(struct sem *sem)
{
  return sem->cnt;
}