#ifndef SEARCH_BY_NAME
#define SEARCH_BY_NAME

struct RBTree;

struct TreeNode* searchByName(struct RBTree* tree, struct TreeNode* currentNode,
                              const char* name);

#endif