#include "../include/fileio_component.h"

// Load components from file
void load_components() {
    FILE *fp = fopen("components.txt", "r");
    if (fp == NULL) {
        return; // File doesn't exist yet
    }
    
    componentCount = 0;
    char line[COMPONENT_NAME_LENGTH + MAX_TASKS * 10]; // Buffer for reading lines
    
    while (fgets(line, sizeof(line), fp) && componentCount < MAX_COMPONENTS) {
        Component component;
        char name[COMPONENT_NAME_LENGTH];
        
        // Initialize
        component.task_count = 0;
        
        // Find the name part
        char *token = strtok(line, "|");
        if (token != NULL) {
            strncpy(name, token, COMPONENT_NAME_LENGTH-1);
            name[COMPONENT_NAME_LENGTH-1] = '\0';
            
            // Store name
            strcpy(component.name, name);
            
            // Read task IDs
            token = strtok(NULL, "|");
            while (token != NULL && component.task_count < MAX_TASKS) {
                component.task_ids[component.task_count++] = atoi(token);
                token = strtok(NULL, "|");
            }
            
            components[componentCount++] = component;
        }
    }
    
    fclose(fp);
}

// Save components to file
void save_components() {
    FILE *fp = fopen("components.txt", "w");
    if (fp == NULL) {
        return; // Can't open file
    }
    
    for (int i = 0; i < componentCount; i++) {
        fprintf(fp, "%s", components[i].name);
        
        for (int j = 0; j < components[i].task_count; j++) {
            fprintf(fp, "|%d", components[i].task_ids[j]);
        }
        
        fprintf(fp, "\n");
    }
    
    fclose(fp);
}