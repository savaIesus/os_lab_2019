#include <stdio.h>
#include <stdlib.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  if (argc != 3) {/*проверка на то, что задано 3 аргумента*/
    printf("Usage: %s seed arraysize\n", argv[0]);
    return 1;
  }

  int seed = atoi(argv[1]);/*превращает первый аргумент в int*/
  if (seed <= 0) {/*проверка на неотрицательность числа*/
    printf("seed is a positive number\n");
    return 1;
  }

  int array_size = atoi(argv[2]);
  if (array_size <= 0) {
    printf("array_size is a positive number\n");
    return 1;
  }

  int *array = malloc(array_size * sizeof(int));/*выделение памяти под массив*/
  GenerateArray(array, array_size, seed);/*генерация случайного массива*/
  struct MinMax min_max = GetMinMax(array, 0, array_size);/*нахождение минимального и максимального элемента массива на заданном промежутке*/
  free(array);/*очистка памяти*/

  printf("min: %d\n", min_max.min);
  printf("max: %d\n", min_max.max);

  return 0;
}
