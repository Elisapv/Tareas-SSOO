#include "./process.h"

// FUNCIONES SHEDULER

void scheduler(Process* all_parents) {
  /* 
    OBJETIVO: 
      1. Iniciar un timer
      2. comparar el timer con TI, recorriendo all_parents hasta que no existan
  */
  Process* queue = malloc(sizeof(Process) * ((N_PROCESSES - 1))); // LIBERAR MEMORIA
  int cont_queue = 0;
  double time = 0;
  bool waitings = true;
  while (waitings) {
    for (int i = 0; i < (N_PROCESSES - 1 ); i++){
      // printf("TI: %f, time: %f\n", (double)all_parents[i].TI, time);
      if ((double)all_parents[i].TI <= time){
        all_parents[i].state = READY;
        queue[cont_queue] = all_parents[i]; // queue debería ser un lista ligada?
        // Acá tenemos que llamar a otra función para que los procesos pasen a RUNNIG
        /* OUTPUT:
          1. IDLE si TI > = 0
        */
        cont_queue++;
      }
      if (cont_queue >= (N_PROCESSES - 1)){
        waitings = false;
      }
    }
    time = (double)clock()/(double)CLOCKS_PER_SEC;
  }
}

void manegeQueue(int index) { // Según yo debería ser una lista ligada
  /* OBJETIVOS:
    1. Meter el siguiente proceso a que corra en la CPU. 
    2. manejar los q's
  */
  /*ESCRIBIR EN OUTPUT
    0. ENTER <PID> <PPID> <GID> TIME (tiempo actual = TI) LINE <NUM_LINE> (posicion + 1 en all_parents) ARG <NUM_ARGS> ()
    Si entre al SO, ahora estoy en la file en estado READY.
    1. RUN <PID> <TIEMPO_TRABAJADO>
    2. Si no alcanzo a terminar vuelve a estado WAITING: WAIT <PID>
    3. Si un proceso vuelve al estado READY: RESUME <PID>
    3. SI TERMINA SU EJECUCIÓN: END <PID> TIME <TIEMPO_ACTUAL_SO>
  */
  double time = 0; // tiempo en la CPU, esperando hijos
  int cont_prog = 0;

  Process* actual;
	Process* previous;
  actual = queue[index];

	Parent* actual_father;
	Parent* previous_father;

  while (cont_prog < queue[index].num_prog_group){
    // significa que no he revisado a todos los programas
    // Esperar el tiempo CI
    if ((double)actual.CI <= time) { // esto no tiene que ser queue[index] pq va a cambiar con el tiempo del loop
      // paso el tiempo, verifico si tengo hijos y vuelvo a hacer el loop
      if (actual.NH == 0) {
        // Termino mi ejecución
        cont_prog++;
      } else {
        int index_children = search_index_children(actual);
        if (index_children == -1) {
          // Yo no tengo más hijos, verficar si mi padre tiene hijos
          if (actual->father != NULL) {
            // Mi padre tiene más hijos que podrían ejecutarse
          } else {
            // SOY EL ORIGINAL, reviso si me quedan hijos
            int children_index = search_index_children(actual->father.children)
            if (children_index == -1){
              printf("CONTADOR_PROG + 1: %d\n", cont_prog++);
              // NO ME QUEDAN HIJOS
              // REPORTE FINAL
            } else {
              // Me quedan hijos
              previous = actual->father;
              actual = previous.children[children_index];
            }
          }
          cont_prog++;
        } else {
          // Me quedan hijos, tiene que partir de nuevo el loop
          previous -> actual;
          actual -> actual->children[index_children];
        }
      }
    }
  }


  

}


// double GetTime() {
//     struct timeval t;
//     int rc = gettimeofday(&t, NULL);
//     assert(rc == 0);
//     return (double)t.tv_sec + (double)t.tv_usec/1e6;
// }


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