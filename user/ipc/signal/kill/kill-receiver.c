#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static void
sig_fn(int sig)
{
  printf("받은 신호: %d\n", sig);
} 

int
main(int argc, char *argv[])
{
  signal(SIGUSR1, sig_fn);
  scanf("\n");

  exit(EXIT_SUCCESS);
}