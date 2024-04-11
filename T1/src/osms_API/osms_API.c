#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <string.h> //para strcmp
#include <stdbool.h> // bool, true, false

char* path;

// funciones generales

void os_mount(char* memory_path){
    path = memory_path;
}