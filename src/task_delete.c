#include "../include/task_delete.h"
#include "../include/ui_core.h"
#include "../include/ui_message.h"
#include "../include/component.h"
#include "../include/fileio_task.h"
#include "../include/fileio_component.h"

// Delete an existing task
void delete_task() {
    clear();
    draw_header("Delete Task");
    
    if (taskCount == 0) {
        display_message("No tasks available to delete", COLOR_INFO);
        getch();
        return;
    }
    
    // Show available tasks
    attron(COLOR_PAIR(COLOR_DEFAULT));
    mvprintw(3, 3, "Select a task to delete:");
    attroff(COLOR_PAIR(COLOR_DEFAULT));
    
    int displayed_tasks = taskCount < 10 ? taskCount : 10;
    
    for (int i = 0; i < displayed_tasks; i++) {
        int priority_color = get_priority_color(tasks[i].priority);
        
        attron(COLOR_PAIR(COLOR_DEFAULT));
        mvprintw(5 + i, 5, "%d. %s", tasks[i].id, tasks[i].title);
        attroff(COLOR_PAIR(COLOR_DEFAULT));
        
        attron(COLOR_PAIR(priority_color));
        mvprintw(5 + i, 40, "[%s]", get_priority_string(tasks[i].priority));
        attroff(COLOR_PAIR(priority_color));
        
        if (strlen(tasks[i].component) > 0) {
            attron(COLOR_PAIR(COLOR_COMPONENT));
            mvprintw(5 + i, 55, "(%s)", tasks[i].component);
            attroff(COLOR_PAIR(COLOR_COMPONENT));
        }
    }
    
    if (taskCount > 10) {
        attron(COLOR_PAIR(COLOR_INFO));
        mvprintw(5 + displayed_tasks, 5, "...and %d more tasks", taskCount - displayed_tasks);
        attroff(COLOR_PAIR(COLOR_INFO));
    }
    
    // Get task ID to delete
    attron(COLOR_PAIR(COLOR_DEFAULT));
    mvprintw(16, 3, "Enter task ID to delete: ");
    attroff(COLOR_PAIR(COLOR_DEFAULT));
    
    echo();
    curs_set(1);
    
    char id_str[4];
    mvgetnstr(16, 28, id_str, 3);
    
    int task_id = atoi(id_str);
    
    if (task_id < 1 || task_id > taskCount) {
        noecho();
        curs_set(0);
        display_message("Invalid task ID!", COLOR_ERROR);
        getch();
        return;
    }
    
    noecho();
    curs_set(0);
    
    // Confirm deletion
    char confirm_msg[100];
    sprintf(confirm_msg, "Delete task \"%s\"? (y/n)", tasks[task_id - 1].title);
    
    attron(COLOR_PAIR(COLOR_ERROR) | A_BOLD);
    mvprintw(18, 3, "%s", confirm_msg);
    attroff(COLOR_PAIR(COLOR_ERROR) | A_BOLD);
    
    char key = getch();
    
    if (key != 'y' && key != 'Y') {
        display_message("Deletion cancelled", COLOR_INFO);
        getch();
        return;
    }
    
    // Remove task from its component if it belongs to one
    if (strlen(tasks[task_id - 1].component) > 0) {
        remove_task_from_component(task_id, tasks[task_id - 1].component);
    }
    
    // Delete the task by shifting all tasks after it
    for (int i = task_id - 1; i < taskCount - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    
    taskCount--;
    
    // Update IDs
    update_task_ids();
    
    // Update component task IDs
    for (int i = 0; i < componentCount; i++) {
        for (int j = 0; j < components[i].task_count; j++) {
            if (components[i].task_ids[j] > task_id) {
                components[i].task_ids[j]--;
            }
        }
    }
    
    // Save tasks and components
    save_tasks();
    save_components();
    
    // Show success message
    display_message("Task deleted successfully!", COLOR_SUCCESS);
    getch();
}