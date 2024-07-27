#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <mqueue.h>
#include <sys/select.h>

#define MSG_N 10
#define MSG_SZ 256
#define BUF_SZ (MSG_SZ + 10)

int
main(int argc, char *argv[])
{
  fd_set read_fds;
  char buf[BUF_SZ];
  mqd_t mqd;
  mode_t perms;
  struct mq_attr attr;

  if (argc <= 1) {
    fputs("메시지 큐 이름(형식: </mq-name>)\n", stderr);
    exit(EXIT_FAILURE);
  }

  perms = 0660;
  attr.mq_flags = 0;
  attr.mq_maxmsg = MSG_N;
  attr.mq_msgsize = MSG_SZ;
  attr.mq_curmsgs = 0;

  if ((mqd = mq_open(argv[1], O_CREAT | O_RDONLY, perms, &attr)) == -1) {
    fputs("mq_open()\n", stderr);
    exit(EXIT_FAILURE);
  }

  while (true) {
    FD_ZERO(&read_fds);
    FD_SET(mqd, &read_fds);
    
    printf("메시지 도착 대기 중...\n");
    
    select(mqd + 1, &read_fds, NULL, NULL, NULL);
    if (FD_ISSET(mqd, &read_fds)) {
      printf("메시지 큐 이름: %s\n", argv[1]);

      memset(buf, 0, MSG_SZ);
      if (mq_receive(mqd, buf, MSG_SZ, NULL) == -1) {
        fputs("mq_receive()\n", stderr);
        exit(EXIT_FAILURE);
      }

      printf("수신 메시지: %s\n", buf);
    }

  }

  mq_close(mqd);

  exit(EXIT_SUCCESS);
}