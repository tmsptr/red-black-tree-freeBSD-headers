#include "../include/serializeToJSON.h"

#include <stdio.h>
#include <stdlib.h>

#include "../include/structures.h"

#define MAX_NAME_LENGTH 256

RB_HEAD(RBTree, TreeNode); /* Root */
RB_PROTOTYPE(RBTree, TreeNode, entry,
             compareNodes) /* Generate function prototypes */

cJSON* serializeTreeToJson(struct RBTree* tree) {
  cJSON* jsonTree = cJSON_CreateArray();
  struct TreeNode* node;
  struct TreeNode* nextNode;

  RB_FOREACH_SAFE(node, RBTree, tree, nextNode) {
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

    if (node == RB_ROOT(tree)) {
      cJSON_AddStringToObject(jsonNode, "root", "true");
    }
  }

  return jsonTree;
}