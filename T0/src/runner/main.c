#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include "../file_manager/manager.h"
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <wait.h>
#include "runner.h"


int main(int argc, char const *argv[])
{
	struct timeval start, end;

    // Obtener el tiempo de inicio
    gettimeofday(&start, NULL);

	signal(SIGTSTP, handler_sigint);

	/*Lectura del input*/
	//argc = número de lineas
	char *file_name = (char *)argv[1];

	InputFile *input_file = read_file(file_name);

	/*Mostramos el archivo de input en consola*/
	// printf("Cantidad de lineas: %d\n", input_file->len);

	for (int i = 0; i < input_file->len; ++i)
	{
		
		int argc = atoi(input_file->lines[i][0]); 	// Número de argumentos
		printf("%d ", atoi(input_file->lines[i][0]));
		printf("%s ", input_file->lines[i][1]);
		for (int j = 2; j < argc + 2; ++j)
		{
			printf("%s ", input_file->lines[i][j]);
		}

		printf("\n");

		if (argc > 0) {  							// Comando normal para ejecutar un proceso
			char **args = input_file->lines[i] + 1; // Por que + 1?
			start_process(file_name, args);
		} else if (argc == -1 && strcmp(input_file->lines[i][1], "wait_all") == 0) {
			continue;
			// int timeout = atoi(input_file->lines[i][2]);
        	// time_t start_time = time(NULL);

        	// while ((time(NULL) - start_time) < timeout) {
            // 	int status;
            // 	pid_t done = wait(&status);
            // 	if (done == -1) {
            //     	if (errno == ECHILD) {  // No más procesos hijos
            //         	break;
            //     	}
            // 	} else {
            //     	// Aquí podrías actualizar la información del proceso que ha terminado, si es necesario
			// 		continue;
            // 	}
        	// }
		}
	}

	// Esperar a que todos los procesos terminen
    int status;
    while (wait(&status) > 0); // Bucle para esperar a todos los procesos hijos

    // Obtener el tiempo de finalización
    gettimeofday(&end, NULL);

    // Calcular la duración total en segundos
    double elapsed = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) / 1000000.0);
    printf("Tiempo total de ejecución: %.2f segundos\n", elapsed);

	// Escribimos archivo csv
	const char *csvFileName = "output.csv";

	// Abre el archivo CSV para escritura
    FILE *csvFile = fopen(csvFileName, "w");
    if (csvFile == NULL) {
        perror("No se pudo abrir el archivo CSV");
        return 1;
    }

    // Escribe los nombres de los procesos en el archivo CSV
    for (int i = 0; i < input_file->len; i++) {
        fprintf(csvFile, "%f", elapsed);
		// fprintf(csvFile, "%d,", ProcessInfo[i]->pid); 
    }
	// Cerrar archivo csv
	fclose(csvFile);

	input_file_destroy(input_file);
}


// for (int i = 0; i < input_file->len; ++i) {
//     int num_args = atoi(input_file->lines[i][0]);  // número de argumentos

//     if (num_args > 0) {  // Comando normal para ejecutar un proceso
//         char **args = input_file->lines[i] + 1;
//         start_process(args[0], args);
//     } else if (num_args == -1 && strcmp(input_file->lines[i][1], "wait_all") == 0) {
//         int timeout = atoi(input_file->lines[i][2]);
//         time_t start_time = time(NULL);

//         while ((time(NULL) - start_time) < timeout) {
//             int status;
//             pid_t done = wait(&status);
//             if (done == -1) {
//                 if (errno == ECHILD) {  // No más procesos hijos
//                     break;
//                 }
//             } else {
//                 // Aquí podrías actualizar la información del proceso que ha terminado, si es necesario
//             }
//         }
//     }
// }

