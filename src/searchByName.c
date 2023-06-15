#include "../include/searchByName.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/computeHash.h"
#include "../include/foundNames.h"
#include "../include/queue.h"
#include "../include/structures.h"
#include "../include/tree.h"
#include "../include/xxhash.h"

#define MAX_NAME_LENGTH 256

struct TreeNode* searchByName(struct RBTree* tree, struct TreeNode* currentNode,
                              const char* name) {
  if (currentNode == NULL) {
    printf("The RB tree is empty.\n");
    return NULL;
  }

  char modifiedName[MAX_NAME_LENGTH];
  int i = 0;
  while (name[i] != '\0') {
    if (i == 0) {
      modifiedName[i] = toupper(name[i]);
    } else {
      modifiedName[i] = tolower(name[i]);
    }
    i++;
  }
  modifiedName[i] = '\0';

  unsigned int hash = computeHash(modifiedName);
  struct NameList foundNames;
  foundNames.names = NULL;
  foundNames.count = 0;

  while (currentNode != NULL) {
    int cmpResult = strcasecmp(name, *(currentNode->fullName));
    if (hash < currentNode->key) {
      currentNode = currentNode->entry.rbe_left;
    } else if (hash > currentNode->key) {
      currentNode = currentNode->entry.rbe_right;
    } else if (cmpResult != 0) {
      currentNode = currentNode->entry.rbe_right;
    } else {
      printf("Found with name '%s'.\n", name);
      char** names = currentNode->fullName;
      char nameString[MAX_NAME_LENGTH] = "";
      int i = 0;
      while (i < currentNode->fullNameLength) {
        // printf("%s ", names[i]);
        strcat(nameString, names[i]);
        strcat(nameString, " ");
        i++;
      }
      addName(&foundNames, nameString);
      memset(nameString, 0, sizeof(nameString));

      if (!TAILQ_EMPTY(&(currentNode->duplicates))) {
        struct DuplicateNode* node;
        TAILQ_FOREACH(node, &(currentNode->duplicates), entries) {
          char** names = node->fullName;
          int i = 0;
          while (i < node->fullNameLength) {
            // printf("%s ", names[i]);
            strcat(nameString, names[i]);
            strcat(nameString, " ");
            i++;
          }
          addName(&foundNames, nameString);
          memset(nameString, 0, sizeof(nameString));

          printf("\n");
        }
      }
      printFoundNames(&foundNames);

      return currentNode;
    }
  }

  printf("'%s' is not found.\n", name);
  return NULL;
}