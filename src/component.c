#include "../include/component.h"
#include "../include/ui_core.h"
#include "../include/ui_message.h"

// Find a component by name
int find_component_by_name(const char* name) {
    if (name == NULL) return -1;
    
    for (int i = 0; i < componentCount; i++) {
        if (strcmp(components[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Add a task to a component
void add_task_to_component(int task_id, const char* component_name) {
    if (component_name == NULL || strlen(component_name) == 0) return;
    
    int comp_idx = find_component_by_name(component_name);
    
    if (comp_idx >= 0) {
        // Component exists, add task to it if there's room
        if (components[comp_idx].task_count < MAX_TASKS) {
            components[comp_idx].task_ids[components[comp_idx].task_count++] = task_id;
        }
    } else if (componentCount < MAX_COMPONENTS) {
        // Create new component
        strncpy(components[componentCount].name, component_name, COMPONENT_NAME_LENGTH - 1);
        components[componentCount].name[COMPONENT_NAME_LENGTH - 1] = '\0';
        components[componentCount].task_count = 1;
        components[componentCount].task_ids[0] = task_id;
        componentCount++;
    }
}

// Remove a task from a component
void remove_task_from_component(int task_id, const char* component_name) {
    if (component_name == NULL) return;
    
    int comp_idx = find_component_by_name(component_name);
    
    if (comp_idx >= 0) {
        // Find and remove this task from the component
        for (int j = 0; j < components[comp_idx].task_count; j++) {
            if (components[comp_idx].task_ids[j] == task_id) {
                // Shift remaining tasks down
                for (int k = j; k < components[comp_idx].task_count - 1; k++) {
                    components[comp_idx].task_ids[k] = components[comp_idx].task_ids[k + 1];
                }
                components[comp_idx].task_count--;
                break;
            }
        }
    }
}

// View tasks by component
void view_components() {
    if (componentCount == 0) {
        display_message("No components available.", COLOR_INFO);
        getch();
        return;
    }

    int selected = 0;
    int page = 0;
    int ch;
    const int TASKS_PER_PAGE = 20;  // Increased from 10 to 12

    do {
        clear();
        draw_header("Component View");
        
        // Calculate screen dimensions
        int max_x, max_y;
        getmaxyx(stdscr, max_y, max_x);
        
        // Define panel widths - make left panel narrower
        int left_panel_width = max_x * 0.3;  // 30% of screen width
        int right_panel_width = max_x - left_panel_width - 3;  // Remaining width minus separator
        int separator_x = left_panel_width + 1;
        
        // Draw section headers
        attron(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
        mvprintw(3, 3, "COMPONENTS");
        mvprintw(3, separator_x + 2, "TASKS IN %s", components[selected].name);
        attroff(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
        
        // Draw section separators
        attron(COLOR_PAIR(COLOR_DEFAULT));
        mvhline(4, 3, '-', left_panel_width - 3);
        mvhline(4, separator_x + 2, '-', right_panel_width - 2);
        attroff(COLOR_PAIR(COLOR_DEFAULT));
        
        // Draw vertical separator
        attron(COLOR_PAIR(COLOR_DEFAULT));
        for (int i = 3; i < max_y - 3; i++) {
            mvaddch(i, separator_x, '|');
        }
        attroff(COLOR_PAIR(COLOR_DEFAULT));
        
        // Draw components list
        for (int i = 0; i < componentCount && i < (max_y - 8); i++) {
            if (i == selected) {
                attron(COLOR_PAIR(COLOR_HIGHLIGHT) | A_BOLD);
                mvprintw(6 + i, 3, "> %-15s (%d)", 
                         components[i].name, components[i].task_count);
                attroff(COLOR_PAIR(COLOR_HIGHLIGHT) | A_BOLD);
            } else {
                attron(COLOR_PAIR(COLOR_DEFAULT));
                mvprintw(6 + i, 3, "  %-15s (%d)", 
                         components[i].name, components[i].task_count);
                attroff(COLOR_PAIR(COLOR_DEFAULT));
            }
        }
        
        // Calculate total pages for current component's tasks
        int total_pages = (components[selected].task_count + TASKS_PER_PAGE - 1) / TASKS_PER_PAGE;
        if (total_pages == 0) total_pages = 1;  // At least one page even if empty
        
        // Ensure page is valid
        if (page >= total_pages) {
            page = total_pages - 1;
        }
        
        if (components[selected].task_count == 0) {
            attron(COLOR_PAIR(COLOR_INFO));
            mvprintw(6, separator_x + 2, "No tasks in this component.");
            attroff(COLOR_PAIR(COLOR_INFO));
        } else {
            // Calculate start and end indices for current page
            int start_idx = page * TASKS_PER_PAGE;
            int end_idx = start_idx + TASKS_PER_PAGE;
            if (end_idx > components[selected].task_count) {
                end_idx = components[selected].task_count;
            }
            
            // Table header for tasks
            attron(COLOR_PAIR(COLOR_HEADER));
            mvprintw(5, separator_x + 2, "STATUS");
            mvprintw(5, separator_x + 15, "TITLE");
            mvprintw(5, max_x - 25, "PRIORITY");
            mvprintw(5, max_x - 15, "ASSIGNEE");
            attroff(COLOR_PAIR(COLOR_HEADER));
            
            // Draw tasks
            for (int i = start_idx; i < end_idx; i++) {
                int task_id = components[selected].task_ids[i];
                int task_idx = -1;
                
                // Find the task with this ID
                for (int j = 0; j < taskCount; j++) {
                    if (tasks[j].id == task_id) {
                        task_idx = j;
                        break;
                    }
                }
                
                if (task_idx != -1) {
                    Task *task = &tasks[task_idx];
                    int row = 6 + (i - start_idx);
                    
                    // Draw task status with color
                    if (task->completed) {
                        attron(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
                        mvprintw(row, separator_x + 2, "[DONE]");
                        attroff(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
                    } else {
                        attron(COLOR_PAIR(COLOR_INFO));
                        mvprintw(row, separator_x + 2, "[PENDING]");
                        attroff(COLOR_PAIR(COLOR_INFO));
                    }
                    
                    // Calculate available width for title
                    int title_max_width = max_x - separator_x - 35;
                    
                    // Title (truncate if needed)
                    char title_display[256]; // Large buffer
                    if (strlen(task->title) > (size_t)title_max_width) {
                        strncpy(title_display, task->title, title_max_width - 3);
                        title_display[title_max_width - 3] = '.';
                        title_display[title_max_width - 2] = '.';
                        title_display[title_max_width - 1] = '.';
                        title_display[title_max_width] = '\0';
                    } else {
                        strncpy(title_display, task->title, sizeof(title_display) - 1);
                        title_display[sizeof(title_display) - 1] = '\0';
                    }
                    
                    // Draw task title
                    attron(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
                    mvprintw(row, separator_x + 15, "%s", title_display);
                    attroff(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
                    
                    // Draw task priority
                    int priority_color = get_priority_color(task->priority);
                    attron(COLOR_PAIR(priority_color) | A_BOLD);
                    mvprintw(row, max_x - 25, "%-7s", get_priority_string(task->priority));
                    attroff(COLOR_PAIR(priority_color) | A_BOLD);
                    
                    // Draw assignee if any
                    if (strlen(task->assignee) > 0) {
                        attron(COLOR_PAIR(COLOR_COMPONENT) | A_BOLD);
                        mvprintw(row, max_x - 15, "@%s", task->assignee);
                        attroff(COLOR_PAIR(COLOR_COMPONENT) | A_BOLD);
                    }
                }
            }
            
            // Show pagination info if needed
            if (total_pages > 1) {
                attron(COLOR_PAIR(COLOR_INFO) | A_BOLD);
                mvprintw(max_y - 4, separator_x + 2, "Page %d of %d", page + 1, total_pages);
                attroff(COLOR_PAIR(COLOR_INFO) | A_BOLD);
                
                // Page navigation hints
                attron(COLOR_PAIR(COLOR_HIGHLIGHT));
                mvprintw(max_y - 4, separator_x + 20, "[<-/-> arrows to navigate pages]");
                attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
            }
        }
        
        // Draw footer with navigation help
        draw_footer("UP/DOWN: Select Component | LEFT/RIGHT: Change Page | Q: Return to Menu");
        
        // Get user input
        ch = getch();
        
        // Handle navigation
        switch (ch) {
            case KEY_UP:
                selected = (selected - 1 + componentCount) % componentCount;
                page = 0; // Reset page when changing components
                break;
                
            case KEY_DOWN:
                selected = (selected + 1) % componentCount;
                page = 0; // Reset page when changing components
                break;
            
            case KEY_RIGHT:
                // Next page
                if (page < total_pages - 1) {
                    page++;
                }
                break;
                
            case KEY_LEFT:
                // Previous page
                if (page > 0) {
                    page--;
                }
                break;
        }
        
    } while (ch != 'q' && ch != 'Q');
}