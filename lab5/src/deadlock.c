#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;

void* thread1(void* arg){
    pthread_mutex_lock(&mutex1);

    sleep(1);
    printf("Thread2 locked mutex1!\n");

    pthread_mutex_lock(&mutex2);

    printf("Thread1 locked mutex2!\n");

    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
}

void* thread2(void* arg){
    pthread_mutex_lock(&mutex2);
    sleep(1);
    printf("Thread2 locked mutex2!\n");

    pthread_mutex_lock(&mutex3);

    printf("Thread2 locked mutex3!\n");

    pthread_mutex_unlock(&mutex3);
    pthread_mutex_unlock(&mutex2);
}

void* thread3(void* arg){
    pthread_mutex_lock(&mutex3);
    sleep(1);
    printf("Thread3 locked mutex3!\n");

    pthread_mutex_lock(&mutex1);

    printf("Thread1 locked mutex1!\n");

    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex3);
}

int main(){
    pthread_t t1, t2, t3;

    pthread_create(&t1, NULL, thread1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);
    pthread_create(&t3, NULL, thread3, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    printf("Main: Both threads finished\n");

    return 0;
}
