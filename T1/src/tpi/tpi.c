#include "tpi.h"

extern FILE* MEM;
static const int PROCESS_SIZE = 256;
static const int PROCESS_MAX = 32;
static const int PROCESS_FILE_SIZE = 24;
static const int PROCESS_FILE_MAX = 10;

// Tabla páginas invertida (REVISAR NÚMEROS PARA QUE SEAN COMO LOS DE NUESTRA TAREA)
static const int TPI_ENTRY_SIZE = 3;
static const int TPI_ENTRY_MAX = 65536;
static const int PAGE_SIZE = 32768;
static const int PROCESS_VIRT_MEM_SIZE = 268435456;
static const int VIR_MEM_PAGE_SIZE = 32768;
static const int MAX_FILE_SIZE = 67108864;
static const int MAX_VIR_MEM_PAGES = 8192;

// Read inverted page table
unsigned int vpn_to_pfn(int process_id, int vpn) {
  char buffer[TPI_ENTRY_SIZE * TPI_ENTRY_MAX];
  fseek(MEM, PROCESS_MAX * PROCESS_SIZE, SEEK_SET);
  fread(buffer, 1, TPI_ENTRY_SIZE * TPI_ENTRY_MAX, MEM);

  // Find entry
  int pfn = -1;
  for (int i = 0; i < IPT_ENTRY_MAX; i++) {
    
    char row[TPI_ENTRY_SIZE];
    for (int j = 0; j < TPI_ENTRY_SIZE; j++) {
      row[j] = buffer[TPI_ENTRY_SIZE * i + TPI_ENTRY_SIZE - 1 - j];
    }

    if (((unsigned char)row[0] >> 7 ) != 0b1) {
      continue;
    }

    int pid = ((unsigned char)row[0] & 0b01111111) << 3;
    pid |= ((unsigned char)row[1] & 0b11100000) >> 5;

    if (pid != process_id) {
      continue;
    }

    int row_vpn = ((unsigned char)row[1] & 0b00011111) << 8;
    row_vpn |= (unsigned char)row[2];

    if (row_vpn != vpn) {
      continue;
    }

    // Read physical address
    pfn = i;
    break;
  }

  return (unsigned int)pfn;
}