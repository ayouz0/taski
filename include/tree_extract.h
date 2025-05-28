#ifndef TREE_EXTRACT_H
#define TREE_EXTRACT_H

#include "task.h"

// Tree data extraction functions
int extract_priority_from_description(char* description, int default_priority);
char* extract_assignee_from_description(char* description, char* assignee_buffer);

#endif /* TREE_EXTRACT_H */