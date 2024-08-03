#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <unistd.h>

#define UNIX_SOCK_NAME  "/tmp/unix_socket"
#define BUF_SZ          128

int
main(int argc, char *argv[])
{
  struct sockaddr_un un;
  int len, n, num, sock_fd;
  char buf[BUF_SZ];

  if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  memset(&un, 0, sizeof(un));
  un.sun_family = AF_UNIX;
  strncpy(un.sun_path, UNIX_SOCK_NAME, sizeof(un.sun_path) - 1);
  len = offsetof(struct sockaddr_un, sun_path) + strlen(UNIX_SOCK_NAME);

  if (connect(sock_fd, (struct sockaddr *)&un, len) == -1) {
    perror("connect()");
    exit(EXIT_FAILURE);
  }

  do {
    printf("서버에 전송할 데이터: ");
    scanf("%d", &num);
    
    if ((n = write(sock_fd, &num, sizeof(num))) == -1) {
      perror("write()");
      break;
    }

    printf("%d바이트 전송, %d 전송\n", n, num);
  } while (num);

  memset(buf, 0, BUF_SZ);
  if ((n = read(sock_fd, buf, BUF_SZ)) == -1) {
    perror("read()");
    exit(EXIT_FAILURE);
  }

  buf[BUF_SZ - 1] = '\0';
  printf("%s\n", buf);

  close(sock_fd);
  
  exit(EXIT_SUCCESS);
}