#include "../include/task_delete.h"
#include "../include/ui_core.h"
#include "../include/ui_message.h"
#include "../include/component.h"
#include "../include/fileio_task.h"
#include "../include/fileio_component.h"
#include <string.h>

// Delete an existing task
void delete_task() {
    int current_page = 0;
    int tasks_per_page = 8;
    int max_pages = (taskCount + tasks_per_page - 1) / tasks_per_page; // Ceiling division
    int selected_index = 0;
    int key;
    bool exit_selection = false;
    
    // Safety check
    if (max_pages < 1) max_pages = 1;
    
    // Make sure taskCount is valid
    if (taskCount <= 0) {
        display_message("No tasks available to delete", COLOR_INFO);
        getch();
        return;
    }
    
    // Selection loop
    while (!exit_selection) {
        clear();
        draw_header("Delete Task");
        
        // Calculate page boundaries safely
        int start_idx = current_page * tasks_per_page;
        int end_idx = start_idx + tasks_per_page;
        
        // Safety check for boundaries
        if (start_idx >= taskCount) {
            current_page = 0;
            start_idx = 0;
        }
        
        if (end_idx > taskCount) {
            end_idx = taskCount;
        }
        
        // Show instructions
        mvprintw(3, 3, "Select a task to delete:");
        
        // Display column headers
        mvprintw(5, 5, "ID");
        mvprintw(5, 12, "Task");
        mvprintw(5, 40, "Priority");
        mvprintw(5, 55, "Component");
        
        // Display tasks for current page
        for (int i = start_idx; i < end_idx; i++) {
            int row = 7 + (i - start_idx);
            
            // Highlight selected task
            if (i == start_idx + selected_index) {
                attron(A_REVERSE);
            }
            
            // Display ID
            mvprintw(row, 5, "%d", tasks[i].id);
            
            // Display title (safely truncated)
            char title[28] = {0};
            strncpy(title, tasks[i].title, 27);
            mvprintw(row, 12, "%s", title);
            
            // Display priority
            mvprintw(row, 40, "[%s]", get_priority_string(tasks[i].priority));
            
            // Display component if exists
            if (strlen(tasks[i].component) > 0) {
                mvprintw(row, 55, "%s", tasks[i].component);
            }
            
            // End highlight
            if (i == start_idx + selected_index) {
                attroff(A_REVERSE);
            }
        }
        
        // Display pagination info
        mvprintw(17, 3, "Page %d of %d (%d tasks)", 
                current_page + 1, max_pages, taskCount);
        
        // Display navigation help
        mvprintw(19, 3, "Up/Down: Navigate  |  Left/Right: Change page  |  Enter: Select  |  Esc: Cancel");
        
        // Get user input
        key = getch();
        
        // Handle keyboard input
        if (key == KEY_UP) {
            if (selected_index > 0) {
                selected_index--;
            }
        }
        else if (key == KEY_DOWN) {
            if (selected_index < tasks_per_page - 1 && 
                start_idx + selected_index + 1 < taskCount) {
                selected_index++;
            }
        }
        else if (key == KEY_LEFT) {
            if (current_page > 0) {
                current_page--;
                selected_index = 0;  // Reset selection on page change for simplicity
            }
        }
        else if (key == KEY_RIGHT) {
            if ((current_page + 1) * tasks_per_page < taskCount) {
                current_page++;
                selected_index = 0;  // Reset selection on page change for simplicity
            }
        }
        else if (key == '\n' || key == KEY_ENTER || key == 10 || key == 13) {
            exit_selection = true;
        }
        else if (key == 27) {  // Escape
            return;
        }
    }
    
    // Safety check
    int task_index = current_page * tasks_per_page + selected_index;
    if (task_index < 0 || task_index >= taskCount) {
        display_message("Invalid task selection", COLOR_ERROR);
        getch();
        return;
    }
    
    int task_id = tasks[task_index].id;
    
    // Confirm deletion
    clear();
    mvprintw(3, 3, "Are you sure you want to delete this task? (y/n)");
    mvprintw(5, 5, "Task: %s", tasks[task_index].title);
    
    char confirm = getch();
    
    if (confirm != 'y' && confirm != 'Y') {
        display_message("Deletion cancelled", COLOR_INFO);
        getch();
        return;
    }
    
    // Remove task from component if needed
    if (strlen(tasks[task_index].component) > 0) {
        remove_task_from_component(task_id, tasks[task_index].component);
    }
    
    // Delete task by shifting (simpler implementation)
    for (int i = task_index; i < taskCount - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    
    taskCount--;
    
    // Update IDs
    update_task_ids();
    
    // Update component references
    for (int i = 0; i < componentCount; i++) {
        for (int j = 0; j < components[i].task_count; j++) {
            if (components[i].task_ids[j] > task_id) {
                components[i].task_ids[j]--;
            }
        }
    }
    
    // Save data
    save_tasks();
    save_components();
    
    // Success message
    display_message("Task deleted successfully", COLOR_SUCCESS);
    getch();
}