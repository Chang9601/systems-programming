#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <stddef.h>
#include <unistd.h>

#define UNIX_SOCK_NAME "/tmp/unix_socket"
#define BUF_SZ 128
#define QUE_SZ 20
#define MAX_CLI_SZ 32

int sock_fds[MAX_CLI_SZ];
int outs[MAX_CLI_SZ] = {0};

static void
init_sock_fds()
{
  int i;

  for (i = 0; i < MAX_CLI_SZ; i++)
    sock_fds[i] = -1;
}

static void
add_sock_fds(int sock_fd)
{
  int i;

  for (i = 0; i < MAX_CLI_SZ; i++) {
    if (sock_fds[i] != -1)
      continue;

    sock_fds[i] = sock_fd;
    break;
  }
}

static void
remove_sock_fds(int sock_fd)
{
  int i;

  for (i = 0; i < MAX_CLI_SZ; i++) {
    if (sock_fds[i] != sock_fd)
      continue;

    sock_fds[i] = -1;
    break;
  }
}

static void
copy_sock_fds(fd_set *fd_set_ptr)
{
  int i;

  FD_ZERO(fd_set_ptr);
  for (i = 0; i < MAX_CLI_SZ; i++) {
    if (sock_fds[i] != -1)
      FD_SET(sock_fds[i], fd_set_ptr);
  }
}

static int
get_max_sock_fd()
{
  int i, max_sock_fd;

  max_sock_fd = -1;

  for (i = 0; i < MAX_CLI_SZ; i++) {
    if (sock_fds[i] > max_sock_fd)
      max_sock_fd = sock_fds[i];
  }

  return max_sock_fd;
}

int
main(int argc, char *argv[])
{
  struct sockaddr_un un;
  int i, len, n, data, listen_sock_fd, cli_sock_fd;
  fd_set read_fds;
  char buf[BUF_SZ];

  init_sock_fds();

  unlink(UNIX_SOCK_NAME);

  if ((listen_sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  memset(&un, 0, sizeof(un));
  un.sun_family = AF_UNIX;
  strncpy(un.sun_path, UNIX_SOCK_NAME, sizeof(un.sun_path) - 1);
  len = offsetof(struct sockaddr_un, sun_path) + strlen(UNIX_SOCK_NAME);

  if (bind(listen_sock_fd, (struct sockaddr *)&un, len) == -1) {
    perror("bind()");
    exit(EXIT_FAILURE);
  }

  if (listen(listen_sock_fd, QUE_SZ) == -1) {
    perror("listen()");
    exit(EXIT_FAILURE);
  }

  add_sock_fds(listen_sock_fd);

  for (;;) {
    copy_sock_fds(&read_fds);

    printf("클라이언트 연결 대기 중...\n");

    select(get_max_sock_fd() + 1, &read_fds, NULL, NULL, NULL);

    if (FD_ISSET(listen_sock_fd, &read_fds)) {
      if ((cli_sock_fd = accept(listen_sock_fd, NULL, NULL)) == -1) {
        perror("accept()");
        exit(EXIT_FAILURE);
      };

      printf("클라이언트(%d) 연결.\n", cli_sock_fd);

      add_sock_fds(cli_sock_fd);
    } else {
      cli_sock_fd = -1;

      for (i = 0; i < MAX_CLI_SZ; i++) {
        if (FD_ISSET(sock_fds[i], &read_fds)) {
          cli_sock_fd = sock_fds[i];

          memset(buf, 0, BUF_SZ);

          if ((n = read(cli_sock_fd, buf, BUF_SZ)) == -1) {
            perror("read()");
            exit(EXIT_FAILURE);
          };

          memcpy(&data, buf, sizeof(data));
          printf("클라이언트(%d) 데이터: %d\n", cli_sock_fd, data);

          if (data == 0) {
            memset(buf, 0, BUF_SZ);
            sprintf(buf, "반환값: %d", outs[i]);

            if ((n = write(cli_sock_fd, buf, BUF_SZ)) == -1) {
              perror("write");
              exit(EXIT_FAILURE);
            }

            close(cli_sock_fd);
            outs[i] = 0;
            remove_sock_fds(cli_sock_fd);
            continue;
          }

          outs[i] += data;
        }
      }
    }
  }

  close(listen_sock_fd);
  remove_sock_fds(listen_sock_fd);
  printf("서버 종료.\n");

  exit(EXIT_SUCCESS);
}