#include "../file_manager/manager.h"
#include "./runner.h"


void handle_process(int n_argument, char* path, int index){
    printf("\nENTRE A HANDLE_PROCESS\n");
    printf("\n");
    // número de argumentos de proceso, path+argumentos, indice del proceso.
    if (n_argument == -1) {
        // WAIT_ALL
        start_time[index] = GetTime();
        printf("START_TIME WAIT: %f del proceso %s\n", start_time[index], path);
        arguments = malloc(sizeof(char*) * 2);
        split_string(path, " ", arguments);

        int timeout = atoi(arguments[2]);
        printf("timeout: %d", timeout);

        wait_all_childs(timeout);
        end_time[index] = GetTime();
        printf("END_TIME WAIT: %f del proceso %s\n", end_time[index], path);
        // free(arguments);

    }

    else if (cont_childs <= max_childs) {
        // Iniciamos el proecos externo
    
        arguments = malloc(sizeof(char*) * (n_argument + 2));
        split_string(path, " ", arguments);
        char** args = malloc(sizeof(char*) * (n_argument + 1));
        arguments[n_argument + 1] = NULL;
        args[n_argument] = NULL;
        printf("PATH: %s\n", arguments[0]);
        for (int i = 1 ; i < n_argument + 1 ; i++) {
            char* arg = arguments[i]; // tengo que liberar esta memoria.
            args[i-1] = arg;
            printf("Argumento %d: %s \n", i, arg);
        }
        printf("\n");

        start_time[index] = GetTime();

        int pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            // Hijo
            printf("\nENTRE A HIJO\n");
            printf("\n");
            // execv(arguments[0], arguments); // 
            execvp(arguments[0], args);
            actual_status[index] = "STARTED";
            printf("Argumeto: %s, Argumnets: %s\n", arguments[0], *args);
            printf("Error ejecutando proceso %s\n", path);
            perror(execvp);

            // free(arguments);
            // free(args);
        } else {
            // Parent
            printf("ENTRE A PARENT\n");
            init_output(index, arguments[0], pid);
            // free(arguments);
            // free(args);

            int indice = available_space();
            if (indice == -1) {
                printf("No hay espacio disponible\n");
            } else {
                printf("ENTRE AL LISTADO DE PROCESOS...\n");
                childs[indice] = pid;
                // printf("childs pid: %d\n", childs[indice]);
                cont_childs++;
            }
            printf("SALI DE PARENT\n");
            printf("\n");

        }

        free(args);
        free(arguments);
    }

    else {
        wait_child();
        return handle_process(n_argument, path, index);
    }
}

// WAIT
void wait_child() {
    // int status = p_status[index];
    int status;
    for (int index = 0; index < N_process; index++) {
        pid_t child_pid = waitpid(p_pids[index], &status, WNOHANG);
        if (strcmp(actual_status[index], "FINISHED") != 0 && child_pid > 0){
            int exit_status = WEXITSTATUS(status);
            p_status[index] = exit_status;
            actual_status[index] = "FINISHED";
            end_time[index] = GetTime();
        }
        
    }
    printf("HOLA EL PROECESO ENTRO A WAIT CHILD\n");
    cont_childs--;
}

void wait_all_childs(int timeout) {
    printf("Waiting for all childs to finish, max time: %d seconds.\n", timeout);
    int status;
    double startTime = GetTime();
    double elapsedTime = 0;

    while (cont_childs > 0 && elapsedTime < timeout) {
        for (int i = 0; i < max_childs; i++) {
            if (childs[i] != -1) {
                pid_t child_pid = waitpid(childs[i], &status, WNOHANG);
                if (child_pid > 0) {
                    // Proceso hijo terminó
                    if (WIFEXITED(status)) {
                        p_status[i] = WEXITSTATUS(status);
                    } else if (WIFSIGNALED(status)) {
                        p_status[i] = WTERMSIG(status);
                    }
                    actual_status[i] = "FINISHED";
                    end_time[i] = GetTime();
                    childs[i] = -1;
                    cont_childs--;
                }
            }
        }

        elapsedTime = GetTime() - startTime;
        sleep(1); // Pausa breve para evitar sobrecargar el CPU ESTO ME SUENA RARO...
    }

    if (elapsedTime >= timeout) {
        printf("Time limit reached, killing remaining childs.\n");
        for (int i = 0; i < max_childs; i++) {
            if (childs[i] != -1) {
                kill(childs[i], SIGKILL);
                waitpid(childs[i], &status, 0); // Asegúrate de esperar al proceso para evitar zombies
                end_output(childs[i], WTERMSIG(status));
                childs[i] = -1;
                cont_childs--;
            }
        }
    }
}

// SEÑALES

