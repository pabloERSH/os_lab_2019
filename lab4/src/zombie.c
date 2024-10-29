#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char const *argv[]){
    pid_t pid;
    pid = fork();
    if(pid == -1){
        perror("fork failed");
        exit(-1);
    }
    if(pid == 0){
        printf("This is child process\n");
        printf("My pid: %d\n", getpid());
        printf("Parent pid: %d\n", getppid());
        exit(0);
    }
    else{
        printf("This is parent process\n");
        printf("My pid: %d\n", getpid());
        printf("Parent pid: %d\n", getppid());
        sleep(60);
        exit(0);
    }
    return 0;
}