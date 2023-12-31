#include "../include/populateTree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/computeHash.h"
#include "../include/structures.h"

#define MAX_NAME_LENGTH 42

RB_PROTOTYPE(RBTree, TreeNode, entry,
             compareNodes) /* Generate function prototypes */

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
    while (token != NULL && fullNameLength < MAX_NAME_LENGTH) {
      fullName[fullNameLength] = strdup(token);
      fullNameLength++;
      token = strtok(NULL, " \t\n");
    }

    /* Calculate hash */
    unsigned int hash = computeHash(fullName[0]);

    /* Create new tree node */
    struct TreeNode* node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    node->key = hash;
    node->fullName = (char**)malloc(sizeof(char*) * (fullNameLength));
    for (int i = 0; i < fullNameLength; i++) {
      node->fullName[i] = strdup(fullName[i]);
    }
    // node->fullName[fullNameLength] = NULL; /* NULL terminate */
    node->fullNameLength = fullNameLength;
    TAILQ_INIT(&(node->duplicates));

    /* Insert node into tree */
    struct TreeNode* existingNode = RB_INSERT(RBTree, tree, node);

    if (existingNode != NULL) {
      /* Add to the queue */
      struct DuplicateNode* duplicate =
          (struct DuplicateNode*)malloc(sizeof(struct DuplicateNode));
      duplicate->fullName = (char**)malloc(sizeof(char*) * (fullNameLength));
      for (int i = 0; i < fullNameLength; i++) {
        duplicate->fullName[i] = strdup(fullName[i]);
      }
      // duplicate->fullName[fullNameLength] = NULL; /* NULL terminate */
      duplicate->fullNameLength = fullNameLength;
      TAILQ_INSERT_TAIL(&(existingNode->duplicates), duplicate, entries);
    }
    free(fullName);
  }
  fclose(file);
}
