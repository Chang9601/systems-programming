#ifndef _EV_LOOP
#define _EV_LOOP

#include <pthread.h>

typedef void (*ev_cb) (void *);

struct task {
  ev_cb cb;
  void *arg;
  struct task *pre, *nxt;
};

enum EV_LOOP_STATE {
  EV_LOOP_IDLE,
  EV_LOOP_BUSY,
};

struct ev_loop {
  /* 작업 배열의 시작 */
  struct task *task_array_head;
  /*
   * 작업 배열에서 상호 배제를 보장하기 위한 뮤텍스. enqueue/deque 작업을 수행한다.
   * 이벤트 루프 속성을 상호 배타적으로 수정하는데 사용된다.
   */
  pthread_mutex_t mutex;
  /* 이벤트 루프 스레드를 일시 중단하기 위한 조건 변수 */
  pthread_cond_t cv;
  /* 이벤트 루프 상태 */
  enum EV_LOOP_STATE state;
  /* 이벤트 루프 스레드 */
  pthread_t *tid;
  /* 
   * 이벤트 루프 스레드가 현재 실행 중인 작업이다.
   * 이벤트 루프가 대기 중일 때는 NULL이다.
   */
  struct task *cur_task;
};

void
ev_loop_init(struct ev_loop *loop);

void
ev_loop_run(struct ev_loop *loop);

struct task *
task_create(struct ev_loop *loop, ev_cb cb, void *arg);

void
task_cancel(struct ev_loop *loop, struct task *task);

#endif