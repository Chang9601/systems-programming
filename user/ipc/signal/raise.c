#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sig_catch_fn(int);

int
main(int argc, char *argv[]) 
{
  if (signal(SIGINT, sig_catch_fn) == SIG_ERR) {
    fputs("signal()\n", stderr);
    exit(EXIT_FAILURE);
  }

  if (raise(SIGINT) == -1) {
    fputs("raise()\n", stderr);
    exit(EXIT_FAILURE);    
  }

  exit(EXIT_SUCCESS);
}

void 
sig_catch_fn(int sig) 
{
  printf("잡힌 신호!\n");
}