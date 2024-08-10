#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

#define BUF_SZ 266

int
main(int argc, char *argv[])
{
  int flags;
  char buf[BUF_SZ];
  mqd_t mqd;

  if (argc != 2) {
    fputs("사용법: /<메시지 큐 이름>\n", stderr);
    exit(EXIT_FAILURE);
  }

  memset(buf, 0, BUF_SZ);
  printf("송신할 메시지: ");
  scanf("%s", buf);

  flags = O_CREAT | O_WRONLY;
  if ((mqd = mq_open(argv[1], flags, NULL, NULL)) == -1) {
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