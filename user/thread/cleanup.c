#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#define N_THR 5
#define BUF_SZ 64
#define FILE_SZ 64

void
mem_cleanup(void *arg)
{
  printf("%s...\n", __func__);

  free(arg);

  return;
}

void
file_cleanup(void *arg)
{
  printf("%s...\n", __func__);
  
  fclose((FILE *)arg);

  return;
}

void *
wr_to_file(void *arg)
{
  char file[FILE_SZ], buf[BUF_SZ];
  int len, cnt, *n;
  FILE *fp;

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

  cnt = 0;
  /* 힙 메모리에 존재하는 arg, 스레드가 취소 신호를 받으면 메모리 해제가 필요하다. */
  n = (int *)arg;

  /* 스레드가 취소 신호를 받으면 정리 함수는 포인터 주소의 메모리를 해제한다. */
  pthread_cleanup_push(mem_cleanup, arg);

  sprintf(file, "thread_%d.txt", *n);

  fp = fopen(file, "w");

  /*
   * 열린 파일은 스레드가 취소될 때 닫혀져야 한다.
   * 즉, 파일이 성공적으로 열리면 스레드는 파일을 닫는 정리 핸들러가 필요하다.
   */  
  pthread_cleanup_push(file_cleanup, (void *)fp);

  while (true) {
    /*
     * 스레드 취소 신호가 전달되었을 때 함수(e.g., sprintf)의 내부 구현을 실행하는 도중에 스레드가 취소될 수 있다.
     * 즉, 스레드가 취소될 경우 정의되지 않은 동작으로 이어질 수 있는 위험한 지점이다.
     * 불변성(invariant) 문제를 해결하기 위해 지연 취소(deferred cancellation)를 사용한다.
     */
    len = sprintf(buf, "%d. 스레드 %d\n", cnt++, *n);
    
    fwrite(buf, sizeof(char), len, fp);
    fflush(fp);
    
    sleep(1);

    pthread_testcancel();
  }

  /*
   * 인자로 0을 전달하면 정리 스택의 맨 위에서 정리 핸들러를 제거한다.
   * 인자로 0 이외의 값을 전달하면 정리 스택의 맨 위에서 정리 핸들러를 제거하고 호출한다.
   */ 
  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);

  /*
   * 스레드가 시작 루틴에서 반환하여 종료되면 정리 핸들러가 호출되지 않지만 이 동작은 구현에 따라 다를 수 있다.
   * 만약 파일을 열 수 없으면 arg 변수를 해제해야 하는데 스레드가 반환문(return (void *)-1;)을 통해 종료될 때 스레드 정리 스택에 있는 정리 핸들러는 호출되지 않는다.
   * 하지만, pthread_exit() 함수를 사용하여 스레드를 종료하면 정리 핸들러가 호출된다.
   */
  pthread_exit(NULL);
}

int
main(int argc, char *argv[])
{
  pthread_t tids[N_THR];
  int i, id, ch, *n;

  for (i = 0; i < N_THR; i++) {
    n = calloc(1, sizeof(*n));
    *n = i;
    
    pthread_create(&tids[i], NULL, wr_to_file, (void *)n);
  }

  while (true) {
    printf("1. 스레드를 취소한다.\n");
    scanf("%d", &ch);

    printf("스레드 번호. [0-%d]: ", N_THR - 1);
    scanf("%d", &id);

    if (id < 0 || id >= N_THR) {
      printf("유효하지 않은 스레드 번호!\n");
      continue;
    }

    switch (ch) {
      case 1:
        pthread_cancel(tids[id]);
        break;
      default:
        continue;
    }
  }

  exit(EXIT_SUCCESS);
}