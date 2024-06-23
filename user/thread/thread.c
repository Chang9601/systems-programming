#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

void *
thr_fn(void *arg)
{
  pid_t pid;
  pthread_t tid;
  char *tname;

  pid = getpid();
  tid = pthread_self();
  tname = (char *)arg;

  while (true) {
    printf("프로세스 아이디: %lu\t스레드 아이디: %lu\t스레드 인자: %s\n", (unsigned long)pid, (unsigned long)tid, tname);
    sleep(1);
  }

  return NULL;
}

int 
main(int args, char *argv[])
{
  pthread_t tid;
  char *tname = "xel";

  /*
   * 스레드 분기 지점.
   * 4번 인자는 반드시 힙 영역 메모리 혹은 데이터 영역 메모리.
   */
  pthread_create(&tid, NULL, thr_fn, (void *)tname);

  printf("메인 스레드(%lu) 종료.\n", (unsigned long)pthread_self());
  pthread_exit(NULL);

  /* 
   * 해당 코드는 프로세스의 종료에 직접적인 영향을 미치지 않는다. 
   * 메인 스레드가 자식 스레드를 생성한 직후에 pthread_exit() 함수를 호출하기 때문이다.
   */
  exit(EXIT_SUCCESS);
}