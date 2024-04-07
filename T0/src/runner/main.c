#include "../file_manager/manager.h"
#include "./runner.h"


int main(int argc, char const *argv[])
{
	signal(SIGINT, handle_sigint);
	signal(SIGTSTP, handle_sigtstp);

	time_reached = false; // revisar

	/*Lectura del input*/

	//argc corresponde número de cosas en el comando que corremos
	printf("argc: %d\n", argc);
	char *file_name = (char *)argv[1];
	char *file_output = (char *)argv[2];

	InputFile *input_file = read_file(file_name);

	printf("%s \n", file_output);
	output_file = fopen(file_output, "w");
	
	// Amount y Max
	int amount = atoi(argv[3]); // Cantidad maxima de procesos externos que pueden correr al mismo tiempo
	int max = -1;
	if (argc == 5) {
		max = atoi(argv[4]);
	}

	printf("amount: %d, max: %d\n", amount, max);

	// Alarma en el tiempo máximo de ejecución // funcionaría con el SIGINT
	if (max > 0) {
		alarm(max);
	}

	/*Mostramos el archivo de input en consola*/
	N_process = input_file->len; // se guarda globalmente el numero de programadas
	printf("Cantidad de lineas: %d\n", input_file->len);

	// REPORTE (ARREGLOS PARA CADA COSA QUE NECESITAMOS DEL REPORTE DE LEN MAX_PROCESS o N° PROCESS)
	childs = malloc(sizeof(int) * amount);
	for (int i = 0; i < amount; i++) {
		childs[i] = -1; // Inicializar el arreglo
	}
	cont_childs = 0; // partimos del proceso 0
	max_childs = amount;

	// Información para el output inicializada
	p_path = malloc(sizeof(char*) * N_process); //path
	for (int i = 0; i < N_process; i++) {p_path[i] = NULL;}
	p_pids = malloc(sizeof(int) * N_process);
	p_status = malloc(sizeof(int) * N_process);
	actual_status = malloc(sizeof(char*) * N_process);
	for (int i = 0; i < N_process; i++) { p_pids[i] = -1; p_status[i] = -1; actual_status[i] = "WAITING";}
	start_time = malloc(sizeof(double) * N_process);
	end_time = malloc(sizeof(double) * N_process);
	for (int i = 0; i < N_process; i++) { p_pids[i] = 0; end_time[i] = 0;}

	for (int i = 0; i < input_file->len; ++i)
	{
		printf("holi entre al for del main\n##########################\n");
		int argc = atoi(input_file->lines[i][0]); 	// = n_argument	// Camiamos argc a la cantidad de argc del proceso.
		printf("%d \n", atoi(input_file->lines[i][0])); 	// Número de argumentos
		char instruction[200];
		printf("%s ", input_file->lines[i][1]);			// Ruta Programa 
		sprintf(instruction, "%s ",input_file->lines[i][1]);
		for (int j = 2; j < argc + 2; ++j)
		{
			strcat(instruction, input_file->lines[i][j]);
			strcat(instruction, " ");
			printf("%s \n", input_file->lines[i][j]); 		// Argumentos
		}
		
		printf("\n");


		printf("AAAAAAAAAAAAAAAAAAAAA\n");
		handle_process(argc, instruction, i);
		// número de argumentos de proceso, path+argumentos, indice del proceso.
		printf("childs_count: %d\n", cont_childs);

	}

	// ESCRIBIR EN EL OUTPUT
	generete_output();

	// LIBERAR LA MEMORIA
	free_memory();

	printf("\nThe program has finished\n");

	input_file_destroy(input_file);
}