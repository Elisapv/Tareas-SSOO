#include "./process.h"

// FUNCIONES SHEDULER
void scheduler() {
  // Para manejar los TI
  Process* queue = malloc(sizeof(Process) * ((N_PROCESSES - 1))); // LIBERAR MEMORIA
  int cont_queue = 0;
  double time = 0;
  bool waitings = true;
  while (waitings) {
    for (int i = 0; i < (N_PROCESSES - 1); i++){
      printf(" TI: %d vs time: %f\n", all_parents[i].TI, time);
      if ((double)all_parents[i].TI <= time){
        all_parents[i].state = READY;
        queue[cont_queue] = &(all_parents[i]);
        // Acá tenemos que llamar a otra función para que los procesos pasen a RUNNIG
        if (all_parents[i].TI > 0) {
          fprintf(output_file, "IDLE %d\n", all_parents[i].TI);
        }
        printf("TIENE QUE INICIAR LA EJECUACIÓN DEL GRUPO\n");
        manegeQueue(queue, cont_queue);
        cont_queue++;
      }
      if (cont_queue >= (N_PROCESSES - 1)){
        waitings = false;
      }
    }
    time = (double)clock()/(double)CLOCKS_PER_SEC;
  }

  // Reporte final
  fprintf(output_file, "REPORT START\n");
  fprintf(output_file, "TIME %d\n", total_time);
  for (int k = 0; k < (N_PROCESSES -1); k++) {
    // este tiene que cambiar a all_parents....
    fprintf(output_file, "GROUP %d %d\n", all_parents[k].GID, all_parents[k].num_prog_group);
		final_report(&all_parents[k]);
	}
  fprintf(output_file, "REPORT END\n"); 
}

void manegeQueue(Process* queue, int index) {
  // double time = 0; // tiempo en la CPU, esperando hijos
  int cont_prog = 0;
  total_time = queue[index].TI;

  Process* actual;
	// Process* previous;
  actual = &queue[index]; // PROCESO PADRE 

  while (cont_prog < queue[index].num_prog_group){
    // significa que no he revisado a todos los programas
    // if ((double)actual.CI <= time) { // esto no tiene que ser queue[index] pq va a cambiar con el tiempo del loop
    // paso el tiempo, verifico si tengo hijos y vuelvo a hacer el loop
    fprintf(output_file, "ENTER %d %d %d TIME %d LINE %d ARG %d\n", actual->PID, actual->PPID, actual->GID, total_time, (cont_prog+1), actual->pos_CI);
      
    if (actual->NH == 0) {
      // No tengo hijos o no me quedan hijos // Termino mi ejecución
      printf("NO TENGO HIJOS\n");
      actual->state = FINISHED;
      actual->GID = 0;
      total_time += actual->CI; // Siempre sumo mi CI
      actual->CI = -1;          // Ya vi este CI asi que lo hago -1
      // Tengo que ir a verificar mi Padre
      fprintf(output_file, "RUN %d %d\n", actual->PID, total_time); // no siempre depende de q's ESTO NO VA ACÁ.
      // if (actual)
      int index_children = search_children(actual->father); //me devuelve el indice
      // si es que index_children == -1 no le quedan hijos a mi father
      // si es que index_children != -1 le quedan hijos a mi father
      if (index_children != -1){
        // tengo que sumar el CE correspondiente
        total_time+=(actual->CE[index_children-1]);
        actual = &(actual->children[index_children]); // voy al siguiente hijo

      } else {
          if (actual->father == NULL ) {printf("DESPUES DE ESTO NO DEBERÍA ENTRAR DE NUEVO AL WHILE\n");}
          else {
            actual = search_father(actual->father->father); // DEVUELVE EL PRIMER HIJO DISPONIBLE, SE SUPONE.
            if (actual == NULL) {
              printf("DESPUES DE ESTO NO DEBERÍA ENTRAR DE NUEVO AL WHILE\n");
              // break;
            }
          }
      }
      cont_prog++;
      printf("MI CONTADOR %d = %d\n", cont_prog, queue[index].num_prog_group);
    }

    else if (actual->NH > 1) { // tengo más de un hijo -> Aplico lo de los CE
      int index_children = search_children(actual); 
      // index_children == 0: si es mi primer hijo sumo mi CI
      // index_children > 0: si es mi segundo hijo o + sumo CE
      // index_children == -1: no me quedan hijos por ejecutar
      if (index_children == 0) {
        total_time+=actual->CI;
        actual->CI = -1; // lo pongo como menos uno, esto SI O SI ESTÁ MAL
        // previous = actual;
        actual = &(actual->children[index_children]);
        cont_prog++;
      } else if (index_children > 0) { // ESTA MAL
        total_time+=(actual->CE[index_children-1]);
        // actual->CE[index_children-1] = -1; // SI O SI ESTÁ MAL
        // previous = actual;
        actual = &(actual->children[index_children]);
        cont_prog++;
      } else if (index_children == -1) {
        cont_prog++;
        // no me quedan hijos, tengo que ver si a mi papa le quedan hijos
        // No tengo que buscar al papa anterior que le queden hijos??
        int index_children = search_children(actual->father);
        if (index_children == -1) {
          /*mi papá no le quedan hijos*/
          // revisar al papa de mi papá
          if (actual->father->father == NULL) {printf("SOY EL PARENT\n");/*soy el papa original*/}
          else {actual = actual->father;} // tengo que revisar al papa
        } else { 
          // previous = actual->father;
          actual = &(actual->father->children[index_children]);
        }
          // if (actual->father != NULL) {
          //   previous = actual->father;
          //   actual = previous->children[]
          //   // soy papa normal
          // } else {
          //   // soy papa original
          //   if (index_children == -1) {/*no quedan hijos por ejecutar en total, hay que sumar el CF final*/}
          //   else {actual = previous = actual->father; actual->father->children[index_children];}
          // }

      }
    } else {
      total_time += actual->CI;
      actual->CI = -1;
      actual = &(actual->children[0]); // mi único hijo lo ejecuto al tiro
      cont_prog++;
    }
    // Tenemos que terminar los procesos que son padres = CF's
    // REPORTE DEL FINAL DEL PARENT
    fprintf(output_file, "END %d TIME %d\n", queue[index].PID, total_time);
  }
  queue[index].cpu_burst = total_time;
}


