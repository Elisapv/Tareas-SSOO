#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <stdbool.h>
#include <string.h> 
#include <time.h>
#include <assert.h>
#include "../file_manager/manager.h"

typedef enum { 
    READY, 
    RUNNING, 
    WAITING, 
    FINISHED 
} State;

typedef struct Process {
    int PID;
    int PPID;
    int GID;
    State state;
    int CI;
    int CF;
    int NH;
    int* CE;
    struct Process* children;       // Arreglo de hijos
    struct Process* father;         // Puntero a mi papá
    // Estas solo las tiene el padre
    int pos_CI;
    int num_prog_group;
    int TI;
    int cpu_burst;
    int q_start;
    int q_delta;
    int q_min;

} Process;

typedef struct Parent {
    Process* process;
    struct Parent* next;
    struct Parent* previous;
} Parent;

// VARIABLES Y ARREGLOS GLOABLES
int N_PROCESSES;
struct Process* all_parents;
struct Process* queue;
int q_start;
int q_delta;
int q_min;
int total_time;

FILE* output_file;


//Funciones Lectura Input
int search_index_children(Process* parent);
int search_index_ce(Process* parent);
void print_process(Process* proc);

//Funciones Scheduler
void scheduler();
// void manegeQueue(Process** queque, int index);
void manegeQueue(int index);
Process* search_father(Process* process);
int search_children(Process* parent);
void final_report(Process* process);
char* find_state(int index);
// double GetTime();

// Liberar Memoria
void free_memory();