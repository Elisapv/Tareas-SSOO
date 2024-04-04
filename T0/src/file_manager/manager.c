// Import used global libraries
#include <stdio.h>  // FILE, fopen, fclose, etc.
#include <string.h> // strtok, strcpy, etc.
#include <stdlib.h> // malloc, calloc, free, etc.

// Import the header file of this module
#include "manager.h"

/*
* Splits a string "str" by a separator "sep", returns an array with The
* resulting strings. Equivalent to Python's str.split(sep).
*/
static char** split_by_sep(char* str, char* sep)
{
  char** new_str = calloc(MAX_SPLIT, sizeof(char*));
  int index = 0;

  char* token = strtok(str, sep);
  while (token != NULL) {
    new_str[index] = calloc(BUFFER_SIZE, sizeof(char));
    strcpy(new_str[index++], token);
    token = strtok(NULL, sep);
  }
  return new_str;
}

/*
* Reads a generic file with the following structure:
*
* 1. n
* 2. data,data,data,...
* ...
* n+1. data,data,data,...
*
* Where "n" is the amount of data lines where each of data lines are
* comma-separated. The file is returned as a InputFile struct.
*/
InputFile* read_file(char* filename)
{
  // Read the file
  FILE* file_pointer = fopen(filename, "r");

  // Declare a buffer and read the first line
  char buffer[BUFFER_SIZE];
  fgets(buffer, BUFFER_SIZE, file_pointer);

  // Define the struct and prepare its attributes
  InputFile* input_file = malloc(sizeof(InputFile));
  input_file->lines = calloc(atoi(buffer), sizeof(char**));
  input_file->len = atoi(buffer);

  // Parse each line
  int index = 0;
  while (fgets(buffer, BUFFER_SIZE, file_pointer)) {
    buffer[strcspn(buffer, "\n")] = 0;
    buffer[strcspn(buffer, "\r")] = 0;
    input_file->lines[index++] = split_by_sep(buffer, " ");
  }

  // Close the file and return the struct
  fclose(file_pointer);
  return input_file;
}

/*
* Frees all the memory used by "input_file".
*/
void input_file_destroy(InputFile* input_file)
{
  // Free each separated data line and its contents
  for (int i = 0; i < input_file->len; i++) {
    for (int j = 0; j < MAX_SPLIT; j++) {
      free(input_file->lines[i][j]);
    }
    free(input_file->lines[i]);
  }

  // Free the lines array
  free(input_file->lines);

  // Free the input_file itself
  free(input_file);
}