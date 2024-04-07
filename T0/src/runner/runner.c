#include "../file_manager/manager.h"
#include "./runner.h"

void handle_process(int n_argument, char* path, int index){
    printf("####################################\n");
    printf("ENTRE A HANDLE_PROCESS\n");
    printf("\n");
    // número de argumentos de proceso, path+argumentos, indice del proceso.
    if (n_argument == -1) {
        // WAIT_ALL
        // start_time[index] = GetTime();
        start_time[index] = (double)clock()/(double)CLOCKS_PER_SEC;
        printf("START_TIME WAIT: %f del proceso %s\n", start_time[index], path);
        arguments = malloc(sizeof(char*) * 2);
        split_string(path, " ", arguments);

        int timeout = atoi(arguments[2]);
        printf("timeout: %d", timeout);

        wait_all_childs(timeout);
        end_time[index] = (double)clock()/(double)CLOCKS_PER_SEC; 
        printf("END_TIME WAIT: %f del proceso %s\n", end_time[index], path);
        free(arguments);

    }

    // cont_childs es el numero de hijos que estan corriendo
    else if (cont_childs <= max_childs) {
        // Iniciamos el proecos externo
        // char **arguments = split_by_sep(path, " ");
        // arguments[MAX_SPLIT-1] = NULL;
    
        arguments = malloc(sizeof(char*) * (n_argument + 2));
        split_string(path, " ", arguments);
        char** args = malloc(sizeof(char*)*n_argument);
        arguments[n_argument + 1] = NULL;
        printf("PATH: %s\n", arguments[0]);
        for (int i = 1 ; i < n_argument + 1 ; i++) {
            char* arg = arguments[i];
            args[i-1] = arg;
            printf("Argumento %d: %s \n", i, arg);
        }

        int pid = fork();
        if (pid == -1) {
            perror("fork");
            // exit(1); // que hacen los exit
        } else if (pid == 0){
            // Hijo
            printf("\nENTRE A HIJO\n");
            printf("\n");
            execvp(arguments[0], args); // 
            printf("Argumeto: %s, Argumnets: %s\n", arguments[0], *args);
            printf("Error ejecutando proceso %s\n", path);
            perror(execvp);

            free(arguments);
            free(args);
            // exit(1);
        } else {
            // Parent
            printf("ENTRE A PARENT\n");
            init_output(index, arguments[0], pid);
            // agregro pid, nombre del programa y start_time
            free(arguments);
            free(args);

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
    }

    else {
        printf("QUE PASO??\n");
        wait_child();
        return handle_process(n_argument, path, index);
        // Llamo de nuevo a la función con los mismos argumentos 
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
    // ESTA TIENE QUE CAMBIAR
    printf("Waiting for all childs to finish, max time: %d\n", timeout);

    // Bloqueamos al ma por max_time segundos
    // double s_time = GetTime();
    // double s_time = time();
    // while ((time() - s_time) < (double)timeout && !time_reached) {
    //     // No hace nada yupi :)
    // }

    if (time_reached) {
        // Acá no tiene que pasar nada?
        return;
    }
    // Si no terminaron los hijos, los matamos
    printf("Killing childs\n");
    for (int i = 0; i < max_childs; i++) {
        if (childs[i] != -1 && !time_reached) {
            printf("Killing child %d\n", childs[i]);
            int status;
            // pid_t child_pid = waitpid(p_pids[index], &status, WNOHANG);
            end_output(childs[i], WEXITSTATUS(status));
            kill(childs[i], SIGKILL); // VER
            childs[i] = -1; // por que me lo pondría así? Obvio que no...
            printf("ESTOY EN WAIT_ALL\n");
            cont_childs--;
        }
    }
}

// SEÑALES
void handle_sigint(int signal) {
    printf("SE TERMINO EL TIEMPO MAX\n");
    sleep(10);
    // HAY QUE MARCAR LOS ESTATUS, TERMINAR O NO TERMINA??
    // STATUS = 2
    for (int i = 0; i < max_childs; i++) {
      if (childs[i] != -1) {
        kill(childs[i], SIGTERM);
        childs[i] = -1; // esto tampoco debería ser así...
      }
  }
}

void handle_sigtstp(int signal){
    time_reached = true;
    printf("SE APRETO CTRL + Z\n");
    // Terminamos los procesos
    // Acá tengo que ponerle un tiempo final
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
        // pid_t child_pid = waitpid(p_pids[i], &status, WNOHANG);
        if (childs[i] != -1) {
            // int exit_status = WTERMSIG(status);
            end_output(childs[i], 3); // ESTATUS DE CONTROL Z
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
    // Search for index
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
                printf("HOLIII\n");
                end_time[i] = (double)clock()/(double)CLOCKS_PER_SEC;
                p_status[i] = 100; // hay que ponerle un status.
                actual_status[i] = "FINISHED";
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