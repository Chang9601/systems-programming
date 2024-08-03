#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>

/*
 * 1로 초기화되는 이진 세마포어로 이는 단일 자원을 제어하며 뮤텍스와 동일한다.
 * 하지만 뮤텍스와 달리 세마포어는 다른 프로세스/스레드에 의해 대기가 풀릴 수 있다.
 */
sem_t sem;
pthread_t tids[5];

#define SEM_VAL 2

void *
fn(void *arg)
{
  int i;
	char *tname;

  tname = (char *)arg;

  sem_wait(&sem);
  printf("%s가 임계 영역에 들어간다.\n", tname);

  for (i = 0; i < 5; i++) {
    printf("임계 영역에서 %s 실행 중.\n", tname);
    sleep(1);
  }

  sem_post(&sem);
  printf("%s가 임계 영역을 나간다.\n", tname);

  return NULL;
}

int
main(int argc, char *argv[])
{
  int i;
  char *tnames[] = {"스레드1", "스레드2", "스레드3", "스레드4", "스레드5"};

  sem_init(&sem, 0, SEM_VAL);

  for (i = 0; i < 5; i++)
    pthread_create(&tids[i], NULL, fn, tnames[i]);

  for (i = 0; i < 5; i++)
    pthread_join(tids[i], NULL);

  sem_destroy(&sem);

  exit(EXIT_SUCCESS);
}