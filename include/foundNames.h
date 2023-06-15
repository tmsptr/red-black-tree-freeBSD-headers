#ifndef FOUND_NAMES
#define FOUND_NAMES

struct NameList;

void addName(struct NameList* list, const char* name);
void freeNames(struct NameList* list);
void printFoundNames(struct NameList* list);
void printNamesToFile(const char* fileName, struct NameList* list);

#endif