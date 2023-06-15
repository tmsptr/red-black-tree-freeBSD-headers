#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "../include/queue.h"
#include "../include/tree.h"

struct TreeNode {
  RB_ENTRY(TreeNode) entry;
  unsigned int key;
  char** fullName;
  int fullNameLength;
  TAILQ_HEAD(DuplicateQueue, DuplicateNode) duplicates;
};

struct DuplicateNode {
  char** fullName;
  int fullNameLength;
  TAILQ_ENTRY(DuplicateNode) entries;
};

struct NameList {
  char** names;
  int count;
};

#endif
