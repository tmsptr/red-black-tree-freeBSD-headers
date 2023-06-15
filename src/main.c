#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/JSONtoText.h"
#include "../include/cJSON.h"
#include "../include/computeHash.h"
#include "../include/foundNames.h"
#include "../include/printNames.h"
#include "../include/queue.h"
#include "../include/searchByName.h"
#include "../include/sortNamesInFile.h"
#include "../include/structures.h"
#include "../include/tree.h"
#include "../include/xxhash.h"

#define MAX_NAME_LENGTH 256

/*
===========================================
COMPARE NODES
===========================================
*/

int compareNodes(struct TreeNode* a, struct TreeNode* b) {
  if (a->key < b->key) {
    return -1;
  } else if (a->key > b->key) {
    return 1;
  } else {
    return 0;
  }
}

RB_HEAD(RBTree, TreeNode); /* Root */
RB_PROTOTYPE(RBTree, TreeNode, entry,
             compareNodes) /* Generate function prototypes */
RB_GENERATE(RBTree, TreeNode, entry, compareNodes) /* Generate functions */

/*
===========================================
POPULATE TREE
===========================================
*/

void populateTree(struct RBTree* tree, const char* fileName) {
  FILE* file = fopen(fileName, "r");
  if (file == NULL) {
    printf("Error opening file.\n");
    return;
  }

  char line[MAX_NAME_LENGTH];
  while (fgets(line, sizeof(line), file)) {
    /* Extract full name */
    int fullNameLength = 0;
    char* token = strtok(line, " \t\n");
    char** fullName = (char**)malloc(sizeof(char*) * MAX_NAME_LENGTH);
    while (token != NULL) {
      fullName[fullNameLength] = strdup(token);
      fullNameLength++;
      token = strtok(NULL, " \t\n");
    }

    /* Calculate hash */
    unsigned int hash = computeHash(fullName[0]);

    /* Create new tree node */
    struct TreeNode* node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    node->key = hash;
    node->fullName = fullName;
    node->fullNameLength = fullNameLength;
    TAILQ_INIT(&(node->duplicates));

    /* Insert node into tree */
    struct TreeNode* existingNode = RB_INSERT(RBTree, tree, node);

    if (existingNode != NULL) {
      /* Add to the queue */
      struct DuplicateNode* duplicate =
          (struct DuplicateNode*)malloc(sizeof(struct DuplicateNode));
      duplicate->fullName = fullName;
      duplicate->fullNameLength = fullNameLength;
      TAILQ_INSERT_TAIL(&(existingNode->duplicates), duplicate, entries);
    }
  }

  fclose(file);
}

/*
===========================================
FIRST OPTION ONE
===========================================
*/

