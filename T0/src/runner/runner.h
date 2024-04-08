#pragma once
#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <stdbool.h> // bool, true, false
#include <time.h>
#include <sys/time.h> 
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <wait.h>
#include <assert.h>
#include <sys/types.h>

#define MAX_ARGS 10
#define LINE_BUFFER_SIZE 1024
#define MAX_PROCESSES 100

// Variable Globales
int* childs;
int cont_childs;
int max_childs;
char** arguments;
FILE* output_file;


// ESTRUCTURAS GLOBALES PARA REPORTE
char** p_path; // path, nombre del comando.
int* p_pids;
int* p_status;
char** actual_status;
double* start_time;
double* end_time;
int N_process;



void handle_process(int n_argument, char* path, int index);
void split_string(char* string, char* delimiter, char** result);
// int available_space();

// Wait
void wait_child();
void wait_all_childs(int timeout);

// Señales
// void handle_sigint(int signal);
void handle_sigalrm(int signal);
void handle_sigtstp(int signal);

// Generar output
double GetTime();
void init_output(int index, char* path, int pid);
void end_output(int pid, int status);
void generete_output();

// Liberar memoria
void free_memory();
