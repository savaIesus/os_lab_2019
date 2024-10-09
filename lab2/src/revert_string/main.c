#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "revert_string.h"

int main(int argc, char *argv[])
{
	if (argc != 2)/* проверка на то что введено 2 аргумента */
	{
		printf("Usage: %s string_to_revert\n", argv[0]);
		return -1;
	}

	char *reverted_str = malloc(sizeof(char) * (strlen(argv[1]) + 1)); /*выделение памяти для записи строки в динамический массив*/
	strcpy(reverted_str, argv[1]);/*копирование строки argv[1] в reversed_str */

	RevertString(reverted_str);/* вызов функции*/

	printf("Reverted: %s\n", reverted_str);
	free(reverted_str);/*освобождение памяти*/
	return 0;
}

