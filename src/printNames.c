#include "../include/printNames.h"

#include <stdio.h>

#include "../include/queue.h"
#include "../include/structures.h"
#include "../include/tree.h"

void printNames(struct RBTree* tree, struct TreeNode* currentNode,
                struct TreeNode* parent, int level) {
  if (currentNode == NULL) {
    return;
  }

  if (parent == NULL) {
    printf("Root - ");
  } else {
    int compareResult = (currentNode->key < parent->key) ? -1 : 1;
    const char* childDirection = (compareResult < 0) ? "Left" : "Right";

    printf("%-5s child of %-10s %-14s   Level: %d ", childDirection,
           parent->fullName[0], parent->fullName[1], level);
  }

  printf("Color: %-5s ",
         (RB_COLOR(currentNode, entry) == RB_BLACK) ? "Black" : "Red");
  printf("Hash: %-10u --- ", currentNode->key);
  for (int i = 0; i < currentNode->fullNameLength; i++) {
    printf("%s ", currentNode->fullName[i]);
  }
  printf("\n");

  /* Print from queue */
  struct DuplicateNode* duplicate;
  TAILQ_FOREACH(duplicate, &(currentNode->duplicates), entries) {
    int rootSkipChars = 7;
    int skipChars = 52;
    if (parent == NULL) {
      printf("%*sQueue member ", rootSkipChars, "");
    } else {
      printf("%*sQueue member ", skipChars, "");
    }
    printf("Hash: %-10u --- ", currentNode->key);
    for (int i = 0; i < duplicate->fullNameLength; i++) {
      printf("%s ", duplicate->fullName[i]);
    }
    printf("\n");
  }

  printNames(tree, RB_LEFT(currentNode, entry), currentNode, level + 1);
  printNames(tree, RB_RIGHT(currentNode, entry), currentNode, level + 1);
}