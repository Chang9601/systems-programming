#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <unistd.h>

#define UNIX_SOCK_NAME "/tmp/unix_socket"
#define BUF_SZ 128
#define QUE_SZ 20

int
main(int argc, char *argv[])
{
  struct sockaddr_un un;
  int out, len, n, data, listen_sock_fd, cli_sock_fd;
  char buf[BUF_SZ];

  /* UNIX 도메인 소켓이 존재하는 경우.*/
  unlink(UNIX_SOCK_NAME);

  if ((listen_sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  memset(&un, 0, sizeof(un));
  un.sun_family = AF_UNIX;
  strncpy(un.sun_path, UNIX_SOCK_NAME, sizeof(un.sun_path) - 1);
  len = offsetof(struct sockaddr_un, sun_path) + strlen(UNIX_SOCK_NAME);

  /* bind() 함수는 운영체제에게 클라이언트 프로세스가 "/tmp/unix_socket" 소켓으로 데이터를 보내면 해당 데이터는 서버 프로세스에 전달되어야 한다고 알려준다. */
  if (bind(listen_sock_fd, (struct sockaddr *)&un, len) == -1) {
    perror("bind()");
    exit(EXIT_FAILURE);
  }

  if (listen(listen_sock_fd, QUE_SZ) == -1) {
    perror("listen()");
    exit(EXIT_FAILURE);
  }

  for (;;) {
    printf("클라이언트 연결 대기 중...\n");

    if ((cli_sock_fd = accept(listen_sock_fd, NULL, NULL)) == -1) {
      perror("accept()");
      exit(EXIT_FAILURE);
    };

    printf("클라이언트 연결.\n");

    out = 0;
    for (;;) {
      memset(buf, 0, BUF_SZ);

      if ((n = read(cli_sock_fd, buf, BUF_SZ)) == -1) {
        perror("read()");
        exit(EXIT_FAILURE);
      }

      memcpy(&data, buf, sizeof(data));
      printf("클라이언트 데이터: %d\n", data);

      if (data == 0)
        break;
      out += data;
    }

    memset(buf, 0, BUF_SZ);
    sprintf(buf, "반환값: %d", out);

    printf("클라이언트에게 데이터 전송\n");

    if ((n = write(cli_sock_fd, buf, BUF_SZ)) == -1) {
      perror("write()");
      exit(EXIT_FAILURE);
    }

    close(cli_sock_fd);
  }

  close(listen_sock_fd);
  printf("서버 종료.\n");

  exit(EXIT_SUCCESS);
}