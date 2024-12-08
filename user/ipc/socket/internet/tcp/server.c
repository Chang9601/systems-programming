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

#define PORT   3000
#define BUF_SZ 1024
#define QUE_SZ 20

struct cdata {
  unsigned int n1;
  unsigned int n2;
};

struct sdata {
  unsigned int n;
};

void
serv_init()
{
  /* 서버와 클라이언트 정보를 저장하는 구조체 */
  struct sockaddr_in saddr, caddr;
  /* 
   * lfd: 새로운 클라이언트 연결을 수락하는 데만 사용되며 데이터 교환은 없다. 
   * cfd: 클라이언트와 서버 간의 데이터 교환에만 사용된다.
   */
  int lfd, cfd, bytes_sent, bytes_recv;
  socklen_t addrlen;
  /*
   * select()가 폴링하는 파일 디스크립터 집합
   * 집합에 있는 파일 디스크립터 중 어느 하나에 데이터가 도착하면 select()가 해제된다.
   */
  fd_set readfds;
  char buf[BUF_SZ];
  struct cdata *cdata;
  struct sdata sdata;

  if ((lfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  /* IPv4 네트워크 패킷만 처리한다. */
  saddr.sin_family = AF_INET;
  /* 포트 번호 3000으로 도착하는 데이터를 처리한다. */
  saddr.sin_port = PORT;
  /* 서버의 어느 인터페이스의 IP와 일치하는 데이터를 처리한다. */
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

  for (;;) {
    FD_ZERO(&readfds);
    /* 서버가 실행 중인 소켓을 집합에 추가한다. */
    FD_SET(lfd, &readfds);

    /* 서버 프로세스는 readfds 집합에 있는 파일 기술자 중 하나에 데이터가 도착할 때까지 차단된다. */
    printf("select() 시스템 호출에 블록...\n");
    select(lfd + 1, &readfds, NULL, NULL, NULL);

    /* 새로운 클라이언트가 서버에 연결할 경우. */
    if (FD_ISSET(lfd, &readfds)) {
      /* 
       * 데이터는 새로운 클라이언트가 서버에 연결할 때만 리스너 소켓에 도달한다(즉, 클라이언트에서 connect()를 호출한다.).
       * accept() 함수는 새로운 임시 파일 기술자를 반환하며 서버는 이 cfd를 클라이언트와 연결이 유지되는 동안 메시지를 교환하는데 사용한다. 
       * 리스너 소켓은 새로운 클라이언트의 연결을 수락하는 데만 사용되며 클라이언트와의 데이터 교환에는 사용되지 않는다.
       */
      if ((cfd = accept(lfd, (struct sockaddr *)&caddr, &addrlen)) == -1) {
        perror("accept()");
        exit(EXIT_FAILURE);
      };

      printf("클라이언트 연결 수락: %s:%u\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
      
      for (;;) {
        memset(buf, 0, sizeof(buf));

        /*
         * 서버는 클라이언트로부터 데이터를 수신한다. 클라이언트의 IP와 포트는 caddr에 저장되며 서버는 caddr을 사용하여 클라이언트에게 응답을 전송한다.  
         * 서버 프로세스는 accept() 함수를 통해 연결 요청이 수락된 클라이언트로부터 cfd로 데이터가 도착할 때까지 차단된다.
         */
        if((bytes_recv = recvfrom(cfd, buf, sizeof(buf), 0, (struct sockaddr *)&caddr, &addrlen)) == -1) {
          perror("recvfrom()");
          exit(EXIT_FAILURE); 
        }

        if (bytes_recv == 0) {
          close(cfd);
          break;
        }

        printf("서버가 클라이언트 %s:%u로부터 %d바이트를 수신\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port), bytes_recv);

        cdata = (struct cdata *)buf;

        if (cdata->n1 == 0 && cdata->n2 == 0) {
          close(cfd);
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

int
main(int argc, char *argv[])
{
  serv_init();
  exit(EXIT_SUCCESS);
}