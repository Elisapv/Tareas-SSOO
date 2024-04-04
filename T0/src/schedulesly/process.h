#include <stdio.h>
#include <stdlib.h>

typedef enum { READY, RUNNING, WAITING, FINISHED } State;

typedef struct ChildProcess {
    int CI;  // Tiempo de CPU antes de posiblemente crear a su hijo
    int NH;  // Número de hijos
    // Incluir más parámetros necesarios para cada hijo
} ChildProcess;

typedef struct Process {
    int PID;        
    int PPID;       
    int GID;        
    State state;    
    int TI;         
    int CI;         
    int NH;         
    ChildProcess* children;  // Arreglo de hijos
    int childrenCount;       // Contador de hijos
} Process;