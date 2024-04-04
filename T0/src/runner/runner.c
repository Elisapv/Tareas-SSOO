#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include "./runner.h"


ProcessInfo process_info[MAX_PROCESSES];
int current_process = 0;
int max_processes;

void start_process(const char *program, char *const argv[]) {
    printf("%s %s\n","file", program);
    printf("%s %s\n","argumentos", *argv);
    pid_t pid = fork();
    if (pid == 0) {
        printf("Pid %i\n", pid);
        // Proceso hijo
        execvp(program, argv);
        perror("execvp"); // Imprime error si execvp falla
        exit(EXIT_FAILURE);
     } else if (pid > 0) {
        // Proceso padre
        printf("Parent process PID: %i, Child process PID: %i\n", getpid(), pid);

        int status;
        waitpid(pid, &status, 0); // Espera a que el proceso hijo termine

        if (WIFEXITED(status)) {
            printf("Child process exited with status %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child process killed by signal %d\n", WTERMSIG(status));
        }
    } else {
        // Error en fork
        perror("fork");
        exit(EXIT_FAILURE);
    }

}


void setup_signal_handlers() {
    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);
    signal(SIGTSTP, on_signal);
}

void handler_sigint(int signal) {
  if (signal == SIGTSTP) {
    printf("Recibida la señal SIGTSTP (Ctrl+Z)\n");
    // Suspender el proceso principal
    kill(getpid(), SIGSTOP);
  }
}

void on_signal(int signum) {
    for (int i = 0; i < current_process; i++) {
        if (process_info[i].pid != 0) {
            kill(process_info[i].pid, signum);
            process_info[i].end_time = time(NULL);
            process_info[i].status = signum;
        }
    }
}

void collect_statistics(const char *output_path) {
    FILE *file = fopen(output_path, "w");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < current_process; i++) {
        fprintf(file, "%s,%ld,%d\n",
                process_info[i].name,
                process_info[i].end_time - process_info[i].start_time,
                process_info[i].status);
    }
    fclose(file);
}

// int main(int argc, char *argv[]) {
//     if (argc < 4) {
//         fprintf(stderr, "Usage: %s <input> <output> <amount> [<max>]\n", argv[0]);
//         exit(EXIT_FAILURE);
//     }

//     const char *input_file = argv[1];
//     const char *output_file = argv[2];
//     max_processes = atoi(argv[3]);

//     setup_signal_handlers();

//     FILE *file = fopen(input_file, "r");
//     if (!file) {
//         perror("fopen");
//         exit(EXIT_FAILURE);
//     }

//     char line[LINE_BUFFER_SIZE];
//     while (fgets(line, sizeof(line), file) && current_process < max_processes) {
//         char *args[MAX_ARGS];
//         char *token = strtok(line, " \n");
//         int i = 0;
//         while (token && i < MAX_ARGS) {
//             args[i++] = token;
//             token = strtok(NULL, " \n");
//         }
//         args[i] = NULL;  // execvp expects a NULL at the end of the arguments array
//         if (i > 1) {  // We need at least a program name
//             start_process(args[0], args);
//         }
//     }

//     fclose(file);

//     // Wait for all child processes to exit
//     while (wait(NULL) > 0);

//     collect_statistics(output_file);

//     return 0;
// }
