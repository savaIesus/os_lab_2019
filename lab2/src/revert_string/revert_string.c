#include "revert_string.h"
#include <string.h>

void RevertString(char *str)
{
	// your code here
	if (str == NULL) return; // Проверка на NULL

    int length = strlen(str); // Получаем длину строки
    int start = 0;            // Начальный индекс
    int end = length - 1;    // Конечный индекс

    // Переворачиваем строку
    while (start < end) {
        // Меняем местами символы
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;

        // Сдвигаем указатели
        start++;
        end--;
    }
}

