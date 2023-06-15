#include "../include/foundNames.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/structures.h"

void addName(struct NameList* list, const char* name) {
  if (list->count == 0) {
    list->names = malloc(sizeof(char*));
  } else {
    list->names = realloc(list->names, (list->count + 1) * sizeof(char*));
  }

  if (list->names == NULL) {
    printf("Memory allocation failed.\n");
    return;
  }

  list->names[list->count] = strdup(name);
  if (list->names[list->count] == NULL) {
    printf("Memory allocation failed.\n");
    return;
  }

  list->count++;
}

void freeNames(struct NameList* list) {
  for (int i = 0; i < list->count; i++) {
    free(list->names[i]);
  }
  free(list->names);
  list->count = 0;
}

void printFoundNames(struct NameList* list) {
  for (int i = 0; i < list->count; i++) {
    printf("%s\n", list->names[i]);
  }
}

void printNamesToFile(const char* fileName, struct NameList* list) {
  FILE* file = fopen(fileName, "w");
  if (file == NULL) {
    printf("Error opening file for writing.\n");
    return;
  }

  for (int i = 0; i < list->count; i++) {
    fprintf(file, "%s\n", list->names[i]);
  }

  fclose(file);
}