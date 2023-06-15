#ifndef SERIALIZE_TREE
#define SERIALIZE_TREE

#include "cJSON.h"

struct RBTree;

cJSON* serializeTreeToJson(struct RBTree* tree);

#endif