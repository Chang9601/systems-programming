#include <stdio.h>
#include <stdlib.h>
#include "barrier.h"

struct thr_barrier barrier;
pthread_t tids[3];

void *
fn(void *arg)
{
  /*
   * 스레드가 스레드 장벽에서 나가는 순서는 무작위일 수 있으며 이는 중요하지 않다.
   * 중요한 것은 모든 스레드가 1번 스레드 장벽을 통과한 다음 모든 스레드가 2번 스레드 장벽을 통과하고 즉, 통과하는 순서이다.
   */
  char *tname = (char *)arg;

  /*
   * 모든 스레드가 스레드 장벽에 대기된다.
   * 즉, 스레드가 모두 스레드 장벽 지점에 도달할 때까지 대기된다.
   */
  thr_barrier_wait(&barrier);
  /* 대기된 스레드의 개수만큼 출력문이 출력된다. */
  printf("%s가 1번 스레드 장벽을 지나간다.\n", tname);

  thr_barrier_wait(&barrier);
  printf("%s가 2번 스레드 장벽을 지나간다.\n", tname);

  thr_barrier_wait(&barrier);
  printf("%s가 3번 스레드 장벽을 지나간다.\n", tname);

  return NULL;
}

int 
main(int argc, char *argv[])
{
  const char *tname1 = "스레드1";
  const char *tname2 = "스레드2";
  const char *tname3 = "스레드3";

  thr_barrier_init(&barrier, 3);

	pthread_create(&tids[0], NULL, fn, (void *)tname1);
	pthread_create(&tids[1], NULL, fn, (void *)tname2);
	pthread_create(&tids[2], NULL, fn, (void *)tname3);

  pthread_join(tids[0], NULL);
  pthread_join(tids[1], NULL);
  pthread_join(tids[2], NULL);

  thr_barrier_print(&barrier);
  thr_barrier_destroy(&barrier);

  exit(EXIT_SUCCESS);
}