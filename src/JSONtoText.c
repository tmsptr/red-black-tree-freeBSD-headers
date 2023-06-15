#include "../include/JSONtoText.h"

#include <stdio.h>
#include <stdlib.h>

#include "../include/cJSON.h"

void convertJSONtoText(const char* json, const char* outputFile) {
  FILE* file = fopen(outputFile, "w");
  if (file == NULL) {
    printf("Error opening the output file.\n");
    return;
  }

  cJSON* root = cJSON_Parse(json);
  if (root == NULL) {
    printf("Error parsing JSON.\n");
    fclose(file);
    return;
  }

  cJSON* item = NULL;
  cJSON_ArrayForEach(item, root) {
    cJSON* fullNameArray = cJSON_GetObjectItem(item, "fullName");
    if (fullNameArray != NULL && cJSON_IsArray(fullNameArray)) {
      cJSON* nameItem = fullNameArray->child;
      while (nameItem != NULL) {
        if (nameItem->valuestring != NULL) {
          fprintf(file, "%s", nameItem->valuestring);
          if (nameItem->next != NULL) {
            fprintf(file, " ");
          }
        }
        nameItem = nameItem->next;
      }
      fprintf(file, "\n");
    }

    cJSON* duplicatesArray = cJSON_GetObjectItem(item, "duplicates");
    if (duplicatesArray != NULL && cJSON_IsArray(duplicatesArray)) {
      cJSON* duplicateItem = NULL;
      cJSON_ArrayForEach(duplicateItem, duplicatesArray) {
        cJSON* duplicateFullNameArray =
            cJSON_GetObjectItem(duplicateItem, "fullName");
        if (duplicateFullNameArray != NULL &&
            cJSON_IsArray(duplicateFullNameArray)) {
          cJSON* duplicateNameItem = duplicateFullNameArray->child;
          while (duplicateNameItem != NULL) {
            if (duplicateNameItem->valuestring != NULL) {
              fprintf(file, "%s", duplicateNameItem->valuestring);
              if (duplicateNameItem->next != NULL) {
                fprintf(file, " ");
              }
            }
            duplicateNameItem = duplicateNameItem->next;
          }
          fprintf(file, "\n");
        }
      }
    }
  }

  cJSON_Delete(root);
  fclose(file);
}