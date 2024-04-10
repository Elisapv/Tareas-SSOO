#include "./process.h"

int main(int argc, char const *argv[])
{
	/*Lectura del input: ./testsT0/P2/inputs/in01.txt */
	char *file_name = (char *)argv[1];
	InputFile *input_file = read_file(file_name);

	N_PROCESSES = input_file->len;

	// all_parents = malloc(sizeof(Process) * N_PROCESSES);

	printf("K líneas: %d\n", input_file->len);
	printf("qstart: %d - qdelta: %d - qmin: %d\n", atoi(input_file->lines[0][0]), atoi(input_file->lines[0][1]), atoi(input_file->lines[0][2]));
	q_start = atoi(input_file->lines[0][0]);
	q_delta = atoi(input_file->lines[0][1]);
	q_min = atoi(input_file->lines[0][2]);

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
		all_parents[i].q_start = q_start;
		all_parents[i].q_delta = q_delta;
		all_parents[i].q_min = q_min;
	}

	Process* actual;
	Process* previous;

	Parent* actual_father;
	Parent* previous_father;
	int pid = 1; 


	for (int i = 1; i < input_file->len; ++i)
	{
		printf("\n");
		printf("PRICIPIO DEL FOR\n");
		int cont_childs = atoi(input_file->lines[i][2]);
		int index_file = 2;
		int num_prog_group = 1; // asumo que está el padre

		int cont = 0; // ppid, ce, children // pero eso afecta a los ppdis?
		printf("TI: %d - CI: %d - NH: %d\n", atoi(input_file->lines[i][0]), atoi(input_file->lines[i][1]), atoi(input_file->lines[i][2]));
		all_parents[i-1].TI = atoi(input_file->lines[i][0]);
		all_parents[i-1].CI = atoi(input_file->lines[i][1]);
		all_parents[i-1].pos_CI = 1;
		all_parents[i-1].NH = atoi(input_file->lines[i][2]);
		all_parents[i-1].children = malloc(sizeof(Process) * (all_parents[i-1].NH));
		// Agregando los PIDS y GIDS
		all_parents[i-1].PID = pid;
		all_parents[i-1].GID = pid;
		all_parents[i-1].father = NULL; // inncesario...
		pid++;

		if (atoi(input_file->lines[i][2]) > 1) { // tengo CE solo si es que tengo más de un hijo
			all_parents[i-1].CE = malloc(sizeof(int) * ((all_parents[i-1].NH) - 1));
			// RECORRER SU CE Y PORNERLE -1
			for ( int q = 0; q < ((all_parents[i-1].NH) - 1); q++){
				all_parents[i-1].CE[q] = -1;
			}
		}

		// RECORRER SUS HIJOS Y PONERLE -1
		for (int q = 0; q < all_parents[i-1].NH; q++){
			all_parents[i-1].children[q].CI = -1;
		}

		all_parents[i-1].PPID = cont; // preguntar
		// HAY QUE AGREGAR AL PADRE A LA LISTA LIGADA DE PARENTS PARA DESPUES ASIGNARLE SU CF
		Parent *newFather = malloc(sizeof(Parent));
		newFather -> process = &all_parents[i-1];
		newFather -> previous = NULL;
		actual_father = newFather;
		previous_father = NULL;
		
		cont++;
		previous = &all_parents[i-1];

		if (all_parents[i-1].NH != 0) {
			num_prog_group += all_parents[i-1].NH;
			int index_children = search_index_children(previous);
			actual = &(all_parents[i-1].children[index_children]);
			// printf("QUE PROCESO SOY EN EL ARRAY DE HIJOS: %d\n", cont-1);
			actual->father = previous;
			// printf("Cont_child: %d\n", cont_childs);

			while (cont_childs > 0) {
				// printf("index_file: %d\n", index_file);
				// printf("\nENTRE ANALISIS NUEVO PROCESO\n");
				
				actual->PPID = cont;
				actual->PID = pid;
				actual->GID = previous->GID;
				pid++;

				index_file++;
				actual->CI = atoi(input_file->lines[i][index_file]);
				actual->pos_CI = index_file;
				// printf("CI: %d\n", actual->CI);

				index_file++;
				actual->NH = atoi(input_file->lines[i][index_file]);
				// printf("NH: %d\n", actual->NH);
				actual->TI = -1;

				if (actual->NH == 0){ // No tiene hijos
					printf("ACTUAL->NH: %d\n", actual->NH );

					index_file++;
					actual->CF = atoi(input_file->lines[i][index_file]);
	
					if (previous->NH > 1) {
						printf("TI: %d\n", previous->TI);
						int index_CE = search_index_ce(previous);
						if (index_CE != -1) {
							// Significa que hay Ce que rellenas
							index_file++;
							previous->CE[cont-1] = atoi(input_file->lines[i][index_file]);
						}
					}

					// BUSCO SI MI PADRE TIENE MÁS HIJOS
					int children_index = search_index_children(previous);
					if (children_index != -1) {
						// Tengo más hijos que ejecutar por lo que el previous no cambia
						actual = &previous->children[children_index];
					} else {
						// A mi Papá (previous) no le quedan hijos, tengo que ver si mi abuelo tiene más hijos (FATHER DE PREVIOUS)
						// Actual tiene que ser siguiente hijo del father, POR AHORA EL ACTUAL ES SEBA 
						// OPCIÓN MI PAPÁ NO TIENE PAPA
						if (previous->father != NULL) { 
							previous = previous->father;
							int children_index = search_index_children(previous);
							actual = &previous->children[children_index];
						} else {
							// SOY EL FATHER
							// TENGO QUE VER SI EL PROCESO PARENT TIENE MÁS HIJOS
							int children_index = search_index_children(all_parents[i-1].children);
							if (children_index == -1) {
								// SOY EL PADRE Y NO ME QUEDAN HIJOS
								index_file++;
								break;
							} else {
								actual = &all_parents[i-1].children[children_index];
							}
						}

					}
				} else {
					// Acá tengo hijos
					// Lo agregamos a la lista ligada de los padres para despues asignarle su CF
					printf("ENTRE ACÁ\n");

					Parent *newParent = malloc(sizeof(Parent));
					newParent->process = actual;
					newParent->previous = actual_father;
					newParent->next = NULL;

					previous_father = actual_father;
					actual_father = newParent,

					// Tengo hijos
					cont_childs += actual->NH;
					num_prog_group += actual->NH;
					// Guardamos el espacio en la memoria para los hijos del proceso y para los CE
					actual->children = malloc(sizeof(Process) * (actual->NH));
					if (actual->NH > 1) {
						actual->CE = malloc(sizeof(int) * ((actual->NH) - 1)); // es por posiciones
					}
					// Previous soy yo y el siguiente es mi primer hijo
					previous = actual;
					actual = &actual->children[0];
					actual->father = previous;

				}
				cont_childs--; // un hijo acaba de guardarse
				printf("CONT_CHILDS AL FINAL DE UN LOOP: %d\n", cont_childs);
				cont++;
			}

			// CF's // Recorrer la lista mientras el puntero actual no sea nulo
			printf("PARTO RELLENANDO LOS CF DE LOS PADRES\n");
			
			if (actual_father->previous == NULL) {
				printf("ACTUAL FATHER NULL\n");
				actual_father->process->CF =  atoi(input_file->lines[i][index_file]);
			} else {
				while (actual_father->previous != NULL) {
					actual_father->process->CF = atoi(input_file->lines[i][index_file]);
					actual_father = actual_father->previous;
					index_file++;
				}
				all_parents[i-1].CF = atoi(input_file->lines[i][index_file]);
			}
		
		} else {
			all_parents[i-1].CF = atoi(input_file->lines[i][3]);
		}

		all_parents[i-1].num_prog_group = num_prog_group;
		// liberar memoria de previous y de actual
	}

	for (int k = 1; k < N_PROCESSES; k++) {
		printf("\nPARTO MOSTRANDO UN PROCESO\n");
		print_process(&all_parents[k-1]);
	}

	scheduler(all_parents);
	// empezar la tarea!!!!

	input_file_destroy(input_file);
}