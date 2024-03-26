#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include "../file_manager/manager.h"

int main(int argc, char const *argv[])
{
	/*Lectura del input: ./testsT0/P2/inputs/in01.txt */
	char *file_name = (char *)argv[1];
	InputFile *input_file = read_file(file_name);

	printf("K líneas: %d\n", input_file->len);
	printf("qstart: %d - qdelta: %d - qmin: %d\n", atoi(input_file->lines[0][0]), atoi(input_file->lines[0][1]), atoi(input_file->lines[0][2]));
	for (int i = 1; i < input_file->len; ++i)
	{
		printf("TI: %d - CI: %d - NH: %d - CF: %d\n", atoi(input_file->lines[i][0]), atoi(input_file->lines[i][1]), atoi(input_file->lines[i][2]), atoi(input_file->lines[i][3]));
	}

	input_file_destroy(input_file);
}