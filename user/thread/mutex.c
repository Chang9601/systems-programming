#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

int arr[] = {17, 42, 55, 63, 88};
pthread_mutex_t mutex;

void *
sum_fn(void *arg)
{
  int i, sum, arr_len;

  arr_len = sizeof(arr) / sizeof(arr[0]);

  while (true) {
    sum = 0;

    pthread_mutex_lock(&mutex);

    for (i = 0; i < arr_len; i++)
      sum += arr[i];
    
    /*
     * 상호 배제가 없을 경우 가능한 합의 값은 3개.
     * 17 + 42 + 55 + 63 + 88 = 265    
     * 17 + 42 + 55 + 63 + 17 = 194
     * 88 + 42 + 55 + 63 + 88 = 336
     * 88 + 42 + 55 + 63 + 17 = 265
     * 
     * 상호 배제가 있을 경우 합은 항상 265.
     */
    printf("합 = %d\n", sum);

    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}

void *
swap_fn(void *arg) 
{
  int tmp, arr_len;
  
  arr_len = sizeof(arr) / sizeof(arr[0]);

  while (true) {
    pthread_mutex_lock(&mutex);

    tmp = arr[0];
    arr[0] = arr[arr_len - 1];
    arr[arr_len - 1] = tmp;

    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}

int
main(int args, char *argv[]) 
{
  pthread_t tid1, tid2;

  pthread_mutex_init(&mutex, NULL);

  pthread_create(&tid1, NULL, sum_fn, NULL);
  pthread_create(&tid2, NULL, swap_fn, NULL);

  pthread_mutex_destroy(&mutex);

  pthread_exit(NULL);

  exit(EXIT_SUCCESS);
}