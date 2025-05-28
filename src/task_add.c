#include "../include/task_add.h"
#include "../include/ui_core.h"
#include "../include/ui_message.h"
#include "../include/component.h"
#include "../include/fileio_task.h"
#include "../include/fileio_component.h"

// Add a new task
void add_task() {
    clear();
    draw_header("Add New Task");
    
    if (taskCount >= MAX_TASKS) {
        display_message("Task limit reached!", COLOR_ERROR);
        getch();
        return;
    }
    
    Task newTask;
    newTask.id = taskCount + 1;
    newTask.created_at = time(NULL);
    newTask.updated_at = newTask.created_at;
    newTask.completed = 0;
    newTask.priority = 2; // Default: Medium
    strcpy(newTask.component, "");  // Default: no component
    strcpy(newTask.assignee, "");   // Default: unassigned
    
    // Get task title
    echo();
    curs_set(1);
    
    attron(COLOR_PAIR(COLOR_DEFAULT));
    mvprintw(4, 5, "Title: ");
    attroff(COLOR_PAIR(COLOR_DEFAULT));
    char title[TITLE_LENGTH];
    mvgetnstr(4, 12, title, TITLE_LENGTH - 1);
    strncpy(newTask.title, title, TITLE_LENGTH - 1);
    newTask.title[TITLE_LENGTH - 1] = '\0';
    
    // Get task description
    attron(COLOR_PAIR(COLOR_DEFAULT));
    mvprintw(6, 5, "Description: ");
    attroff(COLOR_PAIR(COLOR_DEFAULT));
    
    char description[DESC_LENGTH];
    mvgetnstr(6, 18, description, DESC_LENGTH - 1);
    strncpy(newTask.description, description, DESC_LENGTH - 1);
    newTask.description[DESC_LENGTH - 1] = '\0';
    
    // Get task priority
    attron(COLOR_PAIR(COLOR_DEFAULT));
    mvprintw(8, 5, "Priority (1-Low, 2-Medium, 3-High): ");
    attroff(COLOR_PAIR(COLOR_DEFAULT));
    
    char priority_str[2];
    mvgetnstr(8, 42, priority_str, 1);
    
    int priority = atoi(priority_str);
    if (priority >= 1 && priority <= 3) {
        newTask.priority = priority;
    }
    
    // Get component (if any)
    attron(COLOR_PAIR(COLOR_DEFAULT));
    mvprintw(10, 5, "Component (optional): ");
    attroff(COLOR_PAIR(COLOR_DEFAULT));
    
    char component[COMPONENT_NAME_LENGTH];
    mvgetnstr(10, 26, component, COMPONENT_NAME_LENGTH - 1);
    
    if (strlen(component) > 0) {
        strncpy(newTask.component, component, COMPONENT_NAME_LENGTH - 1);
        newTask.component[COMPONENT_NAME_LENGTH - 1] = '\0';
        
        // Add task to component (this will create the component if needed)
        add_task_to_component(newTask.id, component);
    }
    
    // Get assignee (if any)
    attron(COLOR_PAIR(COLOR_DEFAULT));
    mvprintw(12, 5, "Assignee (optional): ");
    attroff(COLOR_PAIR(COLOR_DEFAULT));
    
    char assignee[MAX_ASSIGNEE_LENGTH];
    mvgetnstr(12, 26, assignee, MAX_ASSIGNEE_LENGTH - 1);
    
    if (strlen(assignee) > 0) {
        strncpy(newTask.assignee, assignee, MAX_ASSIGNEE_LENGTH - 1);
        newTask.assignee[MAX_ASSIGNEE_LENGTH - 1] = '\0';
    }
    
    noecho();
    curs_set(0);
    
    // Add task to array
    tasks[taskCount++] = newTask;
    
    // Save all tasks and components
    save_tasks();
    save_components();
    
    // Show success message
    display_message("Task added successfully!", COLOR_SUCCESS);
    getch();
}