#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

#define BUF_SZ 266

int
main(int argc, char *argv[])
{
  char buf[BUF_SZ];
  mqd_t mqd;

  if (argc <= 1) {
    fputs("메시지 큐 이름(형식: </mq-name>)\n", stderr);
    exit(EXIT_FAILURE);
  }

  memset(buf, 0, BUF_SZ);
  printf("송신할 메시지: ");
  scanf("%s", buf);

  if ((mqd = mq_open(argv[1], O_CREAT | O_WRONLY, 0, 0)) == -1) {
    fputs("mq_open()\n", stderr);
    exit(EXIT_FAILURE);
  }

  if (mq_send(mqd, buf, strlen(buf) + 1, 0) == -1) {
    fputs("mq_send()\n", stderr);
    exit(EXIT_FAILURE);
  } 

  mq_close(mqd);

  exit(EXIT_SUCCESS);
}