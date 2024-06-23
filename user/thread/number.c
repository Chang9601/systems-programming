#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int N;

void *
thr_fn(void *arg) 
{
  pthread_t next_tid;
  int n;

  n = *(int *)arg;
  
  free(arg);

  if (n < N) {
    int *next_n;
    
    next_n = calloc(1, sizeof(*next_n));
    *next_n = n + 1;

    /* 1번 스레드는 2번 스레드를, 2번 스레드는 3번 스레드를, N-1번 스레드는 N번 스레드를 생성한다. */
    pthread_create(&next_tid, NULL, thr_fn, (void *)next_n);
    /* 2번 스레드는 1번 스레드에, 3번 스레드는 2번 스레드에, ... N번 스레드는 N-1번 스레드에 결합한다. */
    pthread_join(next_tid, NULL);
  }

  printf("%d\n", n);

  return NULL;
}

void
num(int n)
{
  pthread_t tid;
  int *_n;

  _n = calloc(1, sizeof(*_n));
  *_n = n;

  /* 1번 스레드를 생성한다. */
  pthread_create(&tid, NULL, thr_fn, (void *)_n);
  /* 1번 스레드가 메인 스레드에 결합한다. */
  pthread_join(tid, NULL);

  return;
}

int
main(int argc, char *argv[]) 
{
  int n;

  n = 1;
  
  scanf("%d", &N);
  num(n);
  
  exit(EXIT_SUCCESS);
}