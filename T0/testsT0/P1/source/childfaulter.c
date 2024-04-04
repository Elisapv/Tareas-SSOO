#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

void sigint_handler(int signum) {
    // No hacer nada
}

int main(int argc, char **argv){
    double begin = clock();
    signal(SIGINT, sigint_handler);
    printf("%s: Iniciado...\n", argv[0]);
    while((double)(clock() - begin) / CLOCKS_PER_SEC < 15);
    return 1;
}