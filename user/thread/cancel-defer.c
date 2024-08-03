#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#define N_THR   5
#define BUF_SZ  64
#define FILE_SZ 64

void *
wr_to_file(void *arg)
{
  char file[FILE_SZ], buf[BUF_SZ];
  int len, cnt, *n;
  FILE *fp;

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

  cnt = 0;
  n = (int *)arg;

  sprintf(file, "thread_%d.txt", *n);

  fp = fopen(file, "w");

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

  return NULL;
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