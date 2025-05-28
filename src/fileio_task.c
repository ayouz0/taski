#include "../include/fileio_task.h"

// Load tasks from file
void load_tasks() {
    FILE *fp = fopen("tasks.txt", "r");
    if (fp == NULL) {
        return; // File doesn't exist yet
    }
    
    taskCount = 0;
    char line[TITLE_LENGTH + DESC_LENGTH + 50 + COMPONENT_NAME_LENGTH + MAX_ASSIGNEE_LENGTH]; // Buffer for reading lines
    
    while (fgets(line, sizeof(line), fp) && taskCount < MAX_TASKS) {
        Task task;
        char title[TITLE_LENGTH];
        char description[DESC_LENGTH];
        char component[COMPONENT_NAME_LENGTH];
        char assignee[MAX_ASSIGNEE_LENGTH];
        
        // Parse line with component and assignee fields
        if (sscanf(line, "%d|%[^|]|%[^|]|%d|%d|%[^|]|%[^|]|%ld|%ld", 
                  &task.id, title, description, &task.completed, 
                  &task.priority, component, assignee,
                  &task.created_at, &task.updated_at) == 9) {
            
            // Copy strings to task
            strncpy(task.title, title, TITLE_LENGTH-1);
            task.title[TITLE_LENGTH-1] = '\0';
            
            strncpy(task.description, description, DESC_LENGTH-1);
            task.description[DESC_LENGTH-1] = '\0';
            
            strncpy(task.component, component, COMPONENT_NAME_LENGTH-1);
            task.component[COMPONENT_NAME_LENGTH-1] = '\0';
            
            strncpy(task.assignee, assignee, MAX_ASSIGNEE_LENGTH-1);
            task.assignee[MAX_ASSIGNEE_LENGTH-1] = '\0';
            
            tasks[taskCount] = task;
            taskCount++;
        }
    }
    
    fclose(fp);
}

// Save tasks to file
void save_tasks() {
    FILE *fp = fopen("tasks.txt", "w");
    if (fp == NULL) {
        return; // Can't open file
    }
    
    for (int i = 0; i < taskCount; i++) {
        fprintf(fp, "%d|%s|%s|%d|%d|%s|%s|%ld|%ld\n", 
                tasks[i].id,
                tasks[i].title,
                tasks[i].description,
                tasks[i].completed,
                tasks[i].priority,
                tasks[i].component,
                tasks[i].assignee,
                tasks[i].created_at,
                tasks[i].updated_at);
    }
    
    fclose(fp);
}