int handleFirstOptionOne() {
  struct RBTree tree;
  RB_INIT(&tree);

  char fileName[MAX_NAME_LENGTH];
  printf("Enter the file name (including relative path) to read from: ");
  scanf("%s", fileName);

  sortNamesInFile(fileName, "tmp.txt");
  populateTree(&tree, "tmp.txt");
  remove("tmp.txt");

  printf("\n");
  printf("=========== RB TREE ==========\n");
  printf("\n");
  printNames(&tree, RB_ROOT(&tree), NULL, 0);
  printf("\n");
  printf("==============================\n\n");
  printf("\n");

  /* JSON serialization */
  cJSON* jsonTree = cJSON_CreateArray();
  struct TreeNode *node, *nextNode;
  RB_FOREACH_SAFE(node, RBTree, &tree, nextNode) {
    cJSON* jsonNode = cJSON_CreateObject();
    cJSON_AddNumberToObject(jsonNode, "key", node->key);

    cJSON* fullNameArray = cJSON_CreateArray();
    for (int i = 0; i < node->fullNameLength; i++) {
      cJSON_AddItemToArray(fullNameArray,
                           cJSON_CreateString(node->fullName[i]));
    }
    cJSON_AddItemToObject(jsonNode, "fullName", fullNameArray);

    cJSON* duplicatesArray = cJSON_CreateArray();
    struct DuplicateNode* duplicate;
    TAILQ_FOREACH(duplicate, &(node->duplicates), entries) {
      cJSON* duplicateNode = cJSON_CreateObject();
      cJSON* duplicateFullNameArray = cJSON_CreateArray();
      for (int i = 0; i < duplicate->fullNameLength; i++) {
        cJSON_AddItemToArray(duplicateFullNameArray,
                             cJSON_CreateString(duplicate->fullName[i]));
      }
      cJSON_AddItemToObject(duplicateNode, "fullName", duplicateFullNameArray);
      cJSON_AddItemToArray(duplicatesArray, duplicateNode);
    }
    cJSON_AddItemToObject(jsonNode, "duplicates", duplicatesArray);

    cJSON_AddItemToArray(jsonTree, jsonNode);

    if (node == RB_ROOT(&tree)) {
      cJSON_AddStringToObject(jsonNode, "root", "true");
    }
  }

  char* jsonString = cJSON_Print(jsonTree);
  cJSON_Delete(jsonTree);

  /* Remove extension */
  char* dot = strrchr(fileName, '.');
  if (dot != NULL && strcmp(dot, ".txt") == 0) {
    *dot = '\0';
  }

  /* JSON -> TXT */
  char jsonFileName[MAX_NAME_LENGTH];

  strcpy(jsonFileName, fileName);
  strcat(jsonFileName, ".json");

  FILE* jsonFile = fopen(jsonFileName, "w");
  if (jsonFile != NULL) {
    fputs(jsonString, jsonFile);
    fclose(jsonFile);
    printf("JSON data written to %s\n", jsonFileName);
  } else {
    perror("Error writing JSON data to file.\n");
  }

  free(jsonString);

  /* FREE */
  struct TreeNode *currentNode, *tmpNode;
  RB_FOREACH_SAFE(currentNode, RBTree, &tree, tmpNode) {
    RB_REMOVE(RBTree, &tree, currentNode);
    free(currentNode);
  }
}

/*
===========================================
FIRST OPTION TWO
===========================================
*/

int handleFirstOptionTwo() {
  char fileName[MAX_NAME_LENGTH];
  printf("Enter the file name (including relative path) to read from: ");
  scanf("%s", fileName);

  FILE* file = fopen(fileName, "r");
  if (file == NULL) {
    printf("Error opening file.\n");
    return 1;
  }

  /* Get size */
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  /* Allocate buffer */
  char* jsonBuffer = (char*)malloc(fileSize + 1);
  if (jsonBuffer == NULL) {
    printf("Error allocating memory for JSON buffer.\n");
    fclose(file);
    return 1;
  }

  /* Read into buffer */
  size_t bytesRead = fread(jsonBuffer, 1, fileSize, file);
  if (bytesRead != fileSize) {
    printf("Error reading file.\n");
    fclose(file);
    free(jsonBuffer);
    return 1;
  }

  /* Null terminate string */
  jsonBuffer[fileSize] = '\0';
  convertJSONtoText(jsonBuffer, "tmp.txt");

  int secondOption;
  printf("Loaded %s successfully", fileName);

  struct RBTree tree;
  RB_INIT(&tree);

  sortNamesInFile("tmp.txt", "tmp2.txt");
  populateTree(&tree, "tmp2.txt");

  printf("Choose operation:\n");
  printf("1. Print\n");
  printf("2. Search\n");
  printf("3. Delete\n");
  printf("Operation: ");
  scanf("%d", &secondOption);

  switch (secondOption) {
    case 1:
      //   remove("tmp.txt");
      printNames(&tree, RB_ROOT(&tree), NULL, 0);
      break;
    case 2:
      char searchValue[MAX_NAME_LENGTH];
      printf("Enter the first name you are looking for: ");
      scanf("%s", searchValue);
      searchByName(&tree, RB_ROOT(&tree), searchValue);
      break;
    case 3:
      // Delete
      break;
    default:
      printf("Invalid operation.\n");
      break;
  }

  fclose(file);
  free(jsonBuffer);
  return 0;
}
/*
===========================================
MAIN
===========================================
*/

int main() {
  int firstOption;
  printf("Choose an option:\n");
  printf("1. Store list into RBTree JSON\n");
  printf("2. Load RBTree from JSON\n");
  printf("Option: ");
  scanf("%d", &firstOption);

  switch (firstOption) {
    case 1:
      handleFirstOptionOne();
      break;
    case 2:
      handleFirstOptionTwo();
      break;
    default:
      printf("Invalid option.\n");
      break;
  }
  return 0;
}
