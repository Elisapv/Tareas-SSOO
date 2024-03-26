#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv){
    double begin = clock();
    if(argc < 4){
        printf("No existen suficientes argumentos\n");
        exit(1);
    }
    int min_time = atoi(argv[1]);
    printf("%s: Iniciado...\n", argv[0]);
    printf("%s: String recibido como argumento %s...\n", argv[0], argv[2]);
    while((double)(clock() - begin) / CLOCKS_PER_SEC < min_time);
    exit(atoi(argv[3]));
}