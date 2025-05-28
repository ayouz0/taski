#include "../include/tree_parser.h"
#include "../include/ui_core.h"
#include "../include/ui_message.h"
#include "../include/tree_extract.h"
#include "../include/fileio_task.h"
#include "../include/fileio_component.h"
#include "../include/component.h"
#include <ctype.h>

// Check if a task with similar title already exists in a component
int task_exists_in_component(const char* title, const char* component_name) {
    int comp_idx = find_component_by_name(component_name);
    if (comp_idx == -1) return 0;
    
    // Check all tasks in this component
    for (int i = 0; i < components[comp_idx].task_count; i++) {
        int task_id = components[comp_idx].task_ids[i];
        
        // Find the task with this ID
        for (int j = 0; j < taskCount; j++) {
            if (tasks[j].id == task_id) {
                // Compare titles (case insensitive)
                if (strcasecmp(tasks[j].title, title) == 0) {
                    return 1; // Task exists
                }
                break;
            }
        }
    }
    
    return 0; // Task doesn't exist
}

// Import tasks from a tree file
void import_tree() {
    clear();
    draw_header("Import Task Tree");
    
    echo();
    curs_set(1);
    
    // First ask if user wants to replace existing data
    attron(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
    mvprintw(3, 5, "Do you want to replace all existing tasks? (y/n): \n");
    attroff(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
    
    char replace_choice[2];
    mvgetnstr(3, 52, replace_choice, 1);
    
    bool replace_existing = (replace_choice[0] == 'y' || replace_choice[0] == 'Y');
    
    // Get filename
    attron(COLOR_PAIR(COLOR_DEFAULT));
    mvprintw(5, 5, "Enter tree file path: ");
    attroff(COLOR_PAIR(COLOR_DEFAULT));
    
    char filename[100];
    mvgetnstr(5, 27, filename, 99);
    
    noecho();
    curs_set(0);
    
    if (strlen(filename) == 0) {
        display_message("Import cancelled.", COLOR_INFO);
        getch();
        return;
    }
    
    // If replacing, clear all tasks and components
    if (replace_existing) {
        taskCount = 0;
        componentCount = 0;
        memset(tasks, 0, sizeof(tasks));
        memset(components, 0, sizeof(components));
    }
    
    int result = parse_tree_file(filename);
    
    if (result < 0) {
        char error_msg[100];
        sprintf(error_msg, "Error opening file: %s", filename);
        display_message(error_msg, COLOR_ERROR);
    } else if (result == 0) {
        display_message("No tasks imported (empty file or invalid format).", COLOR_INFO);
    } else {
        char success_msg[100];
        sprintf(success_msg, "Successfully imported %d tasks!", result);
        display_message(success_msg, COLOR_SUCCESS);
        
        // Save after import
        save_tasks();
        save_components();
    }
    
    getch();
}

// Parse a tree file and create tasks from it
int parse_tree_file(const char* filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return -1; // Error opening file
    }
    
    char line[DESC_LENGTH];
    char current_component[COMPONENT_NAME_LENGTH] = "";
    char *task_lines[MAX_TASKS];
    int task_count = 0;
    int tasks_imported = 0;
    
    // Initialize task_lines to NULL
    for (int i = 0; i < MAX_TASKS; i++) {
        task_lines[i] = NULL;
    }
    
    // Read file line by line
    while (fgets(line, sizeof(line), fp) && task_count < MAX_TASKS) {
        // Remove trailing newline
        int len = strlen(line);
        if (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[len-1] = '\0';
            
            // Also handle \r\n line endings
            if (len > 1 && line[len-2] == '\r') {
                line[len-2] = '\0';
            }
        }
        
        // Skip empty lines
        if (strlen(line) == 0) {
            continue;
        }
        
        // Check if this is a component header [COMPONENT]
        if (line[0] == '[' && strchr(line, ']')) {
            // If we have tasks from previous component, process them
            if (task_count > 0 && strlen(current_component) > 0) {
                int imported = create_tasks_from_tree(current_component, task_lines, task_count);
                tasks_imported += imported;
                
                // Free allocated task lines
                for (int i = 0; i < task_count; i++) {
                    if (task_lines[i] != NULL) {
                        free(task_lines[i]);
                        task_lines[i] = NULL;
                    }
                }
                task_count = 0;
            }
            
            // Extract component name
            char *end_bracket = strchr(line, ']');
            if (end_bracket == NULL) continue; // Malformed line
            
            int name_len = end_bracket - line - 1;
            if (name_len > 0 && name_len < COMPONENT_NAME_LENGTH - 1) {
                strncpy(current_component, line + 1, name_len);
                current_component[name_len] = '\0';
            } else {
                // Component name too long or empty
                current_component[0] = '\0';
            }
        } 
        // Otherwise it's a task description
        else if (strlen(current_component) > 0 && task_count < MAX_TASKS) {
            task_lines[task_count] = strdup(line);
            if (task_lines[task_count] == NULL) {
                // Memory allocation failed
                continue;
            }
            task_count++;
        }
    }
    
    // Process any remaining tasks in the last component
    if (task_count > 0 && strlen(current_component) > 0) {
        int imported = create_tasks_from_tree(current_component, task_lines, task_count);
        tasks_imported += imported;
        
        // Free allocated task lines
        for (int i = 0; i < task_count; i++) {
            if (task_lines[i] != NULL) {
                free(task_lines[i]);
            }
        }
    }
    
    fclose(fp);
    return tasks_imported;
}

// Create tasks from parsed tree lines
// Returns the number of new tasks added
int create_tasks_from_tree(const char* component, char* tasks_lines[], int count) {
    int tasks_added = 0;
    
    // Check if this component exists, add if not
    int component_idx = find_component_by_name(component);
    
    if (component_idx == -1 && componentCount < MAX_COMPONENTS) {
        // Create new component
        strncpy(components[componentCount].name, component, COMPONENT_NAME_LENGTH - 1);
        components[componentCount].name[COMPONENT_NAME_LENGTH - 1] = '\0';
        components[componentCount].task_count = 0;
        component_idx = componentCount++;
    }
    
    // If we couldn't create a component, return
    if (component_idx == -1) {
        return 0;
    }
    
    // Create tasks
    for (int i = 0; i < count && taskCount < MAX_TASKS; i++) {
        if (tasks_lines[i] == NULL) continue;
        
        // Create a copy of the task line for duplicate detection
        char task_check_title[TITLE_LENGTH];
        strncpy(task_check_title, tasks_lines[i], TITLE_LENGTH - 1);
        task_check_title[TITLE_LENGTH - 1] = '\0';
        
        // Check for priority tags and assignee tags which would affect the title
        char temp_description[DESC_LENGTH];
        strncpy(temp_description, task_check_title, DESC_LENGTH - 1);
        temp_description[DESC_LENGTH - 1] = '\0';
        
        // Extract priority (just to clean the title)
        extract_priority_from_description(temp_description, 2);
        
        // Extract assignee (just to clean the title)
        char temp_assignee[MAX_ASSIGNEE_LENGTH] = "";
        extract_assignee_from_description(temp_description, temp_assignee);
        
        // Now temp_description should have the clean title without tags
        
        // Check if a similar task already exists in this component
        if (task_exists_in_component(temp_description, component)) {
            // Skip this task as it's likely a duplicate
            continue;
        }
        
        // Initialize the new task with safe defaults
        Task newTask;
        memset(&newTask, 0, sizeof(Task)); // Zero out all fields
        
        newTask.id = taskCount + 1;
        newTask.created_at = time(NULL);
        newTask.updated_at = newTask.created_at;
        newTask.completed = 0;
        newTask.priority = 2; // Default: Medium
        
        // Set component (safely)
        strncpy(newTask.component, component, COMPONENT_NAME_LENGTH - 1);
        newTask.component[COMPONENT_NAME_LENGTH - 1] = '\0';
        
        // Create a copy of the task line to avoid modifying the original
        char task_description[DESC_LENGTH];
        strncpy(task_description, tasks_lines[i], DESC_LENGTH - 1);
        task_description[DESC_LENGTH - 1] = '\0';
        
        // Extract and remove priority from description (#high, #medium, #low)
        newTask.priority = extract_priority_from_description(task_description, 2);
        
        // Extract and remove assignee from description (@username)
        char assignee[MAX_ASSIGNEE_LENGTH] = "";
        extract_assignee_from_description(task_description, assignee);
        strncpy(newTask.assignee, assignee, MAX_ASSIGNEE_LENGTH - 1);
        newTask.assignee[MAX_ASSIGNEE_LENGTH - 1] = '\0';
        
        // Use the whole line as title, ensuring it's not too long
        strncpy(newTask.title, task_description, TITLE_LENGTH - 1);
        newTask.title[TITLE_LENGTH - 1] = '\0';
        
        // Set empty description
        newTask.description[0] = '\0';
        
        // Add task to global array
        tasks[taskCount] = newTask;
        
        // Add task reference to component
        if (component_idx >= 0 && component_idx < MAX_COMPONENTS && 
            components[component_idx].task_count < MAX_TASKS) {
            components[component_idx].task_ids[components[component_idx].task_count++] = newTask.id;
        }
        
        // Increment task count
        taskCount++;
        tasks_added++;
    }
    
    return tasks_added;
}