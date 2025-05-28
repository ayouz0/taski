#ifndef COMPONENT_H
#define COMPONENT_H

#include "task.h"

// Component functions
void view_components();
int find_component_by_name(const char* name);
void add_task_to_component(int task_id, const char* component_name);
void remove_task_from_component(int task_id, const char* component_name);

#endif /* COMPONENT_H */