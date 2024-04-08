#include "../file_manager/manager.h"
#include "./runner.h"


void handle_process(int n_argument, char* path, int index){
    printf("####################################\n");
    printf("ENTRE A HANDLE_PROCESS\n");
    printf("\n");
    // número de argumentos de proceso, path+argumentos, indice del proceso.
    if (n_argument == -1) {
        // WAIT_ALL
        start_time[index] = (double)clock()/(double)CLOCKS_PER_SEC;
        printf("START_TIME WAIT: %f del proceso %s\n", start_time[index], path);
        arguments = malloc(sizeof(char*) * 2);
        split_string(path, " ", arguments);

        int timeout = atoi(arguments[2]);
        printf("timeout: %d", timeout);

        wait_all_childs(timeout);
        end_time[index] = (double)clock()/(double)CLOCKS_PER_SEC; 
        printf("END_TIME WAIT: %f del proceso %s\n", end_time[index], path);
        // free(arguments);

    }

    // cont_childs es el numero de hijos que estan corriendo
    else if (cont_childs <= max_childs) {
        // Iniciamos el proecos externo
        // char **arguments = split_by_sep(path, " ");
        // arguments[MAX_SPLIT-1] = NULL;
    
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
            printf("Argumeto: %s, Argumnets: %s\n", arguments[0], *args);
            printf("Error ejecutando proceso %s\n", path);
            perror(execvp);

            // free(arguments);
            // free(args);
            exit(1);
        } else {
            // Parent
            printf("ENTRE A PARENT\n");
            init_output(index, arguments[0], pid);
            // agregro pid, nombre del programa y start_time
            // free(arguments);
            // free(args);

            int indice = available_space();
            if (indice == -1) {
                printf("No hay espacio disponible\n");
            } else {
                printf("ENTRE AL LISTADO DE PROCESOS...\n");
                childs[indice] = pid;
                cont_childs++;
            }
            printf("SALI DE PARENT\n");
            printf("\n");

        }
        free(args);
        free(arguments);
        // } else {
        //     if (pid == 0) {
        //         execvp(arguments[0], args);
        //         perror(execvp);
        //         exit(1);
        //     } else {
        //         init_output(index, arguments[0], pid); // agregro pid, nombre del programa y start_time
        //         int indice = available_space();
        //         if (indice == -1) {
        //             printf("No hay espacio disponible\n");
        //         } else {
        //             childs[indice] = pid;
        //             cont_childs++;
        //         }
        //         printf("\n");
        //     }
        // }
    }

    else {
        printf("QUE PASO??\n");
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
            // end_time[index] = time();
            end_time[index]= (double)clock()/(double)CLOCKS_PER_SEC;
        }
        
    }
    printf("HOLA EL PROECESO ENTRO A WAIT CHILD\n");
    cont_childs--;
}

void wait_all_childs(int timeout) {
    printf("Waiting for all childs to finish, max time: %d seconds.\n", timeout);
    int status;
    double startTime = (double)clock() / CLOCKS_PER_SEC;
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
                    end_time[i] = (double)clock() / CLOCKS_PER_SEC;
                    childs[i] = -1;
                    cont_childs--;
                }
            }
        }
        elapsedTime = (double)clock() / CLOCKS_PER_SEC - startTime;
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
void handle_sigint(int signal) {
    printf("SE TERMINO EL TIEMPO MAX\n");
    sleep(10);
    // STATUS = 2
    for (int i = 0; i < max_childs; i++) {
      if (childs[i] != -1) {
        kill(childs[i], SIGTERM);
        // tengo que ended_output
        // end_output(childs[i], 2);
        // childs[i] = -1; // esto tampoco debería ser así...
      }
  }
}

void handle_sigtstp(int signal){
    // time_reached = true;
    printf("SE APRETO CTRL + Z\n");
    // Terminamos los procesos
    for (int i = 0; i < max_childs; i++) {
        if (childs[i] != -1) {
            kill(childs[i], SIGTERM);
            childs[i] = -1;
            // double final_time = GetTime();
            double final_time = (double)clock()/(double)CLOCKS_PER_SEC;
            end_time[i] = final_time;
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
        }
    }
    generete_output();
    free_memory();
    free(arguments);
    exit(0);
}

// GENERAR OUTPUT
void init_output(int index, char* path, int pid) {

  char* program_name = malloc((strlen(path) + 1) * sizeof(char));
  strcpy(program_name, path);
  p_path[index] = program_name;
  p_pids[index] = pid;
  start_time[index] = (double)clock()/(double)CLOCKS_PER_SEC;
  actual_status[index] = "STARTED";
//   double time = GetTime();
//   start_time[index] = time;
  printf("START TIME = %f del proceso %s\n", start_time[index], path);

}

void end_output(int pid, int status) {
    // double time = GetTime();
    // double time = time()
    int index;
    for (int i = 0; i < N_process; i++) {
        if (p_pids[i] == pid) {
        index = i;
        break;
        }
    }
    end_time[index] = (double)clock()/(double)CLOCKS_PER_SEC;
    p_status[index] = status;
    actual_status[index] = "FINISHED";
    printf("TIEMPO FINAL: %f proceos: %s\n", end_time[index], p_path[index]);
}

void generete_output() {
    // creo que puedo estar generando el output dos veces...
    printf("\nLLEGUE A GENERAR EL OUTPUT\n");
    for (int i = 0; i < N_process; i++) {
        if (p_path[i] !=  NULL) {
            if (strcmp(actual_status[i], "FINISHED") != 0) {
                end_time[i] = (double)clock()/(double)CLOCKS_PER_SEC;
                p_status[i] = 100; // hay que ponerle un status.
                actual_status[i] = "FINISHED";
                printf("TERMINE EN GENERETE_OUTPUT con end_time: %f: \n", end_time[i]);
            }
            printf("TIMEPO inicio %f proceso %s\n",start_time[i], p_path[i]);
            double total_time = end_time[i] - start_time[i];
            // int status = p_status[i];
            // char* command = p_path[i];
            fprintf(output_file, "%s,%f,%d\n", p_path[i], total_time, p_status[i]); 
            printf("GENERAR OUTPUT FIN\n########################\n");
            printf("\n");
        }
    }
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
// double GetTime() {
//     struct timeval t;
//     int rc = gettimeofday(&t, NULL);
//     assert(rc == 0);
//     return (double)t.tv_sec + (double)t.tv_usec/1e6;
// }


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