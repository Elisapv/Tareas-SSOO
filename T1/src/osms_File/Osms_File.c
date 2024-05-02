#include "./Osms_File.h"

extern FILE* MEM;
static const int PROCESS_SIZE = 256;
static const int PROCESS_MAX = 32;
static const int PROCESS_FILE_SIZE = 24;
static const int PROCESS_FILE_MAX = 10;



int find_file(int process_id, char* file_name) {
	unsigned char buffer[PROCESS_SIZE];
	int pid_entry = find_process(process_id);

	if (pid_entry != -1) {
		fseek(MEM, pid_entry * PROCESS_SIZE, SEEK_SET); // revisar lo de la memoria
		fread(buffer, 1, PROCESS_SIZE, MEM);

		// Find the file in the PCB table
		for (int i = 0; i < PROCESS_FILE_MAX; i++) {
			if (buffer[16 + i * PROCESS_FILE_SIZE] == (char)1 && memcmp(&buffer[17 + i * PROCESS_FILE_SIZE], file_name, 15) == 0) {
				return i;
			}
		}
	}

	return -1;
}


int find_process(int process_id) {
	unsigned char buffer[PROCESS_SIZE];
	int position = -1;
	// Find a free entry in the PCB table
	for (int i = 0; i < PROCESS_MAX; i++) {
		fseek(MEM, i * PROCESS_SIZE, SEEK_SET);
		fread(buffer, 1, PROCESS_SIZE, MEM);

		// If the process is not running, we can use this entry
		if (buffer[0] == 1 && buffer[1] == process_id) {
			position = i;
			break;
		}
	}

	if (position == -1) {
		printf("No hay espacio en la tabla de procesos\n");
	}

	return position;
}


// No se cual es la funcion de las  3 siguientes funciones pero son necesarias para os_open
char* reverse_bytes(char* buffer, int n_bytes) {
	unsigned char* new_buffer = malloc(n_bytes);
	for (int i = 0; i < n_bytes; i++) {
		new_buffer[i] = buffer[n_bytes - i - 1];
	}
	return new_buffer;
}

OsmsFile* init_osms_file(char* file_name, init process_id, char mode, int size) {
    OsmsFile* file = malloc(sizeof(OsmsFile));
    file->process_id = process_id;
    strcpy(file->file_name, file_name);
    file->mode = mode;
    file->content = NULL;
    file->path = NULL;
    file->file_size = size;
    file->read_position  0;
    return file;
}

unsigned int get_physical_address(int process_id, int vpn, int offset) {
  unsigned int pfn = vpn_to_pfn(process_id, vpn);
  if (pfn == -1) {
    return -1;
  }

  return ((pfn + 1) * PAGE_SIZE) - offset;
}

/////////////////
OsmsFile* os_open(int process_id, char* file_name, char mode) {
    unsigned char * buffer = procecess_find_file(process_id, file_name); // esta función no la tenemos

    // Modo de lectura
    if (mode == 'r') {
        if (buffer == NULL) {
            // se intento leer un archivo que no existe
            return NULL;
        }

        // obtener los valores del buffer (ESTO TENEMOS QUE REVISARLO)
        /*
	    BUFFER
	    [0] = 1
	    [1-15] = file_name
	    [16-19] = file_size
	    [20-23] = Virtual address
	    */
        unsigned char valid_byte = (char)buffer[0];
        unsigned char file_name[15]
        /*CHAT
        memcpy(file_name, &buffer[1], 15); // Copiar nombre del archivo
        file_name[14] = '\0'; // Asegurar que termine correctamente
        */

        int file_size = 0;
        // pq 4?
        for (int j = 0; j < 4; j++) {
            file_size |= ((unsigned char)buffer[16 + j] << (8 * j));
        }

        unsigned char reverse_virtual_addres[4];
        memcpy(reverse_virtual_addres, &buffer[20], 4);
        unsigned char* virtual_address = reverse_bytes(reverse_virtual_addres, 4); // esta funcion no la tengo

        // I need to get the first 4 bits no significative + 13 bits VPN + 15 offset of the virtual address
		int vpn = (virtual_address[0] << 9) | (virtual_address[1] << 1) | (virtual_address[2] >> 7);
		int offset = (virtual_address[2] << 8) | virtual_address[3];

		int physical_address = get_physical_address(process_id, vpn, offset); // esta funcion no la tenemos 

        // Create the file
		OsmsFile* file = init_osms_file(file_name, process_id, 'r', file_size); // Esta funcion no la tenemos

        long int absolute_address = pow(2, 13) + 3 * pow(2, 16) + pow(2, 13) + physical_address;

        // Need memory for the content
		file->content = malloc(file_size * sizeof(char));
		// Copy the content from memory to the file content
		memcpy(file->content, &MEM[absolute_address], file_size);

        // free used memory
		free(buffer);
		free(virtual_address);

		return file;

    } else if (mode == "w") { // modo escribir
        if (buffer != NULL) {
            return NULL;
        }
        OsmsFile* file = init_osms_file(file_name, process_id, 'w', 0) // Esta funcion no la tenemos
        return file;
    } else {
        printf("Modo invalido\n");
        exit(1);
        return NULL;
    }
}


