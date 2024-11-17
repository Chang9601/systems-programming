#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ev-loop.h"

struct arg_obj {
  int *nums;
  int n;
};

int calc_sum(int [], int);
int calc_mul(int [], int);
void calc_sum_cb(void *);
void calc_mul_cb(void *);

int
main(int arcv, char *argv[])
{
  struct arg_obj *arg_obj;
  struct ev_loop loop;
  int nums[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  ev_loop_init(&loop);
  ev_loop_run(&loop);
  
  sleep(1);

  arg_obj = calloc(1, sizeof(*arg_obj));
  arg_obj->nums = nums;
  arg_obj->n = sizeof(nums) / sizeof(nums[0]);

  task_create(&loop, calc_sum_cb, (void *)arg_obj);
  task_create(&loop, calc_mul_cb, (void *)arg_obj);

  pthread_exit(NULL);
}


int
calc_sum(int nums[], int n)
{
  int i, sum;

  sum = 0;
  for (i = 0; i < n; i++)
    sum += nums[i];

  return sum;
}

int
calc_mul(int nums[], int n)
{
  int i, mul;

  mul = 1;
  for (i = 0; i < n; i++)
    mul *= nums[i];

  return mul;
}

void
calc_sum_cb(void *arg)
{
 struct arg_obj *arg_obj;

 arg_obj = (struct arg_obj *)arg;
 printf("합 = %d\n", calc_sum(arg_obj->nums, arg_obj->n));
}

void
calc_mul_cb(void *arg)
{
 struct arg_obj *arg_obj;

 arg_obj = (struct arg_obj *)arg;
 printf("곱 = %d\n", calc_mul(arg_obj->nums, arg_obj->n));
}