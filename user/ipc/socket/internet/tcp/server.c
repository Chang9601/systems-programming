#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <errno.h>

#define SERV_PORT 3000

void
init_serv()
{
  struct sockaddr_in serv_addr, cli_addr;
  int len, listen_sock_fd, cli_sock_fd;
  fd_set read_fds;

  if ((listen_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = SERV_PORT;
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  len = sizeof(struct sockaddr);

  if (bind(listen_sock_fd, (struct sockaddr *)&serv_addr, len) == -1) {
    perror("bind()");
    exit(EXIT_FAILURE);
  }

  

  exit(EXIT_SUCCESS);
}