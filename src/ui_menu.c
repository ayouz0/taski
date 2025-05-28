#include "../include/ui_menu.h"

// Draw the menu with options
void draw_menu(const char* options[], int num_options, int selected) {
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);
    
    int menu_width = 40;
    int start_y = (max_y - num_options * 3) / 2;
    int start_x = (max_x - menu_width) / 2;
    
    // Draw menu title
    attron(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
    mvprintw(start_y - 2, start_x, "+-------------- MAIN MENU -------------+");
    attroff(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
    
    // Draw each option in a box
    for (int i = 0; i < num_options; i++) {
        int y = start_y + i * 3;
        
        if (i == selected) {
            attron(COLOR_PAIR(COLOR_HIGHLIGHT) | A_BOLD);
            // Top border
            mvprintw(y, start_x, "+");
            for (int j = 0; j < menu_width - 2; j++) 
                mvaddch(y, start_x + 1 + j, '-');
            mvprintw(y, start_x + menu_width - 1, "+");
            
            // Option with shortcut
            mvprintw(y + 1, start_x, "| [%c] %-32s |", options[i][0], options[i]);
            
            // Bottom border
            mvprintw(y + 2, start_x, "+");
            for (int j = 0; j < menu_width - 2; j++) 
                mvaddch(y + 2, start_x + 1 + j, '-');
            mvprintw(y + 2, start_x + menu_width - 1, "+");
            attroff(COLOR_PAIR(COLOR_HIGHLIGHT) | A_BOLD);
        } else {
            attron(COLOR_PAIR(COLOR_DEFAULT));
            // Top border
            mvprintw(y, start_x, "+");
            for (int j = 0; j < menu_width - 2; j++) 
                mvaddch(y, start_x + 1 + j, '-');
            mvprintw(y, start_x + menu_width - 1, "+");
            
            // Option with shortcut
            mvprintw(y + 1, start_x, "| (%c) %-32s |", options[i][0], options[i]);
            
            // Bottom border
            mvprintw(y + 2, start_x, "+");
            for (int j = 0; j < menu_width - 2; j++) 
                mvaddch(y + 2, start_x + 1 + j, '-');
            mvprintw(y + 2, start_x + menu_width - 1, "+");
            attroff(COLOR_PAIR(COLOR_DEFAULT));
        }
    }
}