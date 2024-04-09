#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <string.h> 
#include "../file_manager/manager.h"

typedef enum { READY, RUNNING, WAITING, FINISHED } State;

typedef struct Process {
    int PID;
    int PPID;
    int GID;
    State state;
    int TI;
    int CI;
    int CF;
    int NH;
    int* CE;
    struct Process* children;       // Arreglo de hijos
    struct Process* father;         // Puntero a mi papá
    // int childrenCount;           // Contador de hijos
} Process;

typedef struct Parent {
    Process* process;
    struct Parent* next;
    struct Parent* previous;
} Parent;

// VARIABLES Y ARREGLOS GLOABLES
int N_PROCESSES;
Process* all_parents;
int q_start;
int q_delta;
int q_min;


//Funciones
int search_index_children(Process* parent);
void print_process(Process* proc);