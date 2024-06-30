#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define BUF_SZ 15

void *
fn(void *arg)
{
  pthread_t tid;
  int time, cnt;
  char *ret;

  time = *(int *)arg;
  cnt = 0;

  /* 인자가 더 이상 필요하지 않기에 메모리를 해제한다. */
  free(arg);

  while (cnt < time) {
    tid = pthread_self();
    printf("스레드(%lu) %d초간 작업 중.\n", (unsigned long)tid, time);
    sleep(1);
    cnt++;
  }

  ret = calloc(BUF_SZ, sizeof(*ret));
  sprintf(ret, "%d초 끝!", time);

  return (void *)ret;
}

// TODO: int를 void *으로 수정.
pthread_t
make_thread(int arg) 
{
  pthread_t tid;
  pthread_attr_t attr;
  int *time;
  
  pthread_attr_init(&attr);

  time = calloc(1, sizeof(*time));
  *time = arg;

  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_create(&tid, &attr, fn, (void *)time);

  pthread_attr_destroy(&attr);

  return tid;
}

static void
safe_free(void **ptr)
{
  if (ptr != NULL && *ptr != NULL) {
    free(*ptr);
    *ptr = NULL;
  }
}

int
main(int argc, char *argv[]) 
{
  pthread_t tid1, tid2;
  void *tret;

  tid1 = make_thread(3);
  tid2 = make_thread(8);

  pthread_join(tid1, &tret);
  if (tret) {
    printf("3초 작업하는 스레드(%lu)의 반환값: %s\n", (unsigned long)tid1, (char *)tret);
    safe_free(&tret);
  }
  
  pthread_join(tid2, &tret);
  if (tret) {
    printf("8초 작업하는 스레드(%lu)의 반환값: %s\n", (unsigned long)tid2, (char *)tret);
    safe_free(&tret);
  }

  exit(EXIT_SUCCESS);    
}