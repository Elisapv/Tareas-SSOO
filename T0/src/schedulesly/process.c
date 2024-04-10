#include "./process.h"

// FUNCIONES SHEDULER

void scheduler(Process* all_parents) {
  printf("HOLA\n");
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