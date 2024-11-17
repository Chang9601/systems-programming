#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include "ev-loop.h"

static void *_ev_loop_fn(void *);
static struct task *_task_get_nxt(struct ev_loop *);
static void _task_add(struct ev_loop *, struct task *);
static void _task_remove(struct ev_loop *, struct task *);
static bool _task_exist(struct task *);
static void _task_schedule(struct ev_loop *, struct task *);

void
ev_loop_init(struct ev_loop *loop) 
{
  loop->task_array_head = NULL;
  pthread_mutex_init(&loop->mutex, NULL);
  pthread_cond_init(&loop->cv, NULL);
  loop->state = EV_LOOP_IDLE;
  loop->cur_task = NULL;
}

void
ev_loop_run(struct ev_loop *loop) 
{
  pthread_attr_t attr;

  assert(loop->tid == NULL);

  loop->tid = calloc(1, sizeof(pthread_t));

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  pthread_create(loop->tid, &attr, _ev_loop_fn, (void *)loop);
}

struct task *
task_create(struct ev_loop *loop, ev_cb cb, void *arg)
{
  struct task *task;

  task = calloc(1, sizeof(*task));
  task->arg = arg;
  task->cb = cb;
  task->pre = task->nxt = NULL;

  _task_schedule(loop, task);

  return task;
}

void
task_cancel(struct ev_loop *loop, struct task *task)
{
  if (loop->cur_task == task)
    return;

  pthread_mutex_lock(&loop->mutex);
  _task_remove(loop, task);
  pthread_mutex_unlock(&loop->mutex);

  free(task);
}

static void *
_ev_loop_fn(void *arg)
{
  struct task *task;
  struct ev_loop *loop;

  loop = (struct ev_loop *)arg;

  for (;;) {
    pthread_mutex_lock(&loop->mutex);

    while ((task = _task_get_nxt(loop)) == NULL) {
      printf("작업이 없어서 이벤트 루프 스레드가 잠든다.\n");

      loop->state = EV_LOOP_IDLE;
      pthread_cond_wait(&loop->cv, &loop->mutex);
    }
    
    loop->state = EV_LOOP_BUSY;
    /* 
     * 이벤트 루프 스레드가 신호를 받는다. 
     * 신호를 받으면 새로운 작업을 작업 배열에서 꺼내고 다시 대기하는지 아니면 작업을 수행하는지 결정한다.
     */
    pthread_mutex_unlock(&loop->mutex);

    printf("이벤트 루프 스레드가 일어나서 작업을 시작한다.\n");

    loop->cur_task = task;
    task->cb(task->arg);
    loop->cur_task = NULL;
    free(task);
  }

  return NULL;
}

static void 
_task_schedule(struct ev_loop *loop, struct task *task)
{
  pthread_mutex_lock(&loop->mutex);

  _task_add(loop, task);

  if (loop->state == EV_LOOP_BUSY) {
    pthread_mutex_unlock(&loop->mutex);
    return;
  }

  pthread_cond_signal(&loop->cv);
  pthread_mutex_unlock(&loop->mutex);
}

static struct task *
_task_get_nxt(struct ev_loop *loop)
{
  struct task *task;

  if (!loop->task_array_head)
    return NULL;

  task = loop->task_array_head;
  loop->task_array_head = task->nxt;

  if (loop->task_array_head)
    loop->task_array_head->pre = NULL;

  task->pre = task->nxt = NULL;

  return task;
}

static void
_task_add(struct ev_loop *loop, struct task *task)
{
  struct task *cur_task, *pre_task;

  pre_task = NULL;

  cur_task = loop->task_array_head;

  while (cur_task) {
    pre_task = cur_task;
    cur_task = cur_task->nxt;
  }

  if (pre_task) {
    pre_task->nxt = task;
    task->pre = pre_task;
  } else {
    loop->task_array_head = task;
  }
}

static void
_task_remove(struct ev_loop *loop, struct task *task)
{

  if (!task->pre) {
    if (task->nxt) {
      task->nxt = task->nxt->pre = NULL;
    }

    return;
  }

  if (!task->nxt) {
    task->pre = task->pre->nxt = NULL;
    
    return;
  }

  task->pre->nxt = task->nxt;
  task->nxt->pre = task->pre;
  task->pre = task->nxt = NULL;
}

static bool
_task_exist(struct task *task)
{
  return !(!task->pre && !task->nxt);
}