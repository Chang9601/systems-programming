#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT    3000
#define BUF_SZ  1024
#define QUE_SZ  20
#define MAX_CLI 32

struct cdata {
  unsigned int n1;
  unsigned int n2;
};

struct sdata {
  unsigned int n;
};

int monitored_fds[32];

static void init_monitored_fds();
static void add_monitored_fds(int);
static void remove_monitored_fds(int);
static void reinit_readfds(fd_set *);
static int get_max_fd();

void
serv_init()
{
  struct sockaddr_in saddr, caddr;
  int i, lfd, cfd, bytes_sent, bytes_recv;
  socklen_t addrlen;
  fd_set readfds;
  char buf[BUF_SZ];
  struct cdata *cdata;
  struct sdata sdata;

  init_monitored_fds();

  if ((lfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  saddr.sin_family = AF_INET;
  saddr.sin_port = PORT;
  saddr.sin_addr.s_addr = INADDR_ANY;
  addrlen = sizeof(struct sockaddr);

  if (bind(lfd, (struct sockaddr *)&saddr, addrlen) == -1) {
    perror("bind()");
    exit(EXIT_FAILURE);
  }

  if (listen(lfd, QUE_SZ) < 0) {
    perror("listen()");
    exit(EXIT_FAILURE);
  }

  add_monitored_fds(lfd);

  for (;;) {
    reinit_readfds(&readfds);

    printf("select() 시스템 호출에 블록...\n");
    select(get_max_fd() + 1, &readfds, NULL, NULL, NULL);

    if (FD_ISSET(lfd, &readfds)) {
      if ((cfd = accept(lfd, (struct sockaddr *)&caddr, &addrlen)) == -1) {
        perror("accept()");
        exit(EXIT_FAILURE);
      };

      add_monitored_fds(cfd);
      printf("클라이언트 연결 수락: %s:%u\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));


    } else {
      cfd = -1;

      for (i = 0; i < MAX_CLI; i++) {
        if (FD_ISSET(monitored_fds[i], &readfds)) {
          cfd = monitored_fds[i];

          memset(buf, 0, sizeof(buf));
          if((bytes_recv = recvfrom(cfd, buf, sizeof(buf), 0, (struct sockaddr *)&caddr, &addrlen)) == -1) {
            perror("recvfrom()");
            exit(EXIT_FAILURE); 
          }

          if (bytes_recv == 0) {
            close(cfd);
            remove_monitored_fds(cfd);
            break;
          }

          printf("서버가 클라이언트 %s:%u로부터 %d바이트를 수신\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port), bytes_recv);

          cdata = (struct cdata *)buf;

          if (cdata->n1 == 0 && cdata->n2 == 0) {
            close(cfd);
            remove_monitored_fds(cfd);
            printf("서버가 클라이언트와 연결 종료: %s:%u\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
            break;
          }

          sdata.n = cdata->n1 + cdata->n2;

          if((bytes_sent = sendto(cfd, &sdata, sizeof(sdata), 0, (struct sockaddr *)&caddr, addrlen)) == -1) {
            perror("sendto()");
            exit(EXIT_FAILURE);
          }

          printf("서버가 클라이언트에 응답으로 %d바이트 전송\n", bytes_sent);
        }
      }
    }
  }
}

int
main(int argc, char *argv[])
{
  serv_init();
  exit(EXIT_SUCCESS);
}


static void
init_monitored_fds()
{
  int i;

  for (i = 0; i < MAX_CLI; i++)
    monitored_fds[i] = -1;
}

static void 
add_monitored_fds(int fd)
{
  int i;

  for (i = 0; i < MAX_CLI; i++) {
    if (monitored_fds[i] != -1)
      continue;

    monitored_fds[i] = fd;
    break;
  }
}

static void 
remove_monitored_fds(int fd)
{
  int i;

  for (i = 0; i < MAX_CLI; i++) {
    if (monitored_fds[i] != fd)
      continue;

    monitored_fds[i] = -1;
    break;
  }
}

static void 
reinit_readfds(fd_set *ptr)
{
  int i;

  FD_ZERO(ptr);
  for (i = 0; i < MAX_CLI; i++)
    if(monitored_fds[i] != -1)
      FD_SET(monitored_fds[i], ptr);
}

static int
get_max_fd()
{
  int i, max;

  max = -1;
  for (i = 0; i < MAX_CLI; i++)
    if (monitored_fds[i] > max)
      max = monitored_fds[i];

  return max;
}
