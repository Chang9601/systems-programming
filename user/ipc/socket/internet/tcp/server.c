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

#define SERV_PORT 3000
#define BUF_SZ    1024
#define QUE_SZ    20

struct cli_data {
  unsigned int n1;
  unsigned int n2;
};

struct return_data {
  unsigned int n;
};

void
init_serv()
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
  fd_set read_fds;
  char buf[BUF_SZ];
  struct cli_data *data;
  struct return_data *result;

  if ((lfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  /* IPv4 네트워크 패킷만 처리한다. */
  saddr.sin_family = AF_INET;
  /* 포트 번호 3000으로 도착하는 데이터를 처리한다. */
  saddr.sin_port = SERV_PORT;
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
    FD_ZERO(&read_fds);
    /* 서버가 실행 중인 소켓을 집합에 추가한다. */
    FD_SET(lfd, &read_fds);

    /* 서버 프로세스는 read_fds 집합에 있는 파일 기술자 중 하나에 데이터가 도착할 때까지 차단된다. */
    select(lfd + 1, &read_fds, NULL, NULL, NULL);

    /* 새로운 클라이언트가 서버에 연결할 경우. */
    if (FD_ISSET(lfd, &read_fds)) {
      /* 
       * 데이터는 새로운 클라이언트가 서버에 연결할 때만 리스너 소켓에 도달한다(즉, 클라이언트에서 connect()를 호출한다.).
       * accept() 함수는 새로운 임시 파일 기술자를 반환하며 서버는 이 cli_socket_fd를 클라이언트와의 연결이 유지되는 동안 메시지를 주고받는 데 사용한다. 
       * 리스너 소켓은 새로운 클라이언트의 연결을 수락하는 데만 사용되며 클라이언트와의 데이터 교환에는 사용되지 않는다.
       */
      if ((cfd = accept(lfd, (struct sockaddr *)&caddr, &addrlen)) == -1) {
        perror("accept()");
        exit(EXIT_FAILURE);
      };

      printf("클라이언트 연결 수락: %s:%u\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
      
      for (;;) {
        memset(buf, 0, sizeof(buf));

        if((bytes_recv = recvfrom(cfd, buf, sizeof(buf), 0, (struct sockaddr *)&caddr, &addrlen)) == -1) {
          perror("recvfrom()");
          exit(EXIT_FAILURE); 
        }

        if (bytes_recv == 0) {
          close(cfd);
          break;
        }

        printf("%d %s:%d\n");

        data = (struct cli_data *)buf;

        if (data->n1 == 0 && data->n2 == 0) {
          close(cfd);
          print("");
          break;
        }

        result->n = data->n1 + data->n2;

        if((bytes_sent = sendto(cfd, (char *)&result, sizeof(*result), 0, (struct sockaddr *)&caddr, addrlen) == -1)) {
          perror("sendto()");
          exit(EXIT_FAILURE);
        }

        print("%d %s:%u\n");
      }
    }
  }

  exit(EXIT_SUCCESS);
}