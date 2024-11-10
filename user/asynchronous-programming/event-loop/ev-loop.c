#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include "ev-loop.h"

static struct task *
get_nxt_task(struct ev_loop *loop)
{
  struct task *task;

  if (!loop->task_array_head)
    return NULL;

  task = loop->task_array_head;
  loop->task_array_head = task->right;

  if (loop->task_array_head)
    loop->task_array_head->left = NULL;

  task->left = task->right = NULL;

  return task;
}

static void
add_task(struct ev_loop *loop, struct task *task)
{
  struct task *cur_task, *pre_task;

  pre_task = NULL;

  cur_task = loop->task_array_head;

  while (cur_task) {
    pre_task = cur_task;
    cur_task = cur_task->right;
  }

  if (pre_task) {
    pre_task->right = task;
    task->left = pre_task;
  } else {
    loop->task_array_head = task;
  }
}

static bool
is_task_present(struct task *task)
{
  return !(!task->left && !task->right);
}

void
init_ev_loop(struct ev_loop *loop) 
{
  loop->task_array_head = NULL;
  pthread_mutex_init(&loop->mutex, NULL);
  pthread_cond_init(&loop->cv, NULL);
  loop->state = EV_LOOP_IDLE;
  loop->cur_task = NULL;
}

void *
ev_loop_fn(void *arg)
{
  while (true) {
    sleep(2);
    printf("%s() called\n", __func__);
  }
}

void
run_ev_loop(struct ev_loop *loop) 
{
  pthread_attr_t attr;

  assert(loop->tid == NULL);

  loop->tid = calloc(1, sizeof(pthread_t));

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  pthread_create(loop->tid, &attr, ev_loop_fn, (void *)loop);
}