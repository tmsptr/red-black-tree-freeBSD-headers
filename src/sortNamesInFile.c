#include "../include/sortNamesInFile.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 256

int compareNames(const void* a, const void* b) {
  const char* nameA = *(const char**)a;
  const char* nameB = *(const char**)b;
  return strcmp(nameA, nameB);
}

int sortNamesInFile(const char* inputFileName, const char* outputFileName) {
  FILE* inputFile = fopen(inputFileName, "r");
  if (inputFile == NULL) {
    printf("Error opening input file.\n");
    return 1;
  }

  FILE* outputFile = fopen(outputFileName, "w");
  if (outputFile == NULL) {
    printf("Error opening output file.\n");
    fclose(inputFile);
    return 1;
  }

  char line[MAX_NAME_LENGTH];
  char** names = NULL;
  int numNames = 0;

  while (fgets(line, sizeof(line), inputFile)) {
    /* Remove newline chars */
    if (line[strlen(line) - 1] == '\n') {
      line[strlen(line) - 1] = '\0';
    }

    /* Get Mem */
    char* name = (char*)malloc(strlen(line) + 1);
    strcpy(name, line);

    /* Add to array */
    names = realloc(names, sizeof(char*) * (numNames + 1));
    names[numNames] = name;
    numNames++;
  }

  fclose(inputFile);
  qsort(names, numNames, sizeof(char*), compareNames);

  /* Write sorted to the file */
  for (int i = 0; i < numNames; i++) {
    fprintf(outputFile, "%s\n", names[i]);
    free(names[i]);
  }
  free(names);
  fclose(outputFile);
  return 0;
}
