#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "../../lab3/src/find_min_max.h"
#include "../../lab3/src/utils.h"

int pnum = -1;
pid_t* pids;
bool is_time_out = false;


void alarm_handler(int signum){
  printf("Таймаут истек!!! вызываем функцию kill для дочерних процессов!\n");
  for (int i = 0; i < pnum; i++){
    kill(pids[i], SIGKILL);
  }
  is_time_out = true;
}

int main(int argc, char **argv) {
  int timeout = -1;
  int seed = -1;
  int array_size = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed < 0){
              printf("seed must be a positive number!");
              return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size < 0){
              printf("array_size must be a positive number!");
              return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if(pnum < 0){
              printf("pnum must be a positive number!");
              return 1;
            }
            break;
          case 3:
            with_files = true;
            break;
          case 4:
            timeout = atoi(optarg);
            if (timeout <= 0) {
              printf("timeout is a positive number\n");
              return 1;
            }
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s required: --seed \"num\" --array_size \"num\" --pnum \"num\" optional: --timeout \"num\" --by_files\n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int pipes[pnum][2];
  int pid_size = array_size / pnum;
  pids = malloc(pnum * sizeof(pid_t));

  if (!with_files){
    for (int i = 0; i < pnum; i++) {
        if (pipe(pipes[i]) == -1) {
            printf("pipe failed, number is %d", i);
            return 1;
        }
    }
  }

  if (timeout > 0) {
    if (signal(SIGALRM, alarm_handler) == SIG_ERR) {
        perror("signal");
        return 1;
    }
    //prinf("%d", timeout);
    alarm(timeout);
  }

  for (int i = 0; i < pnum; i++) {
    pids[i] = fork();
    pid_t child_pid = pids[i];
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process
        int pid_start = pid_size * i;
        int pid_end = (i == pnum - 1) ? array_size : pid_start + pid_size;
        struct MinMax pid_min_max = GetMinMax(array, pid_start, pid_end);

        if (with_files) {
          char filename[256];
          snprintf(filename, sizeof(filename), "%s%d.txt", "result_", i);
          FILE *file = fopen(filename, "w");
          if (file == NULL) {
              printf("fopen failed, number is %d\n", i);
              return 1;
          }
          fprintf(file, "%d %d\n", pid_min_max.min, pid_min_max.max);
          fclose(file);
        } else {
          close(pipes[i][0]); // Закрываем дескриптор для чтения
          write(pipes[i][1], &pid_min_max.min, sizeof(int));
          write(pipes[i][1], &pid_min_max.max, sizeof(int));
          close(pipes[i][1]); // Закрываем дескриптор для записи
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }
  int status;
  int i = 0;
  while (active_child_processes > 0) {
    waitpid(pids[i], &status, WNOHANG);
    if (!WIFEXITED(status)) {
      printf("Child process %d did not exit normally\n", i);
    }
    i++;
    active_child_processes -= 1;
  }
  free(pids);

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      char filename[256];
      snprintf(filename, sizeof(filename), "%s%d.txt", "result_", i);
      FILE *file = fopen(filename, "r");
      if (file == NULL) {
          printf("fopen (r) failed, number is %d\n", i);
          continue;
      }
      fscanf(file, "%d %d", &min, &max);
      fclose(file);
      remove(filename);
    } else {
      close(pipes[i][1]);  // Закрываем дескриптор для записи
      read(pipes[i][0], &min, sizeof(int));
      read(pipes[i][0], &max, sizeof(int));
      close(pipes[i][0]);  // Закрываем дескриптор для чтения
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  if(!is_time_out){
    printf("Min: %d\n", min_max.min);
    printf("Max: %d\n", min_max.max);
    printf("Elapsed time: %fms\n", elapsed_time);
  }
  
  fflush(NULL);
  return 0;
}
