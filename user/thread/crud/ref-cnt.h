#ifndef _REF_CNT_H
#define _REF_CNT_H

#include <pthread.h>
#include <stdint.h>

struct ref_cnt {
  uint32_t cnt;
  pthread_spinlock_t spinlock;
};

void
ref_cnt_init(struct ref_cnt *ref_cnt);

void
ref_cnt_inc(struct ref_cnt *ref_cnt);

bool
ref_cnt_dec(struct ref_cnt *ref_cnt);

void
ref_cnt_destroy(struct ref_cnt *ref_cnt);

void
thr_running(struct ref_cnt *ref_cnt);

bool
thr_done(struct ref_cnt *ref_cnt);

#endif