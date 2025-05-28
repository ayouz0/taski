#include "../include/task_edit.h"
#include "../include/ui_core.h"
#include "../include/ui_message.h"
#include "../include/component.h"
#include "../include/fileio_task.h"
#include "../include/fileio_component.h"

// Edit an existing task
void edit_task() {
    clear();
    draw_header("Edit Task");
    
    if (taskCount == 0) {
        display_message("No tasks available to edit", COLOR_INFO);
        getch();
        return;
    }
    
    int page = 0;
    int ch;
    int selected_task = 0;
    const int TASKS_PER_PAGE = 12;
    int total_pages = (taskCount + TASKS_PER_PAGE - 1) / TASKS_PER_PAGE;
    bool task_selected = false;
    
    // Task selection screen
    while (!task_selected) {
        clear();
        draw_header("Edit Task - Select Task");
        
        // Calculate screen dimensions
        int max_x, max_y;
        getmaxyx(stdscr, max_y, max_x);
        
        // Draw table header with bold and colored text
        attron(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
        mvprintw(3, 3, "ID");
        mvprintw(3, 8, "TITLE");
        mvprintw(3, max_x - 55, "COMPONENT");
        mvprintw(3, max_x - 35, "PRIORITY");
        mvprintw(3, max_x - 25, "STATUS");
        attroff(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
        
        // Draw separator
        attron(COLOR_PAIR(COLOR_DEFAULT));
        for (int i = 3; i < max_x - 3; i++) {
            mvaddch(4, i, '-');
        }
        attroff(COLOR_PAIR(COLOR_DEFAULT));
        
        // Calculate start and end indices for current page
        int start_idx = page * TASKS_PER_PAGE;
        int end_idx = start_idx + TASKS_PER_PAGE;
        if (end_idx > taskCount) {
            end_idx = taskCount;
        }
        
        // Draw tasks for current page
        for (int i = start_idx; i < end_idx; i++) {
            int row = 5 + (i - start_idx);
            bool is_selected = (i == selected_task);
            
            // ID with highlighting if selected
            if (is_selected) {
                attron(COLOR_PAIR(COLOR_HIGHLIGHT) | A_BOLD);
                mvprintw(row, 1, ">");
            } else {
                attron(COLOR_PAIR(COLOR_DEFAULT));
            }
            
            mvprintw(row, 3, "%d", tasks[i].id);
            
            // Calculate available width for title
            int title_max_width = max_x - 70;
            if (title_max_width < 20) title_max_width = 20; // Minimum width
            
            // Title (truncate if needed)
            char title_display[256]; // Large buffer
            if (strlen(tasks[i].title) > (size_t)title_max_width) {
                strncpy(title_display, tasks[i].title, title_max_width - 3);
                title_display[title_max_width - 3] = '.';
                title_display[title_max_width - 2] = '.';
                title_display[title_max_width - 1] = '.';
                title_display[title_max_width] = '\0';
            } else {
                strncpy(title_display, tasks[i].title, sizeof(title_display) - 1);
                title_display[sizeof(title_display) - 1] = '\0';
            }
            
            mvprintw(row, 8, "%s", title_display);
            
            // Component
            if (strlen(tasks[i].component) > 0) {
                // Component (truncate if needed)
                char component_display[18]; // 17 chars + null terminator
                if (strlen(tasks[i].component) > 15) {
                    strncpy(component_display, tasks[i].component, 14);
                    component_display[14] = '.';
                    component_display[15] = '.';
                    component_display[16] = '\0';
                } else {
                    strncpy(component_display, tasks[i].component, 17);
                    component_display[17] = '\0';
                }
                
                mvprintw(row, max_x - 55, "%s", component_display);
            }
            
            // Priority with appropriate color
                        int priority_color = is_selected ? COLOR_HIGHLIGHT : get_priority_color(tasks[i].priority);
            attron(COLOR_PAIR(priority_color) | (is_selected ? A_BOLD : 0));
            mvprintw(row, max_x - 35, "%-7s", get_priority_string(tasks[i].priority));
            attroff(COLOR_PAIR(priority_color) | (is_selected ? A_BOLD : 0));
            
            // Status
            if (tasks[i].completed) {
                mvprintw(row, max_x - 25, "[DONE]");
            } else {
                mvprintw(row, max_x - 25, "[PENDING]");
            }
            
            if (is_selected) {
                attroff(COLOR_PAIR(COLOR_HIGHLIGHT) | A_BOLD);
            } else {
                attroff(COLOR_PAIR(COLOR_DEFAULT));
            }
        }
        
        // Show pagination info if needed
        if (total_pages > 1) {
            attron(COLOR_PAIR(COLOR_INFO) | A_BOLD);
            mvprintw(max_y - 4, 3, "Page %d of %d", page + 1, total_pages);
            attroff(COLOR_PAIR(COLOR_INFO) | A_BOLD);
            
            // Page navigation hints
            attron(COLOR_PAIR(COLOR_HIGHLIGHT));
            mvprintw(max_y - 4, 20, "[←/→ arrows to navigate pages]");
            attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
        }
        
        // Draw footer with navigation help
        draw_footer("UP/DOWN: Select Task | LEFT/RIGHT: Change Page | ENTER: Edit | Q: Cancel");
        
        // Get user input
        ch = getch();
        
        // Handle navigation
        switch (ch) {
            case KEY_UP:
                if (selected_task > 0) {
                    selected_task--;
                    // If selection moves to previous page
                    if (selected_task < page * TASKS_PER_PAGE) {
                        page--;
                    }
                }
                break;
                
            case KEY_DOWN:
                if (selected_task < taskCount - 1) {
                    selected_task++;
                    // If selection moves to next page
                    if (selected_task >= (page + 1) * TASKS_PER_PAGE) {
                        page++;
                    }
                }
                break;
                
            case KEY_RIGHT:
                // Next page
                if (page < total_pages - 1) {
                    page++;
                    // Adjust selected_task to first item on new page
                    if (selected_task < page * TASKS_PER_PAGE) {
                        selected_task = page * TASKS_PER_PAGE;
                    }
                }
                break;
                
            case KEY_LEFT:
                // Previous page
                if (page > 0) {
                    page--;
                    // Adjust selected_task to last item on new page
                    if (selected_task >= (page + 1) * TASKS_PER_PAGE) {
                        selected_task = (page + 1) * TASKS_PER_PAGE - 1;
                    }
                }
                break;
                
            case 10: // Enter key
                task_selected = true;
                break;
                
            case 'q':
            case 'Q':
                return; // Exit without editing
        }
    }
    
    // Found task to edit
    Task *task = &tasks[selected_task];
    
    clear();
    draw_header("Edit Task");
    
    // Show current task details
    attron(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
    mvprintw(3, 3, "Editing Task #%d", task->id);
    attroff(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
    
    // Edit title
    attron(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
    mvprintw(5, 3, "Title:");
    attroff(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
    
    attron(COLOR_PAIR(COLOR_DEFAULT));
    mvprintw(5, 10, "[%s]", task->title);
    attroff(COLOR_PAIR(COLOR_DEFAULT));
    
    echo();
    curs_set(1);
    
    char new_title[TITLE_LENGTH];
    mvgetnstr(5, 11 + strlen(task->title), new_title, TITLE_LENGTH - 1);
    
    if (strlen(new_title) > 0) {
        strncpy(task->title, new_title, TITLE_LENGTH - 1);
        task->title[TITLE_LENGTH - 1] = '\0';
    }
    
    // Edit description
    attron(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
    mvprintw(7, 3, "Description:");
    attroff(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
    
    attron(COLOR_PAIR(COLOR_DEFAULT));
    mvprintw(7, 16, "[%s]", task->description);
    attroff(COLOR_PAIR(COLOR_DEFAULT));
    
    char new_description[DESC_LENGTH];
    mvgetnstr(7, 17 + strlen(task->description), new_description, DESC_LENGTH - 1);
    
    if (strlen(new_description) > 0) {
        strncpy(task->description, new_description, DESC_LENGTH - 1);
        task->description[DESC_LENGTH - 1] = '\0';
    }
    
    // Edit priority with color
    attron(COLOR_PAIR(get_priority_color(task->priority)) | A_BOLD);
    mvprintw(9, 3, "Priority [%s] (1-Low, 2-Medium, 3-High): ", 
             get_priority_string(task->priority));
    attroff(COLOR_PAIR(get_priority_color(task->priority)) | A_BOLD);
    
    char new_priority[2];
    mvgetnstr(9, 45, new_priority, 1);
    
    if (strlen(new_priority) > 0) {
        int priority = atoi(new_priority);
        if (priority >= 1 && priority <= 3) {
            task->priority = priority;
        }
    }
    
    // Edit component
    attron(COLOR_PAIR(COLOR_COMPONENT) | A_BOLD);
    mvprintw(11, 3, "Component [%s]: ", task->component);
    attroff(COLOR_PAIR(COLOR_COMPONENT) | A_BOLD);
    
    char new_component[COMPONENT_NAME_LENGTH];
    mvgetnstr(11, 15 + strlen(task->component), new_component, COMPONENT_NAME_LENGTH - 1);
    
    // If component changed, update component lists
    if (strlen(new_component) > 0 && strcmp(task->component, new_component) != 0) {
        // Remove task from old component if it had one
        if (strlen(task->component) > 0) {
            remove_task_from_component(task->id, task->component);
        }
        
        // Update task's component
        strncpy(task->component, new_component, COMPONENT_NAME_LENGTH - 1);
        task->component[COMPONENT_NAME_LENGTH - 1] = '\0';
        
        // Add to new component
        add_task_to_component(task->id, new_component);
    }
    
    // Edit assignee
    attron(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
    mvprintw(13, 3, "Assignee [%s]: ", task->assignee);
    attroff(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
    
    char new_assignee[MAX_ASSIGNEE_LENGTH];
    mvgetnstr(13, 15 + strlen(task->assignee), new_assignee, MAX_ASSIGNEE_LENGTH - 1);
    
    if (strlen(new_assignee) > 0) {
        strncpy(task->assignee, new_assignee, MAX_ASSIGNEE_LENGTH - 1);
        task->assignee[MAX_ASSIGNEE_LENGTH - 1] = '\0';
    }
    
    // Toggle completion status
    attron(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
    mvprintw(15, 3, "Completed [%s] (y/n): ", task->completed ? "Yes" : "No");
    attroff(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
    
    char completed[2];
    mvgetnstr(15, 25, completed, 1);
    
    if (completed[0] == 'y' || completed[0] == 'Y') {
        task->completed = 1;
    } else if (completed[0] == 'n' || completed[0] == 'N') {
        task->completed = 0;
    }
    
    noecho();
    curs_set(0);
    
    // Update timestamp
    task->updated_at = time(NULL);
    
    // Save all tasks and components
    save_tasks();
    save_components();
    
    // Show success message
    display_message("Task updated successfully!", COLOR_SUCCESS);
    getch();
}