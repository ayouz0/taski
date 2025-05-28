#include "../include/fileio_task.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

void load_tasks() {
    char filepath[256];
    char* home = getenv("HOME");
    
    if (home == NULL) {
        // Fallback if HOME environment variable isn't available
        snprintf(filepath, sizeof(filepath), ".tasks.txt");
    } else {
        snprintf(filepath, sizeof(filepath), "%s/.tasky/tasks.txt", home);
    }
    
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        return; // File doesn't exist yet
    }
    
    taskCount = 0;
    char line[TITLE_LENGTH + DESC_LENGTH + 50 + COMPONENT_NAME_LENGTH + MAX_ASSIGNEE_LENGTH]; // Buffer for reading lines
    
    // Debug output to verify we're reading the right file
    printf("Loading tasks from: %s\n", filepath);
    
    while (fgets(line, sizeof(line), fp) && taskCount < MAX_TASKS) {
        Task task;
        
        // Initialize strings to empty to handle missing fields
        task.title[0] = '\0';
        task.description[0] = '\0';
        task.component[0] = '\0';
        task.assignee[0] = '\0';
        
        // Remove newline character if present
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        
        // Create a copy of the line to work with
        char lineCopy[TITLE_LENGTH + DESC_LENGTH + 50 + COMPONENT_NAME_LENGTH + MAX_ASSIGNEE_LENGTH];
        strcpy(lineCopy, line);
        
        // Parse using strtok for better handling of empty fields
        char *token;
        char *saveptr;
        
        // Extract ID
        token = strtok_r(lineCopy, "|", &saveptr);
        if (token == NULL) continue;
        task.id = atoi(token);
        
        // Extract Title
        token = strtok_r(NULL, "|", &saveptr);
        if (token != NULL) {
            strncpy(task.title, token, TITLE_LENGTH-1);
            task.title[TITLE_LENGTH-1] = '\0';
        }
        
        // Extract Description
        token = strtok_r(NULL, "|", &saveptr);
        if (token != NULL) {
            strncpy(task.description, token, DESC_LENGTH-1);
            task.description[DESC_LENGTH-1] = '\0';
        }
        
        // Extract Completed
        token = strtok_r(NULL, "|", &saveptr);
        if (token != NULL) {
            task.completed = atoi(token);
        } else {
            task.completed = 0;
        }
        
        // Extract Priority
        token = strtok_r(NULL, "|", &saveptr);
        if (token != NULL) {
            task.priority = atoi(token);
        } else {
            task.priority = 1; // Default to medium
        }
        
        // Extract Component
        token = strtok_r(NULL, "|", &saveptr);
        if (token != NULL) {
            strncpy(task.component, token, COMPONENT_NAME_LENGTH-1);
            task.component[COMPONENT_NAME_LENGTH-1] = '\0';
        }
        
        // Extract Assignee
        token = strtok_r(NULL, "|", &saveptr);
        if (token != NULL) {
            strncpy(task.assignee, token, MAX_ASSIGNEE_LENGTH-1);
            task.assignee[MAX_ASSIGNEE_LENGTH-1] = '\0';
        }
        
        // Extract Created At
        token = strtok_r(NULL, "|", &saveptr);
        if (token != NULL) {
            task.created_at = atol(token);
        } else {
            task.created_at = time(NULL);
        }
        
        // Extract Updated At
        token = strtok_r(NULL, "|", &saveptr);
        if (token != NULL) {
            task.updated_at = atol(token);
        } else {
            task.updated_at = task.created_at;
        }
        
        // Add task to array
        tasks[taskCount] = task;
        taskCount++;
        
        // Debug output to verify task loading
        printf("Loaded task: %d, %s\n", task.id, task.title);
    }
    
    printf("Loaded %d tasks\n", taskCount);
    fclose(fp);
}

// Save tasks to file
void save_tasks() {
    char filepath[256];
    char* home = getenv("HOME");
    
    if (home == NULL) {
        // Fallback if HOME environment variable isn't available
        snprintf(filepath, sizeof(filepath), ".tasks.txt");
    } else {
        snprintf(filepath, sizeof(filepath), "%s/.tasky/tasks.txt", home);
    }
    
    // Create .tasky directory if it doesn't exist
    if (home != NULL) {
        char dirname[256];
        snprintf(dirname, sizeof(dirname), "%s/.tasky", home);
        
        // This is a simple way to create a directory if it doesn't exist
        // mkdir will fail silently if the directory already exists
        char mkdir_cmd[300];
        snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s", dirname);
        system(mkdir_cmd);
    }
    
    FILE *fp = fopen(filepath, "w");
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

