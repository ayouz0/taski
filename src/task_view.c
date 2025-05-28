#include "../include/task_view.h"
#include "../include/ui_core.h"

// View all tasks
void view_tasks() {
    clear();
    draw_header("Task List");
    
    if (taskCount == 0) {
        attron(COLOR_PAIR(COLOR_INFO));
        mvprintw(10, 20, "No tasks available.");
        attroff(COLOR_PAIR(COLOR_INFO));
        
        draw_footer("Press any key to return to menu");
        getch();
        return;
    }
    
    int page = 0;
    int ch;
    const int TASKS_PER_PAGE = 12;
    int total_pages = (taskCount + TASKS_PER_PAGE - 1) / TASKS_PER_PAGE;
    
    do {
        clear();
        draw_header("Task List");
        
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
        mvprintw(3, max_x - 15, "ASSIGNEE");
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
            
            // ID with bold
            attron(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
            mvprintw(row, 3, "%d", tasks[i].id);
            attroff(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
            
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
            
            attron(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
            mvprintw(row, 8, "%s", title_display);
            attroff(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
            
            // Component
            if (strlen(tasks[i].component) > 0) {
                attron(COLOR_PAIR(COLOR_COMPONENT) | A_BOLD);
                
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
                attroff(COLOR_PAIR(COLOR_COMPONENT) | A_BOLD);
            }
            
            // Priority
            int priority_color = get_priority_color(tasks[i].priority);
            attron(COLOR_PAIR(priority_color) | A_BOLD);
            mvprintw(row, max_x - 35, "%-7s", get_priority_string(tasks[i].priority));
            attroff(COLOR_PAIR(priority_color) | A_BOLD);
            
            // Status
            if (tasks[i].completed) {
                attron(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
                mvprintw(row, max_x - 25, "[DONE]");
                attroff(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
            } else {
                attron(COLOR_PAIR(COLOR_INFO) | A_BOLD);
                mvprintw(row, max_x - 25, "[PENDING]");
                attroff(COLOR_PAIR(COLOR_INFO) | A_BOLD);
            }
            
            // Assignee
            if (strlen(tasks[i].assignee) > 0) {
                attron(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
                mvprintw(row, max_x - 15, "@%s", tasks[i].assignee);
                attroff(COLOR_PAIR(COLOR_DEFAULT) | A_BOLD);
            }
        }
        
        // Show pagination info if needed
        if (total_pages > 1) {
            attron(COLOR_PAIR(COLOR_INFO) | A_BOLD);
            mvprintw(max_y - 4, 3, "Page %d of %d", page + 1, total_pages);
            attroff(COLOR_PAIR(COLOR_INFO) | A_BOLD);
            
            // Page navigation hints
            attron(COLOR_PAIR(COLOR_HIGHLIGHT));
            mvprintw(max_y - 4, 20, "[<-/-> arrows to navigate pages]");
            attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
        }
        
        // Draw footer with navigation help
        draw_footer("LEFT/RIGHT: Change Page | Q: Return to Menu");
        
        // Get user input
        ch = getch();
        
        // Handle pagination
        switch (ch) {
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