#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int
main(int argc, char *argv[])
{
  kill(5939, SIGUSR1);
  scanf("\n");

  exit(EXIT_SUCCESS);
}