void handle_sigalrm(int signal) {
    printf("SIGALRM\n");
    for (int i = 0; i < max_childs; i++) {
        if (childs[i] != -1) {
            kill(childs[i], SIGINT);
        }
    }

    sleep(10);
    for (int i = 0; i < max_childs; i++) {
        if (childs[i] != -1) {
            int status;
            pid_t result = waitpid(childs[i], &status, WNOHANG);
            if ( result == 0 ) {
                kill(childs[i], SIGTERM);
            }
            end_time[i] = GetTime();
            actual_status[i] = "FINISHED";
            p_status[i] = WTERMSIG(status); // SIEMPRE TIENE QUE SER 2, VERIFICAR
            printf("MAX TIMEPO ALCANZADO\nstatus: %d\n, proceso: %s\n", p_status[i], p_path[i]);
        }
    }
}

// void handle_sigint(int signal) {
//     printf("SE TERMINO EL TIEMPO MAX\n");
//     sleep(10);
//     // STATUS = 2
//     for (int i = 0; i < max_childs; i++) {
//       if (childs[i] != -1) {
//         kill(childs[i], SIGTERM);
//         // tengo que ended_output
//         end_output(childs[i], 2);
//         actual_status[i] = "FINISHED";
//         // childs[i] = -1; // esto tampoco debería ser así...
//       }
//   }
// }

void handle_sigtstp(int signal){
    // time_reached = true;
    printf("SE APRETO CTRL + Z\n");
    // Terminamos los procesos
    for (int i = 0; i < max_childs; i++) {
        if (childs[i] != -1) {
            kill(childs[i], SIGTERM);
            childs[i] = -1;
            end_time[i] = GetTime();
            actual_status[i] = "FINISHED";
        }
    }
    // Esperamos 10 segundos
    sleep(10);
    // Si no terminaron, los matamos
    // int status;
    for (int i = 0; i < max_childs; i++) {
        if (childs[i] != -1) {
            // int exit_status = WTERMSIG(status);
            // end_output(childs[i], exit_status); // ESTATUS DE CONTROL Z
            end_output(childs[i], 3); // 
            kill(childs[i], SIGKILL);
            childs[i] = -1;
            actual_status[i] = "FINISHED";
        }
    }
    generete_output();
    free_memory();
    free(arguments);
    exit(0);
}

// GENERAR OUTPUT
void init_output(int index, char* path, int pid) {
    printf("INIT_OUTPUT\n");
    char* program_name = malloc((strlen(path) + 1) * sizeof(char));
    strcpy(program_name, path);
    p_path[index] = program_name;
    p_pids[index] = pid;
}

void end_output(int pid, int status) {
    // time_t time = time()
    int index;
    for (int i = 0; i < N_process; i++) {
        if (p_pids[i] == pid) {
        index = i;
        break;
        }
    }
    end_time[index] = GetTime();
    p_status[index] = status;
    actual_status[index] = "FINISHED";
    printf("TIEMPO FINAL: %f proceos: %s\n", end_time[index], p_path[index]);
}

void generete_output() {
    printf("\nLLEGUE A GENERAR EL OUTPUT\n");
    for (int i = 0; i < N_process; i++) {
        if (p_path[i] !=  NULL) {
            if (strcmp(actual_status[i], "FINISHED") != 0) {
                printf("\nEl procesos termina en generar output\n");
                int status;
                waitpid(childs[i], &status, 0);
                end_time[i] = GetTime();
                p_status[i] = WEXITSTATUS(status);

            }
            printf("proceso %s\nstatus %d\n", p_path[i], p_status[i]);
            int total_time = (int)(end_time[i] - start_time[i]);
            fprintf(output_file, "%s,%d,%d", p_path[i], total_time, p_status[i]); 
        }
    }
    printf("GENERAR OUTPUT FIN\n");
    printf("\n");
}


// ESPACIO DISPONIBLE
int available_space() {
  for (int i = 0; i < max_childs; i++) {
    if (childs[i] == -1) {
      return i;
    }
  }
  return -1;
}


// MANEJAR ARGUMENTOS
void split_string(char* string, char* delimiter, char** result) {
  char* token = strtok(string, delimiter);
  int i = 0;
  while (token != NULL) {
    result[i] = token;
    token = strtok(NULL, delimiter);
    i++;
  }
  char* last = result[i - 1];
  last[strcspn(last, "\n")] = 0;
  result[i-1] = last;
}

// TIEMPOS
double GetTime() {
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    assert(rc == 0);
    return (double)t.tv_sec + (double)t.tv_usec/1e6;
}


// LIBERAR MEMORIA
void free_memory() {
	for (int i = 0; i < N_process; i++) {
		if (p_path[i] != NULL) {free(p_path[i]);}
	}

	free(childs);
	free(p_path);
	free(p_status);
    free(actual_status);
	free(p_pids);
	free(start_time);
	free(end_time);
	fclose(output_file);
}