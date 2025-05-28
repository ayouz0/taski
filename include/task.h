#ifndef TASK_H
#define TASK_H

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Define constants
#define MAX_TASKS 100
#define TITLE_LENGTH 50
#define DESC_LENGTH 200
#define NUM_OPTIONS 8  // Added one more option for tree import
#define MAX_COMPONENTS 10
#define COMPONENT_NAME_LENGTH 30
#define MAX_ASSIGNEE_LENGTH 20

// Color pair definitions
#define COLOR_DEFAULT    1
#define COLOR_HEADER     2
#define COLOR_HIGHLIGHT  3
#define COLOR_LOW        4
#define COLOR_MEDIUM     5
#define COLOR_HIGH       6
#define COLOR_SUCCESS    7
#define COLOR_ERROR      8
#define COLOR_INFO       9
#define COLOR_COMPONENT  10

// Task structure
typedef struct {
    int id;
    char title[TITLE_LENGTH];
    char description[DESC_LENGTH];
    int completed;  // Using int instead of bool for simplicity
    int priority;   // 1: Low, 2: Medium, 3: High
    char component[COMPONENT_NAME_LENGTH]; // Component this task belongs to
    char assignee[MAX_ASSIGNEE_LENGTH];    // Person assigned to the task
    time_t created_at;
    time_t updated_at;
} Task;

extern Task tasks[MAX_TASKS];
// Component structure to organize tasks
typedef struct {
    char name[COMPONENT_NAME_LENGTH];
    int task_count;
    int task_ids[MAX_TASKS]; // Store task IDs that belong to this component
} Component;

// Global variables declaration
extern Task tasks[MAX_TASKS];
extern int taskCount;
extern int current_theme;
extern Component components[MAX_COMPONENTS];
extern int componentCount;
extern const char *options[];

// Core task functions
void initialize_colors();
int get_priority_color(int priority);
const char* get_priority_string(int priority);
void update_task_ids();
void toggle_theme();

#endif /* TASK_H */