int search_children(Process* parent) {
  if (parent == NULL) {
    printf("QUE PASO?\n");
    return -1;
  }
  for (int i = 0; i < parent->NH; i++) {
    if (parent->children[i].CI != -1) {
      return i;
    } 
  }
  return -1;
}

Process* search_father(Process* process) {
  // 1. El Proceso es null el PARENT
  // 2. El proximo papá no es NULL mi papá es cualquiera
  if (process->father == NULL){
    // SOY EL PADRE
    return process->father;
  } else {
    int index_children = search_children(process->father);
    if (index_children == -1) {return search_father(process->father->father);} // no le quedan hijos a el, busco en su papá
    else {return &(process->father->children[index_children]);} // devuelvo al hijo que tiene que seguir
  }
}

void final_report(Process* process){
  if (process == NULL) {
    return;
  }
  char* status = find_state(process->state);
  fprintf(output_file, "PROGRAM %d %d %d %s %d\n", process->PID, process->PPID, process->GID, status, process->cpu_burst);
  for (int i = 0; i < process->NH; i++) {
      final_report(&process->children[i]);
  }
}

char* find_state(int index) {
  switch (index) {
    case 0: return "READY";
    case 1: return "RUNNING";
    case 2: return "WAITING";
    case 3: return "FINISHED";
    default: return "UNKNOWN";
  }
}

// FUNCIONES DE LECTURA DEL INPUT

int search_index_children(Process* parent) {
  printf("SUPUESTO LARGO DE LA LISTA DE HIJOS: %d\n", parent->NH);
  for (int i = 0; i < parent->NH; i++) {
    // printf("POSICIÓN DE BUSCAR A MI HIJOS DISPONIBLES: %d\n", i);
    // printf("MI CI: %d\n", parent->children[i].CI);
    if (parent->children[i].CI == -1) {
      // printf("SOY LA POSICIÓN DISPONIBLE: %d\n", i);
      return i;
    }
  }
  return -1;
}

int search_index_ce(Process* parent){
  // solo tiene CE[] si es que tiene hijos, acá se da el segmentation fauld
  // printf("NH PAPA EN SEARCH: %d\n",parent->NH -1);
  for (int i = 0; i < ((parent->NH) -1); i++){
    // printf("%d\n", parent->CE[i]);
    if (parent->CE[i] == -1){
      return i;
    }
  }
  return -1;
};

void print_process(Process* process) {
    if (process == NULL) {
        return;
    }
    
    // Imprime la información del proceso actual
    printf("Process TI: %d, CI: %d, NH: %d, CF: %d, PPID: %d, PID: %d, GID: %d\n",process->TI, process-> CI, process-> NH, process->CF, process->PPID, process->PID, process->GID);

    if (process->CE != NULL) {
        for (int i = 0; i < (process->NH) -1; i++) {
            printf("CE[%d] = %d\n", i, process->CE[i]);
        }
    }


    // Recorre y imprime todos los hijos del proceso
    for (int i = 0; i < process->NH; i++) {
        print_process(&process->children[i]);
    }
}