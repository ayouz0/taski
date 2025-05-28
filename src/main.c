#include "../include/task.h"
#include "../include/ui_core.h"
#include "../include/ui_menu.h"
#include "../include/task_add.h"
#include "../include/task_view.h"
#include "../include/task_edit.h"
#include "../include/task_delete.h"
#include "../include/component.h"
#include "../include/tree_parser.h"
#include "../include/fileio_task.h"
#include "../include/fileio_component.h"

// Menu options
const char *options[] = {
    "Add Task",
    "View Tasks",
    "View by Component",
    "Edit Task",
    "Delete Task",
    "Import Tree",
    "Toggle Theme",
    "Exit"
};

// Global variables
Task tasks[MAX_TASKS];
int taskCount = 0;
int current_theme = 0;
Component components[MAX_COMPONENTS];
int componentCount = 0;

int main() {
    int ch;
    int selected = 0;
    
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);          // Hide cursor
    
    if (!has_colors()) {
        endwin();
        printf("Your terminal does not support colors!\n");
        return 1;
    }
    
    // Initialize colors
    initialize_colors();
    
    // Load tasks and components from file
    load_tasks();
    load_components();
    
    // Main program loop
    while (1) {
        clear();
        // Draw UI elements
        draw_header("Task Manager");
        draw_menu(options, NUM_OPTIONS, selected);
        draw_footer("Use Up/Down to navigate, Enter to select");
        
        // Get user input
        ch = getch();
        
        switch (ch) {
            case KEY_UP:
                selected = (selected - 1 + NUM_OPTIONS) % NUM_OPTIONS;
                break;
                
            case KEY_DOWN:
                selected = (selected + 1) % NUM_OPTIONS;
                break;
                
            case 10: // Enter key
                switch (selected) {
                    case 0: add_task(); break;
                    case 1: view_tasks(); break;
                    case 2: view_components(); break;
                    case 3: edit_task(); break;
                    case 4: delete_task(); break;
                    case 5: import_tree(); break;
                    case 6: toggle_theme(); break;
                    case 7: 
                        // Save and exit
                        save_tasks();
                        save_components();
                        clear();
                        endwin();
                        printf("Task Manager closed. Your tasks have been saved.\n");
                        return 0;
                }
                break;
        }
    }
    
    endwin();
    return 0;
}