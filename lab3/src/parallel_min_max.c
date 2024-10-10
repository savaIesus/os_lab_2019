#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
    int seed = -1;
    int array_size = -1;
    int pnum = -1;
    bool with_files = false;

    while (true) {
        static struct option options[] = {
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {"by_files", no_argument, 0, 'f'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0: seed = atoi(optarg); break;
                    case 1: array_size = atoi(optarg); break;
                    case 2: pnum = atoi(optarg); break;
                    case 3: with_files = true; break;
                    default: printf("Индекс %d выходит за пределы опций\n", option_index);
                }
                break;
            case 'f':
                with_files = true; break;
            case '?':
                break;
            default:
                printf("getopt вернул код символа 0%o?\n", c);
        }
    }

    if (optind < argc) {
        printf("Есть хотя бы один аргумент без опции\n");
        return 1;
    }

    if (seed == -1 || array_size == -1 || pnum == -1) {
        printf("Использование: %s --seed \"num\" --array_size \"num\" --pnum \"num\"\n", argv[0]);
        return 1;
    }

    int *array = malloc(sizeof(int) * array_size);
    if (array == NULL) {
        perror("Не удалось выделить память");
        return 1;
    }

    GenerateArray(array, array_size, seed);
    
    int active_child_processes = 0;
    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    for (int i = 0; i < pnum; i++) {
        pid_t child_pid = fork();
        if (child_pid >= 0) {
            active_child_processes += 1;
            if (child_pid == 0) {
                // Логика дочернего процесса здесь
                // Выполнить вычисления и отправить результаты через IPC
                exit(0); // Убедитесь, что дочерний процесс завершает работу корректно
            }
        } else {
            perror("Ошибка при создании процесса!");
            free(array);
            return 1;
        }
    }

    while (active_child_processes > 0) {
        wait(NULL); // Ждать любой дочерний процесс
        active_child_processes -= 1;
    }

    struct MinMax min_max;
    min_max.min = INT_MAX;
    min_max.max = INT_MIN;

    // Чтение результатов из файлов или каналов здесь

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0 +
                          (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);

    printf("Min: %d\n", min_max.min);
    printf("Max: %d\n", min_max.max);
    printf("Время выполнения: %f мс\n", elapsed_time);
    
    return 0;
}