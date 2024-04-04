#pragma once
#include <sys/wait.h>
#include <time.h>

#define MAX_ARGS 10
#define LINE_BUFFER_SIZE 1024
#define MAX_PROCESSES 100

typedef struct {
    pid_t pid;
    char name[LINE_BUFFER_SIZE];
    time_t start_time;
    time_t end_time;
    int status;
} ProcessInfo;

void start_process(const char *program, char *const argv[]);
void setup_signal_handlers();
void on_signal(int signum);
void collect_statistics(const char *output_path);
void handler_sigint(int signal);
