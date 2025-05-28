#include "../include/ui_core.h"

// Initialize color pairs
void initialize_colors() {
    start_color();
    
    // Define color pairs
    init_pair(COLOR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_HEADER, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_HIGHLIGHT, COLOR_BLACK, COLOR_CYAN);
    init_pair(COLOR_LOW, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_MEDIUM, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_HIGH, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_SUCCESS, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_ERROR, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_INFO, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_COMPONENT, COLOR_MAGENTA, COLOR_BLACK);
}

// Draw the header with title
void draw_header(const char* title) {
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);
    
    // Draw header line
    attron(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
    mvhline(0, 0, ' ', max_x);
    mvprintw(0, (max_x - strlen(title) - 4) / 2, "[T] %s", title);
    attroff(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
    
    // Draw separator line
    attron(COLOR_PAIR(COLOR_DEFAULT));
    mvhline(1, 0, '-', max_x);
    attroff(COLOR_PAIR(COLOR_DEFAULT));
}

// Draw the footer with message
void draw_footer(const char* message) {
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);
    
    // Draw separator line
    attron(COLOR_PAIR(COLOR_DEFAULT));
    mvhline(max_y - 2, 0, '-', max_x);
    attroff(COLOR_PAIR(COLOR_DEFAULT));
    
    // Draw footer with message
    attron(COLOR_PAIR(COLOR_INFO));
    mvhline(max_y - 1, 0, ' ', max_x);
    mvprintw(max_y - 1, 2, "%s", message);
    attroff(COLOR_PAIR(COLOR_INFO));
}