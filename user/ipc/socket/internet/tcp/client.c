#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h>
#include <memory.h>

#define PORT 3000
#define IP   "127.0.0.1"

struct cdata {
  unsigned int n1;
  unsigned int n2;
};

struct sdata {
  unsigned int n;
};

void
cli_init()
{
  struct sockaddr_in addr;
  struct hostent *h;
  int fd, bytes_sent, bytes_recv;
  socklen_t addrlen;
  struct cdata cdata;
  struct sdata sdata;

  h = (struct hostent *)gethostbyname(IP);
  addr.sin_family = AF_INET;
  addr.sin_port = PORT;
  /* h_addr은 hostent 구조체의 필드로 h_addr_list의 첫 번째 IP 주소를 가진다. */
  addr.sin_addr = *((struct in_addr *)h->h_addr);

  addrlen = sizeof(struct sockaddr);

  if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  if (connect(fd, (struct sockaddr *)&addr, addrlen) == -1) {
    perror("connect()");
    exit(EXIT_FAILURE);
  }

  for (;;) {
    printf("n1: ");
    scanf("%u", &cdata.n1);
    printf("n2: ");
    scanf("%u", &cdata.n2);

    if ((bytes_sent = sendto(fd, &cdata, sizeof(cdata), 0, (struct sockaddr *)&addr, addrlen)) == -1) {
      perror("sendto()");
      exit(EXIT_FAILURE);
    }

    printf("클라이언트가 서버에 %d바이트 전송\n", bytes_sent);

    if ((bytes_recv = recvfrom(fd, &sdata, sizeof(sdata), 0, (struct sockaddr *)&addr, &addrlen)) == -1) {
      perror("recvfrom()");
      exit(EXIT_FAILURE);
    }

    printf("클라이언트가 %d바이트를 수신\n", bytes_recv);

    printf("데이터: %u\n", sdata.n);
  }
}

int
main(int argc, char *argv[])
{
  cli_init();
  exit(EXIT_SUCCESS);
}