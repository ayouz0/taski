#ifndef TREE_PARSER_H
#define TREE_PARSER_H

#include "task.h"

// Tree parsing functions
void import_tree();
int parse_tree_file(const char* filename);
int create_tasks_from_tree(const char* component, char* tasks[], int count);

#endif /* TREE_PARSER_H */