// PENDIENTE PARA ESCRIBIR EN ARCHIVOS
// uint64_t os_write(OsmsFile* file_desc, uint8_t* buffer, uint64_t n_bytes)

// LEER EN ARCHIVOS
uint64_t os_read(OsmsFile* file_desc, uint8_t* buffer, uint64_t n_bytes) {
    // Check if the file exists
	if (file_desc == NULL) {
		printf("El archivo no existe\n");
		return 0; // retornar 0 bytes leídos
	}

	// Check if the file has content
	if (file_desc->content == NULL) {
		printf("El archivo está vacío\n");
		return 0;
	}

	// Read the file from the virtual memory
	// read_virtual_memory(file_desc->path, buffer, n_bytes); // está función no la tengo y ellos la tienen pendiente....

    // Calcular los bytes restantes para leer
    uint64_t bytes_restantes = atoi(file_desc->file_size) - file_desc->read_position;
    uint64_t bytes_a_leer = (n_bytes > bytes_restantes) ? bytes_restantes : n_bytes;

    if (bytes_a_leer <= 0) {
        return 0; // No hay más contenido que leer
    }

    // Leer desde la posición actual
    memcpy(buffer, file_desc->content + file_desc->read_position, bytes_a_leer);

    // Actualizar la posición de lectura
    file_desc->read_position += bytes_a_leer;

    return bytes_a_leer; // Devuelve el número de bytes efectivamente leídos
}


// UPLOAD 
uint64_t os_upload(int process_id, char* file_path) {
    // Verificar que el proceso exista
	int process_position = process_find(process_id); // esta no existe en mi tarea
	if (process_position == -1) {
		printf("El proceso no existe\n");
		return -1; // verificar que devolver en este caso...
	}

	// Verificar que el archivo exista
	FILE* file = fopen(file_path, "rb");
	if (file == NULL) {
		printf("El archivo no existe o no se puede abrir\n");
		return -1; // verificar que devolver
	}

	char* file_name = basename(file_path);  // está función no la tenemos
    // unsigned char* buffer = process_find_file(process_id, file_name); // Esta funcion no la tenemos

    // CHAT
    fseek(file, 0, SEEK_END); // Ir al final para obtener el tamaño del archivo
    uint64_t file_size = ftell(file); // Tamaño del archivo
    fseek(file, 0, SEEK_SET); // Volver al inicio para leer

    // Crear espacio para el contenido del archivo
    uint8_t* content = (uint8_t*)malloc(file_size); // Alocar memoria para el contenido

    // Leer el contenido del archivo en la memoria alocada
    size_t bytes_leidos = fread(content, 1, file_size, file);
    fclose(file); // Cerrar el archivo después de leer

    // Aquí debería haber una función para escribir el contenido en la memoria virtual del proceso
    // Por ejemplo, algo como `write_to_virtual_memory(process_id, file_name, content, bytes_leidos)`

    // Para esta implementación, vamos a retornar el número de bytes leídos
    return bytes_leidos; // Devuelve el número de bytes que se pudieron escribir

}