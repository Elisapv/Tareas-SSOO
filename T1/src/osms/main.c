#include "../osms_API/osms_API.h"
#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc

int main(int argc, char const *argv[])
{
    os_mount((char *)argv[1]); // montar la memoria
    // os_start_process(0, "auction"); // Se crea el proceso 0
    // os_upload(0, "kurta.eye"); // Proceso 0 carga un archivo en memoria
    // os_start_process(1, "kortopi"); // Se crea otro proceso
    // uint8_t *buffer = malloc(74); // crea el buffer
    // OsmsFile* file = os_open(0, "kurta.eye", 'r'); // abre el archivo
    // os_read(file, buffer, 72); // copia el archivo al buffer
    // os_close(file); // cierra el archivo
    // OsmsFile * file = os_open(1, "kurtaCopy.eye", 'w'); // abre un archivo nuevo
    // os_write(file, buffer, 72); // escribe la copia del archivo
    // os_close(file); // cierra el archivo
    // free(buffer); // libera el buffer
    // os_delete_file(0, "kurta.eye"); // lo elimina
    // os_finish_process(0); // cierra el proceso
    // os_finish_process(1);
}