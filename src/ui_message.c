#include "../include/ui_message.h"

// Display a message box
void display_message(const char* message, int color_pair) {
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);
    
    // Draw message box
    int msg_len = strlen(message);
    int box_width = msg_len + 6;
    int start_x = (max_x - box_width) / 2;
    int start_y = max_y / 2 - 2;
    
    attron(color_pair | A_BOLD);
    
    // Draw top border
    mvaddch(start_y, start_x, '+');
    for (int i = 1; i < box_width - 1; i++) {
        mvaddch(start_y, start_x + i, '-');
    }
    mvaddch(start_y, start_x + box_width - 1, '+');
    
    // Draw sides
    for (int i = 1; i <= 3; i++) {
        mvaddch(start_y + i, start_x, '|');
        mvaddch(start_y + i, start_x + box_width - 1, '|');
    }
    
    // Draw bottom border
    mvaddch(start_y + 4, start_x, '+');
    for (int i = 1; i < box_width - 1; i++) {
        mvaddch(start_y + 4, start_x + i, '-');
    }
    mvaddch(start_y + 4, start_x + box_width - 1, '+');
    
    // Draw message
    mvprintw(start_y + 2, start_x + 3, "%s", message);
    
    attroff(color_pair | A_BOLD);
}