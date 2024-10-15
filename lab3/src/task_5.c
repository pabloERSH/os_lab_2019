#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    printf("запуск в отдельном процессе sequential_min_max\n");
    execv("./sequential_min_max", argv);

    // Если execv завершается успешно, этот код не будет выполнен
    perror("execv");
    return 1;
}