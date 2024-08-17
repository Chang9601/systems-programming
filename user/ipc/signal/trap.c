#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static void
sig_int(int sig)
{
  printf("프로세스 정지.\n");

  exit(EXIT_SUCCESS);
}

static void
sig_abrt(int sig)
{
  printf("프로세스 중단.\n");

  exit(EXIT_SUCCESS);
}

int
main(int argc, char *argv[])
{
  char c;

  signal(SIGINT, sig_int);
  signal(SIGABRT, sig_abrt);

  printf("프로그램을 정지하나(y/n)?: ");
  scanf("%c", &c);

  if (c == 'y')
    abort();

  exit(EXIT_SUCCESS);
}