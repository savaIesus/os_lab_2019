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
        int current_optind = optind ? optind : 1;

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
                    case 0:
                        seed = atoi(optarg);
                        break;
                    case 1:
                        array_size = atoi(optarg);
                        break;
                    case 2:
                        pnum = atoi(optarg);
                        break;
                    case 3:
                        with_files = true;
                        break;
                    default:
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
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
               argv[0]);
        return 1;
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    int active_child_processes = 0;

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    // Объявление массива для pipe
    int pipe_fd[2];
    
    // Создание pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < pnum; i++) {
        pid_t child_pid = fork();
        if (child_pid >= 0) {
            active_child_processes += 1;
            if (child_pid == 0) {
                // Child process
                close(pipe_fd[0]); // Закрываем конец чтения в дочернем процессе

                int start_index = (array_size / pnum) * i;
                int end_index = (i == pnum - 1) ? array_size : start_index + (array_size / pnum);
                
                int local_min = INT_MAX;
                int local_max = INT_MIN;

                for (int j = start_index; j < end_index; j++) {
                    if (array[j] < local_min) local_min = array[j];
                    if (array[j] > local_max) local_max = array[j];
                }

                // Записываем результаты в pipe
                write(pipe_fd[1], &local_min, sizeof(int));
                write(pipe_fd[1], &local_max, sizeof(int));
                
                close(pipe_fd[1]); // Закрываем конец записи после записи
                return 0; // Завершаем дочерний процесс
            }
        } else {
            printf("Fork failed!\n");
            return 1;
        }
    }

    while (active_child_processes > 0) {
        wait(NULL); // Ждем завершения всех дочерних процессов
        active_child_processes -= 1;
    }

    struct MinMax min_max;
    min_max.min = INT_MAX;
    min_max.max = INT_MIN;

    for (int i = 0; i < pnum; i++) {
        int min_from_pipe, max_from_pipe;

        // Читаем результаты из pipe
        read(pipe_fd[0], &min_from_pipe, sizeof(int));
        read(pipe_fd[0], &max_from_pipe, sizeof(int));

        if (min_from_pipe < min_max.min) min_max.min = min_from_pipe;
        if (max_from_pipe > min_max.max) min_max.max = max_from_pipe;
    }

    close(pipe_fd[0]); // Закрываем конец чтения после использования

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);

    printf("Min: %d\n", min_max.min);
    printf("Max: %d\n", min_max.max);
    printf("Elapsed time: %fms\n", elapsed_time);
    
    return 0;
}
