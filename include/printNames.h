#ifndef PRINT_NAMES
#define PRINT_NAMES

struct RBTree;
struct TreeNode;

void printNames(struct RBTree* tree, struct TreeNode* currentNode,
                struct TreeNode* parent, int level);

#endif