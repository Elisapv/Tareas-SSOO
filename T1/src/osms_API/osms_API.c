#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <string.h> //para strcmp
#include <stdbool.h> // bool, true, false
#include "./osms_API.h"

char* path;
FILE *MEM;
static const int PROCESS_SIZE = 256;
static const int PROCESS_MAX = 32;
static const int PROCESS_FILE_SIZE = 24; // esto es distinto
static const int PROCESS_FILE_MAX = 10;  // esto es distinto

// funciones generales

void os_mount(char* memory_path){
    path = memory_path;
    MEM = fopen(path, "r+b"); // supongo que hay que abrir la memoria una vez
    // if (MEM == NULL) { // Abre el archivo solo si no está ya abierto
    //     MEM = fopen(memory_path, "rb");
    // }
}

/* Imprimir en pantalla los procesos existente */
void os_ls_process() {

    unsigned char buffer[PROCESS_SIZE];
    for (int i = 0; i < PROCESS_MAX; i++) {
        fseek(MEM, i* PROCESS_SIZE, SEEK_SET); // revisar que hace esto
        fread(buffer, 1, PROCESS_SIZE, MEM);   // revisar que hace esto

        if (buffer[0] == (char)1) {
            char process_name[14];
            memcpy(process_name, &buffer[2], 14); // revisar que hace esto

            process_name[13] = '\0'; // Asegura el carácter nulo para terminar la cadena

            int process_id = (int)buffer[1];
            printf("%s %d\n", process_name, process_id); // revisar que hace acá
        }
    }
}

/* Verificar si un archivo existe en un proceso */
int os_exists(int process_id, char* file_name) {
    // Retona 1 si existe y 0 en caso contrario
    int file_entry = find_file(process_id, file_name);
	return file_entry != -1;
}

/* Se imprime en pantalla una lista de archivos dentro de un procesos */
void os_ls_files(int process_id) {
    
    unsigned char buffer[PROCESS_SIZE];
	int pid_entry = find_process(process_id);

	if (pid_entry != -1) {
		fseek(MEM, pid_entry * PROCESS_SIZE, SEEK_SET);
		fread(buffer, 1, PROCESS_SIZE, MEM);

    // Find the file in the PCB table
    for (int i = 0; i < PROCESS_FILE_MAX; i++) {
      if (buffer[16 + i * PROCESS_FILE_SIZE] == (char)1) {
        char file_name[15];
        memcpy(file_name, &buffer[17 + i * PROCESS_FILE_SIZE], 15);

        int size = 0;
        for (int j = 0; j < 4; j++) {
          size += (int)buffer[32 + i * PROCESS_FILE_SIZE + j] << (8 * j);
        }
        unsigned char va[4];
        for (int j = 3; j >= 0; j--) {
          va[3 - j] = buffer[36 + i * PROCESS_FILE_SIZE + j];
        }

        int vpn = (va[0] << 9) | (va[1] << 1) | (va[2] >> 7);
        int offset = ((va[2] & 0x7f) << 8) | va[3];

        // int pfn = vpn_toK_pfn(process_id, vpn); // parece que no lo necesito
        int virtual_address = (vpn << 9) + offset;

        // <VPN> <File_Size> <Dirección Virtual> <File_Name>
        printf("%d %d %d %s\n", vpn, size, virtual_address, file_name);
      }
    }
  }
}

/* Mostrar en pantalla el estado del Frame Bitmap */
void os_frame_bitmap(int PFN_i, int PFN_f) {
    fseek(MEM, PFN_i, SEEK_SET);

    int used = 0; // Para contar frames usados
    int free = 0; // Para contar frames libres

    for (int i = PFN_i; i <= PFN_f; i++) {
        unsigned char frame_status;         // Para almacenar el estado del frame
        fread(&frame_status, 1, 1, MEM);    // Leer el estado del frame

        if (frame_status == 1) {
            used++;                         // Aumentar el conteo de usados
        } else {
            free++;                         // Aumentar el conteo de libres
        }

        // Imprimir el estado del frame (sin espacios entre ellos)
        printf("%d", frame_status);
    }
    printf("\n");
    printf("USADOS %d\n", used);
    printf("LIBRES %d\n", free);
}


// funciones para procesos
int os_start_process(int process_id, char* process_name) {
    unsigned char buffer[PROCESS_SIZE];
    // int free_entry = -1;

    // Buscar una entrada libre en la tabla de PCBs
    for (int i = 0; i < PROCESS_MAX; i++) {
        fseek(MEM, i * PROCESS_SIZE, SEEK_SET); // Posicionar al inicio de la entrada
        fread(buffer, 1, PROCESS_SIZE, MEM); // Leer la entrada del PCB

        if (buffer[0] == 0) { // Si está libre
            // free_entry = i;
            buffer[0] = (char)1; // Marcar como ocupado
            buffer[1] = (char)process_id; // Establecer el ID del proceso
            memcpy(&buffer[2], process_name, 14); // Copiar el nombre del proceso
            buffer[15] = '\0'; // Asegurarse de que el nombre termine correctamente

            // Escribir los datos del proceso en la entrada libre
            fseek(MEM, i * PROCESS_SIZE, SEEK_SET); // Volver al inicio de la entrada
            fwrite(buffer, 1, PROCESS_SIZE, MEM); // Escribir en la entrada del PCB

            return 0;
        }
    }

    // Si no se encuentra una entrada libre, error
    return -1; // Indica que no se pudo iniciar el proceso
}

int os_finish_process(int process_id) {
    unsigned char buffer[PROCESS_SIZE];
    int pid_entry = find_process(process_id);

    if (pid_entry == -1) {
        return -1;
    }

    fseek(MEM, pid_entry * PROCESS_SIZE, SEEK_SET);
    fread(buffer, 1, PROCESS_SIZE, MEM);

    buffer[0] = (char)0;

    for (int j = 0; j < PROCESS_FILE_MAX; j++) {
        buffer[16 + j * PROCESS_FILE_SIZE] = (char)0;
    }

    fseek(MEM, pid_entry * PROCESS_SIZE, SEEK_SET);
    fwrite(buffer, 1, PROCESS_SIZE, MEM);
    return 0;
}


// funciones para manejo de archivos