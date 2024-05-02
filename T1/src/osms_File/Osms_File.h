#pragma once
#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../tpi/tpi.h"
#include "../osms_API/Osms_API.h"

// Aquí se define la estructura de un Osms_File
typedef struct osmsfile {
	int process_id;
	char* file_name;    // name of the file
	char* file_size;    // size of the file
	char* path;         // path of the file
	char* content;      // discutible
	char mode;         // r o w (if is w, check if the file doesn't exist)
	uint64_t read_position;  //  Posición actual de lectura (recomendación de chat)
} OsmsFile;


int find_file (int process_id, char* file_name);
int find_process(int process_id);

// funciones de archivos

OsmsFile* os_open(int process_id, char* file_name, char mode);

// uint64_t os_write(OsmsFile* file_desc, uint8_t* buffer, uint64_t n_bytes); // pendiente.

uint64_t os_read(OsmsFile* file_desc, uint8_t* buffer, uint64_t n_bytes);

uint64_t os_upload(int process_id, char* file_path);

// uint64_t os_download(OsmsFile* file_desc, char* dest);

// void os_delete_file(int process_id, char* file_name);

// void os_close(OsmsFile* file_desc);


// Extras

char* reverse_bytes(char* buffer, int n_bytes);
OsmsFile* init_osms_file(char* file_name, init process_id, char mode, int size);
/*
  Get the physical address of a file in a process. If the file is not found, it returns -1.
  The address will be equal to (PFN + 1) * PAGE_SIZE - offset 
*/
unsigned int get_physical_address(int process_id, int vpn, int offset);