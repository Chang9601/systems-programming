#include "udp.h"

struct pkt {
  char addr[16];
  uint32_t port;
  recv_fn fn;
};

void
mem_cleanup(void *arg) 
{
  printf("%s...\n", __func__);

  free(arg);

  return;
}

void
sock_cleanup(void *arg) 
{
  int sock_fd;

  printf("%s...\n", __func__);

  sock_fd = *(int *)arg;

  close(sock_fd);

  return;
}

static void *
_init_serv(void *arg) 
{
  struct pkt *pkt; 
  char addr[16], *buf;
  uint32_t port;
  recv_fn fn;
  int sock_fd, n;
  struct sockaddr_in serv, cli;
  socklen_t addr_len;

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

  pkt = (struct pkt *)arg;
  strncpy(addr, pkt->addr, 16);
  port = pkt->port;
  fn = pkt->fn;

  free(pkt);
  pkt = NULL;

  if ((sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    printf("소켓 생성 실패, errno = %d\n", errno);
    
    pthread_exit((void *)-1);
  }

  pthread_cleanup_push(sock_cleanup, (void *)(intptr_t)sock_fd);

  serv.sin_family = AF_INET;
  serv.sin_port = port;
  serv.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock_fd, (struct sockaddr *)&serv, sizeof(serv)) == -1) {
    printf("소켓 바인딩 실패, errno = %d\n", errno);
    
    pthread_exit((void *)-2);
  }

  buf = calloc(1, BUF_SZ);
  n = 0;
  addr_len = sizeof(cli);

  pthread_cleanup_push(mem_cleanup, (void *)buf);

  for (;;) {
    memset(buf, 0, BUF_SZ);

    n = recvfrom(sock_fd, buf, BUF_SZ, 0, (struct sockaddr *)&cli, &addr_len);

    /* 호스트 바이트 순서로 된 IP 주소를 네트워크 바이트 순서로 변환한 후 문자열 형식으로 변환한다. */
    fn(buf, n, ntop(htonl(cli.sin_addr.s_addr), 0), cli.sin_port);

    /* 대기 전에 취소해야 하는지 확인하기 위해 취소 지점을 삽입한다. */
    pthread_testcancel();
  }

  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);

  pthread_exit(NULL);
}

pthread_t *
init_serv(char *addr, uint32_t port, recv_fn fn) 
{
  pthread_attr_t attr;
  pthread_t *tid;
  struct pkt *pkt;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  pkt = calloc(1, sizeof(*pkt));

  strncpy(pkt->addr, addr, 16);
  pkt->port = port;
  pkt->fn = fn;

  tid = calloc(1, sizeof(*tid));
  pthread_create(tid, &attr, _init_serv, (void *)pkt);

  pthread_attr_destroy(&attr);

  return tid;
}

int
send_msg(char *addr, uint32_t port, char *msg, uint32_t msg_len) 
{
  struct sockaddr_in in;
  struct hostent *host;
  int sock_fd, n;

  in.sin_family = AF_INET;
  in.sin_port = port;

  host = (struct hostent *)gethostbyname(addr);
  /* h_addr은 hostent 구조체의 필드로, h_addr_list의 첫 번째 IP 주소를 가진다. */
  in.sin_addr = *((struct in_addr *)host->h_addr);

  if ((sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    printf("소켓 생성 실패, errno = %d\n", errno);

    return -1;
  }

  n = sendto(sock_fd, msg, msg_len, 0, (struct sockaddr *)&in, sizeof(in));
  close(sock_fd);

  return n;
}

char *
ntop(uint32_t addr, char *buf) 
{
  static char ip[16];
  char *out;

  out = buf ? buf : ip;

  memset(out, 0, 16);
  
  addr = htonl(addr);
  /* p는 표현(presentation), n은 숫자(numeric)를 나타낸다. */
  inet_ntop(AF_INET, &addr, out, 16);
  
  out[15] = '\0';

  return out;
}