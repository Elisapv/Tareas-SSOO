#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include "../file_manager/manager.h"
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <wait.h>


int main(int argc, char const *argv[])
{
	/*Lectura del input*/
	char *file_name = (char *)argv[1];
	InputFile *input_file = read_file(file_name);

	/*Mostramos el archivo de input en consola*/
	printf("Cantidad de lineas: %d\n", input_file->len);
	for (int i = 0; i < input_file->len; ++i)
	{
		
		int argc = atoi(input_file->lines[i][0]);
		printf("%d ", atoi(input_file->lines[i][0]));
		printf("%s ", input_file->lines[i][1]);
		for (int j = 2; j < argc + 2; ++j)
		{
			printf("%s ", input_file->lines[i][j]);
		}
		
		printf("\n");
	}

	input_file_destroy(input_file);
}