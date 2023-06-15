#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/structures.h"
#include "../include/xxhash.h"

unsigned int computeHash(const char* str) {
  XXH32_state_t* state = XXH32_createState();
  XXH32_reset(state, 0);
  XXH32_update(state, str, strlen(str));
  unsigned int hash = XXH32_digest(state);
  XXH32_freeState(state);
  return hash;
}