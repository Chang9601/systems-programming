#include <stdio.h>
#include <stdlib.h>
#include "ev-loop.h"

struct ev_loop ev;

int
main(int arcv, char *argv[])
{

  init_ev_loop(&ev);
  run_ev_loop(&ev);

  pthread_exit(NULL);
}