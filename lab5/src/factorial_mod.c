#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <semaphore.h>

sem_t semaphore;
typedef struct {
    int start;
    int end;
    int mod;
    int *result;
    //pthread_mutex_t *mutex;
} thread_data;

void* factorial_mod_part(void *arg) {
    thread_data *data = (thread_data *)arg;
    int start = data->start;
    int end = data->end;
    int mod = data->mod;
    int *result = data->result;
    //pthread_mutex_t *mutex = data->mutex;

    int local_result = 1;
    for (int i = start; i <= end; i++) {
        local_result = (local_result * i) % mod;
    }

    //pthread_mutex_lock(mutex);
    sem_wait(&semaphore);
    *result = (*result * local_result) % mod;
    sem_post(&semaphore);
    //pthread_mutex_unlock(mutex);
}

int main(int argc, char **argv) {
    int k = -1;
    int mod = 0;
    int pnum = 0;

    while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
        case 0:
            switch (option_index) {
            case 0:
                k = atoi(optarg);
                if (k < 0){
                printf("k must be a positive number!");
                return 1;
                }
                break;
            case 1:
                mod = atoi(optarg);
                if (mod <= 0){
                printf("mod must be > 0!");
                return 1;
                }
                break;
            case 2:
                pnum = atoi(optarg);
                if(pnum < 0){
                printf("pnum must be > 0!");
                return 1;
                }
                break;

            defalut:
                printf("Index %d is out of options\n", option_index);
            }
            break;
        case 'f':
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

    if (k == -1 || mod == 0 || pnum == 0) {
        printf("Usage: %s --k \"num\" --mod \"num\" --pnum \"num\" \n",
            argv[0]);
        return 1;
    }

    //pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    sem_init(&semaphore, 0, 1);

    int result = 1;

    pthread_t threads[pnum];
    thread_data tdata[pnum];

    int chunk_size = k / pnum;
    int remainder = k % pnum;

    for (int i = 0; i < pnum; i++) {
        tdata[i].start = i * chunk_size + 1;
        tdata[i].end = (i + 1) * chunk_size;
        if (i == pnum - 1) {
            tdata[i].end += remainder;
        }
        tdata[i].mod = mod;
        tdata[i].result = &result;
        //tdata[i].mutex = &mutex;

        pthread_create(&threads[i], NULL, factorial_mod_part, &tdata[i]);
    }

    for (int i = 0; i < pnum; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("mod(%d!, %d) = %d\n", k, mod, result);

    //pthread_mutex_destroy(&mutex);
    sem_destroy(&semaphore);
}