#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <pthread.h>
#include "../../lab3/src/utils.h"
#include "threads_sum.h"

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  /*
   *  TODO:
   *  threads_num by command line arguments
   *  array_size by command line arguments
   *	seed by command line arguments
   */

  uint32_t threads_num = 0;
  uint32_t array_size = 0;
  uint32_t seed = 0;
  
  if (argc != 4) {
    printf("Usage: %s seed array_size threads_num\n", argv[0]);
    return 1;
  }

  seed = atoi(argv[1]);
  if (seed <= 0) { 
    printf("seed is a positive number\n");
    return 1;
  }

  array_size = atoi(argv[2]);
  if (array_size <= 0) {
    printf("array_size is a positive number\n");
    return 1;
  }

  threads_num = atoi(argv[3]);
  if (threads_num <= 0) { 
    printf("threads_num is a positive number\n");
    return 1;
  }

  pthread_t threads[threads_num];

  /*
   * TODO:
   * your code here
   * Generate array here
   */

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);

  // for (int i=0; i < array_size; ++i){
  //   printf("%d ", array[i]);
  // }
  // printf("\n");

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  struct SumArgs args[threads_num];

  int chunk_size = array_size / threads_num;
  for (uint32_t i = 0; i < threads_num; i++) {
    args[i].array = array;
    args[i].begin = i * chunk_size;
    args[i].end = (i + 1) * chunk_size;
    if (i == threads_num - 1) {
      args[i].end = array_size; 
    }

    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void *)&sum);
    total_sum += sum;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  printf("Total sum: %d\n", total_sum);
  printf("Elapsed time: %fms\n", elapsed_time);
  return 0;
}
