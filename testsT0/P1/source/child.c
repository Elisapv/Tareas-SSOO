#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv){
    double begin = clock();
    printf("%s: Iniciado...\n", argv[0]);
    while((double)(clock() - begin) / CLOCKS_PER_SEC < 3);
    return 1;
}