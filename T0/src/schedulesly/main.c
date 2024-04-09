#include "./process.h"

int main(int argc, char const *argv[])
{
	/*Lectura del input: ./testsT0/P2/inputs/in01.txt */
	char *file_name = (char *)argv[1];
	InputFile *input_file = read_file(file_name);

	N_PROCESSES = input_file->len;

	all_parents = malloc(sizeof(Process) * N_PROCESSES);
	for (int i = 0; i < N_PROCESSES; i++) {
		all_parents[i].PID = -1;
		all_parents[i].PPID = -1;
		all_parents[i].GID = -1;
		all_parents[i].state = WAITING;
		all_parents[i].TI = -1;
		all_parents[i].CI = -1;
		all_parents[i].NH = -1;
		all_parents[i].CF = -1;
		all_parents[i].children = NULL;
		all_parents[i].CE = NULL;
		all_parents[i].father = NULL; // es que no tiene papá
	}

	printf("K líneas: %d\n", input_file->len);
	printf("qstart: %d - qdelta: %d - qmin: %d\n", atoi(input_file->lines[0][0]), atoi(input_file->lines[0][1]), atoi(input_file->lines[0][2]));
	q_start = atoi(input_file->lines[0][0]);
	q_delta = atoi(input_file->lines[0][1]);
	q_min = atoi(input_file->lines[0][2]);

	Process* actual;
	Process* previous;

	Parent* actual_father;
	Parent* previous_father;


	for (int i = 1; i < input_file->len; ++i)
	{
		int cont_childs = atoi(input_file->lines[i][2]);
		int index_file = 2;

		int cont = 0; // ppid, ce, children // pero eso afecta a los ppdis?
		printf("TI: %d - CI: %d - NH: %d\n", atoi(input_file->lines[i][0]), atoi(input_file->lines[i][1]), atoi(input_file->lines[i][2]));
		all_parents[i-1].TI = atoi(input_file->lines[i][0]);
		all_parents[i-1].CI = atoi(input_file->lines[i][1]);
		all_parents[i-1].NH = atoi(input_file->lines[i][2]);
		all_parents[i-1].children = malloc(sizeof(Process) * (all_parents[i-1].NH));
		if (all_parents->NH > 1) { // tengo CE solo si es que tengo más de un hijo
			all_parents[i-1].CE = malloc(sizeof(int) * ((all_parents[i-1].NH) - 1));
		}
		// RECORRER SUS HIJOS Y PONERLE -1
		for (int q = 0; q < all_parents[i-1].NH; q++){
			all_parents[i-1].children[q].CI = -1;
		}
		all_parents[i-1].PPID = cont; // preguntar
		// HAY QUE AGREGAR AL PADRE A LA LISTA LIGADA DE PARENTS PARA DESPUES ASIGNARLE SU CF
		actual_father->process = &all_parents[i-1];
		actual_father->previous = NULL;
		///
		cont++;

		previous = &all_parents[i-1];
		if (all_parents[i-1].NH != 0) {
			actual = &(all_parents[i-1].children[cont-1]);
			printf("QUE PROCESO SOY EN EL ARRAY DE HIJOS: %d\n", cont-1);
			actual->father = previous;
			while (cont_childs > 0) {
				printf("index_file: %d\n", index_file);
				printf("ENTRE AL CONTADOR\n");
				
				actual->PPID = cont;

				index_file++;
				actual->CI = atoi(input_file->lines[i][index_file]);
				printf("CI: %d\n", actual->CI);

				index_file++;
				actual->NH = atoi(input_file->lines[i][index_file]);
				printf("NH: %d\n", actual->NH);
				actual->TI = -1;

				if (actual->NH == 0){ // No tiene hijos
					index_file++;
					actual->CF = atoi(input_file->lines[i][index_file]);
	
					// Qué hice acá? Se que es para los CE
					if (previous->NH > 1) {
						printf("entre aqui nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn\n");
						index_file++;
						printf("index_file: %d\n", index_file);
						printf("cont_ %d\n", cont-1);
						previous->CE[cont-1] = atoi(input_file->lines[i][index_file]);
					}

					// BUSCO SI MI PADRE TIENE MÁS HIJOS
					int children_index = search_index_children(previous);
					if (children_index != -1) {
						// tengo más hijos que ejecutar por lo que el previous no cambia
						// previous = actual; 
						actual = &previous->children[children_index];
					} else {
						// A mi Papá (previous) no le quedan hijos, tengo que ver si mi abuelo tiene más hijos (FATHER DE PREVIOUS)
						// actual tiene que ser siguiente hijo del father, POR AHORA EL ACTUAL ES SEBA 
						// OOPCIÓN MI PAPÁ NO TIENE PAPA
						if (previous->father != NULL) { 
							// REVISAR ESTA LÓGICA
							previous = previous->father;
							int children_index = search_index_children(previous);
							printf("1. CHILDREN INDEX (POSICIÓN DISPONIBLE EN EL ARRAY DE CHILDRENS): %d\n", children_index);
							actual = &previous->children[children_index];
						} else {
							// SOY EL FATHER
							// TENGO QUE VER SI EL PROCESO PARENT TIENE MÁS HIJOS
							int children_index = search_index_children(all_parents[i-1].children);
							if (children_index == -1) {
								// SOY EL PADRE Y NO ME QUEDAN HIJOS
								printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
								index_file++;
								printf("ELLLLL INDEXX FILE: %d\n", index_file);
								break;
							} else {
								printf("2. CHILDREN INDEX (POSICIÓN DISPONIBLE EN EL ARRAY DE CHILDRENS): %d\n", children_index);
								actual = &all_parents[i-1].children[children_index];
							}
						}

					}
				} else {
					// lo agregamos a la lista ligada de los padres para despues asignarle su CF
					previous_father = actual_father;
					actual_father->process = actual;
					actual_father->previous = previous_father;
					actual_father->next = NULL;
					previous_father->next = actual_father;

					// Tengo hijos
					cont_childs += actual->NH;
					// guardamos el espacio en la memoria para los hijos del proceso y para los CE
					actual->children = malloc(sizeof(Process) * (actual->NH));
					if (actual->NH > 1) {
						actual->CE = malloc(sizeof(int) * ((actual->NH) - 1)); // es por posiciones
					}
					// previous soy yo y el siguiente es mi primer hijo
					previous = actual;
					actual = &actual->children[0];

				}
				cont_childs--; // un hijo acaba de guardarse
				printf("CONT_CHILDS AL FINAL DE UN LOOP: %d\n", cont_childs);
				cont++;
			}

			// Recorrer la lista mientras el puntero actual no sea nulo
			// CF's
			// index_file--;
			printf("PARTO RELLENANDO LOS CF DE LOS PADRES\n");
			
			if (actual_father->previous == NULL) {
				printf("SOLO YO TENGO HIJOS\n");
				printf("INDEX: %d\n", index_file);
				actual_father->process->CF =  atoi(input_file->lines[i][index_file]);
			} else {
				while (actual_father->previous != NULL) {
					actual_father->process->CF = atoi(input_file->lines[i][index_file]);
					index_file++;
					actual_father = actual_father->previous;
				}
			}
		
		} else {
			all_parents[i-1].CF = atoi(input_file->lines[i][3]);
		}
		// liberar memoria de previous y de actual
	}

	printf("N_PROCESS: %d\n", N_PROCESSES);
	for (int k = 1; k < N_PROCESSES; k++) {
		print_process(&all_parents[k-1]);
	}
	// empezar la tarea!!!!

	input_file_destroy(input_file);
}