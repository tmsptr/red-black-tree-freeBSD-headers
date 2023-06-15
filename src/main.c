#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/JSONtoText.h"
#include "../include/cJSON.h"
#include "../include/computeHash.h"
#include "../include/foundNames.h"
#include "../include/populateTree.h"
#include "../include/printNames.h"
#include "../include/queue.h"
#include "../include/searchByName.h"
#include "../include/serializeToJSON.h"
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

/*
===========================================
MACROS
===========================================
*/

RB_HEAD(RBTree, TreeNode);                         /* Root */
RB_GENERATE(RBTree, TreeNode, entry, compareNodes) /* Generate functions */

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

  if (sortNamesInFile(fileName, "tmp.txt") != 0) {
    return 1;
  }
  populateTree(&tree, "tmp.txt");
  remove("tmp.txt");

  printf("\n");
  printf("=========== RB TREE ==========\n");
  printf("\n");
  printNames(&tree, RB_ROOT(&tree), NULL, 0);
  printf("\n");
  printf("==============================\n\n");
  printf("\n");

  cJSON* jsonTree = serializeTreeToJson(&tree);
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

int mainMenu();

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

  struct RBTree tree;
  RB_INIT(&tree);

  sortNamesInFile("tmp.txt", "tmp2.txt");
  populateTree(&tree, "tmp2.txt");
  remove("tmp.txt");
  remove("tmp2.txt");

  int secondOption;
  do {
    printf("\n");
    printf("/*\n");
    printf("===========================================\n");
    printf("SECONDARY MENU\n");
    printf("===========================================\n");
    printf("*/\n");
    printf("Loaded %s successfully\n", fileName);
    printf("Choose operation with your file:\n");
    printf("1. Print\n");
    printf("2. Search name\n");
    printf("3. Delete name\n");
    printf("4. Back to main menu\n");
    printf("Operation: ");
    scanf("%d", &secondOption);

    switch (secondOption) {
      case 1:
        printNames(&tree, RB_ROOT(&tree), NULL, 0);
        break;
      case 2: {
        char searchValue[MAX_NAME_LENGTH];
        printf("Enter the first name you are looking for: ");
        scanf("%s", searchValue);
        searchByName(&tree, RB_ROOT(&tree), searchValue);
        break;
      }
      case 3:
        printf("Not implemented yet.\n");
        break;
      case 4:
        mainMenu();
        break;
      default:
        printf("Bad operation.\n");
        break;
    }
  } while (secondOption != 4);

  fclose(file);
  free(jsonBuffer);
  return 0;
}

/*
===========================================
MAIN MENU
===========================================
*/

int mainMenu() {
  int firstOption;
  do {
    printf("\n");
    printf("/*\n");
    printf("===========================================\n");
    printf("MAIN MENU\n");
    printf("===========================================\n");
    printf("*/\n");
    printf("Choose an option:\n");
    printf("1. Store list into RBTree JSON\n");
    printf("2. Load RBTree from JSON\n");
    printf("3. Exit\n");
    printf("Option: ");
    scanf("%d", &firstOption);

    switch (firstOption) {
      case 1:
        handleFirstOptionOne();
        break;
      case 2:
        handleFirstOptionTwo();
        break;
      case 3:
        printf("Exiting the program.\n");
        return 0;
      default:
        printf("Invalid option.\n");
        break;
    }
  } while (firstOption != 3);
}

/*
===========================================
MAIN
===========================================
*/

int main() {
  mainMenu();
  return 0;
}
