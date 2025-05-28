#include "../include/task.h"
#include "../include/ui_message.h"
#include <stdlib.h>
#include <string.h>

// Define theme structure
typedef struct {
    char name[50];
    short default_fg, default_bg;
    short header_fg, header_bg;
    short highlight_fg, highlight_bg;
    short success_fg, success_bg;
    short error_fg, error_bg;
    short info_fg, info_bg;
    short component_fg, component_bg;
    short low_priority_fg, low_priority_bg;
    short med_priority_fg, med_priority_bg;
    short high_priority_fg, high_priority_bg;
} Theme;

// Define available themes
#define NUM_THEMES 5
Theme themes[NUM_THEMES] = {
    // Classic (Dark)
    {
        "Classic Dark",
        COLOR_WHITE, COLOR_BLACK,     // Default
        COLOR_BLACK, COLOR_CYAN,      // Header
        COLOR_WHITE, COLOR_BLUE,      // Highlight
        COLOR_GREEN, COLOR_BLACK,     // Success
        COLOR_RED, COLOR_BLACK,       // Error
        COLOR_YELLOW, COLOR_BLACK,    // Info
        COLOR_CYAN, COLOR_BLACK,      // Component
        COLOR_GREEN, COLOR_BLACK,     // Low priority
        COLOR_YELLOW, COLOR_BLACK,    // Medium priority
        COLOR_RED, COLOR_BLACK        // High priority
    },
    
    // Light Theme
    {
        "Light Mode",
        COLOR_BLACK, COLOR_WHITE,     // Default
        COLOR_BLUE, COLOR_WHITE,      // Header
        COLOR_WHITE, COLOR_BLUE,      // Highlight
        COLOR_GREEN, COLOR_WHITE,     // Success
        COLOR_RED, COLOR_WHITE,       // Error
        COLOR_BLUE, COLOR_WHITE,      // Info
        COLOR_MAGENTA, COLOR_WHITE,   // Component
        COLOR_GREEN, COLOR_WHITE,     // Low priority
        COLOR_YELLOW, COLOR_WHITE,    // Medium priority
        COLOR_RED, COLOR_WHITE        // High priority
    },
    
    // Blue Theme
    {
        "Ocean Blue",
        COLOR_WHITE, COLOR_BLUE,      // Default
        COLOR_WHITE, COLOR_CYAN,      // Header
        COLOR_YELLOW, COLOR_BLUE,     // Highlight
        COLOR_GREEN, COLOR_BLUE,      // Success
        COLOR_RED, COLOR_BLUE,        // Error
        COLOR_CYAN, COLOR_BLUE,       // Info
        COLOR_WHITE, COLOR_CYAN,      // Component
        COLOR_GREEN, COLOR_BLUE,      // Low priority
        COLOR_YELLOW, COLOR_BLUE,     // Medium priority
        COLOR_RED, COLOR_BLUE         // High priority
    },
    
    // Green Theme
    {
        "Forest",
        COLOR_WHITE, COLOR_GREEN,     // Default
        COLOR_BLACK, COLOR_YELLOW,    // Header
        COLOR_WHITE, COLOR_RED,       // Highlight
        COLOR_CYAN, COLOR_GREEN,      // Success
        COLOR_RED, COLOR_GREEN,       // Error
        COLOR_YELLOW, COLOR_GREEN,    // Info
        COLOR_BLACK, COLOR_YELLOW,    // Component
        COLOR_BLUE, COLOR_GREEN,      // Low priority
        COLOR_YELLOW, COLOR_GREEN,    // Medium priority
        COLOR_RED, COLOR_GREEN        // High priority
    },
    
    // High Contrast
    {
        "High Contrast",
        COLOR_WHITE, COLOR_BLACK,     // Default
        COLOR_BLACK, COLOR_WHITE,     // Header
        COLOR_BLACK, COLOR_YELLOW,    // Highlight
        COLOR_GREEN, COLOR_BLACK,     // Success
        COLOR_RED, COLOR_BLACK,       // Error
        COLOR_YELLOW, COLOR_BLACK,    // Info
        COLOR_CYAN, COLOR_BLACK,      // Component
        COLOR_GREEN, COLOR_BLACK,     // Low priority
        COLOR_YELLOW, COLOR_BLACK,    // Medium priority
        COLOR_RED, COLOR_BLACK        // High priority
    }
};

// Save and load themes from file
void save_theme_preference() {
    FILE* fp = fopen("theme.cfg", "w");
    if (fp) {
        fprintf(fp, "%d", current_theme);
        fclose(fp);
    }
}

void load_theme_preference() {
    FILE* fp = fopen("theme.cfg", "r");
    if (fp) {
        if (fscanf(fp, "%d", &current_theme) != 1) {
            current_theme = 0; // Default if file is corrupted
        }
        fclose(fp);
        
        // Ensure theme index is valid
        if (current_theme < 0 || current_theme >= NUM_THEMES) {
            current_theme = 0;
        }
    } else {
        current_theme = 0; // Default if file doesn't exist
    }
}

// Get priority color based on current theme
int get_priority_color(int priority) {
    switch (priority) {
        case 1: return COLOR_LOW;
        case 2: return COLOR_MEDIUM;
        case 3: return COLOR_HIGH;
        default: return COLOR_DEFAULT;
    }
}

// Get priority name
const char* get_priority_string(int priority) {
    switch (priority) {
        case 1: return "Low";
        case 2: return "Medium";
        case 3: return "High";
        default: return "Unknown";
    }
}

// Update task IDs to be sequential
void update_task_ids() {
    for (int i = 0; i < taskCount; i++) {
        tasks[i].id = i + 1;
    }
}

// Apply the selected theme
void apply_theme(int theme_index) {
    if (theme_index < 0 || theme_index >= NUM_THEMES) {
        theme_index = 0; // Default to first theme if invalid
    }
    
    current_theme = theme_index;
    Theme* theme = &themes[theme_index];
    
    // Initialize color pairs based on theme
    init_pair(COLOR_DEFAULT, theme->default_fg, theme->default_bg);
    init_pair(COLOR_HEADER, theme->header_fg, theme->header_bg);
    init_pair(COLOR_HIGHLIGHT, theme->highlight_fg, theme->highlight_bg);
    init_pair(COLOR_SUCCESS, theme->success_fg, theme->success_bg);
    init_pair(COLOR_ERROR, theme->error_fg, theme->error_bg);
    init_pair(COLOR_INFO, theme->info_fg, theme->info_bg);
    init_pair(COLOR_COMPONENT, theme->component_fg, theme->component_bg);
    init_pair(COLOR_LOW, theme->low_priority_fg, theme->low_priority_bg);
    init_pair(COLOR_MEDIUM, theme->med_priority_fg, theme->med_priority_bg);
    init_pair(COLOR_HIGH, theme->high_priority_fg, theme->high_priority_bg);
    
    // Set default background
    bkgd(COLOR_PAIR(COLOR_DEFAULT));
    
    // Save the preference
    save_theme_preference();
}

// Show theme selection menu
void select_theme() {
    int selected = current_theme;
    int ch;
    
    do {
        clear();
        
        // Draw header
        attron(COLOR_PAIR(COLOR_HEADER));
        mvprintw(0, 0, "Select Theme");
        for (int i = 12; i < COLS; i++) {
            mvaddch(0, i, ' ');
        }
        attroff(COLOR_PAIR(COLOR_HEADER));
        
        // Draw separator
        attron(COLOR_PAIR(COLOR_DEFAULT));
        for (int i = 0; i < COLS; i++) {
            mvaddch(1, i, '-');
        }
        attroff(COLOR_PAIR(COLOR_DEFAULT));
        
        // Draw themes list
        for (int i = 0; i < NUM_THEMES; i++) {
            if (i == selected) {
                attron(COLOR_PAIR(COLOR_HIGHLIGHT) | A_BOLD);
                mvprintw(i + 3, 4, "> %s %s", 
                         themes[i].name, 
                         (i == current_theme) ? "(Current)" : "");
                attroff(COLOR_PAIR(COLOR_HIGHLIGHT) | A_BOLD);
            } else {
                attron(COLOR_PAIR(COLOR_DEFAULT));
                mvprintw(i + 3, 4, "  %s %s", 
                         themes[i].name,
                         (i == current_theme) ? "(Current)" : "");
                attroff(COLOR_PAIR(COLOR_DEFAULT));
            }
        }
        
        // Draw preview section
        int preview_y = NUM_THEMES + 5;
        
        attron(COLOR_PAIR(COLOR_HEADER));
        mvprintw(preview_y, 4, "Theme Preview");
        attroff(COLOR_PAIR(COLOR_HEADER));
        
        // Save current theme
        int old_theme = current_theme;
        
        // Apply selected theme temporarily for preview
        apply_theme(selected);
        
        // Draw preview elements
        preview_y += 2;
        
        attron(COLOR_PAIR(COLOR_DEFAULT));
        mvprintw(preview_y, 4, "Default Text");
        attroff(COLOR_PAIR(COLOR_DEFAULT));
        
        attron(COLOR_PAIR(COLOR_HEADER));
        mvprintw(preview_y, 20, "Header");
        attroff(COLOR_PAIR(COLOR_HEADER));
        
        attron(COLOR_PAIR(COLOR_HIGHLIGHT));
        mvprintw(preview_y, 30, "Highlighted");
        attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
        
        preview_y++;
        
        attron(COLOR_PAIR(COLOR_SUCCESS));
        mvprintw(preview_y, 4, "Success");
        attroff(COLOR_PAIR(COLOR_SUCCESS));
        
        attron(COLOR_PAIR(COLOR_ERROR));
        mvprintw(preview_y, 20, "Error");
        attroff(COLOR_PAIR(COLOR_ERROR));
        
        attron(COLOR_PAIR(COLOR_INFO));
        mvprintw(preview_y, 30, "Info");
        attroff(COLOR_PAIR(COLOR_INFO));
        
        preview_y++;
        
        attron(COLOR_PAIR(COLOR_COMPONENT));
        mvprintw(preview_y, 4, "Component");
        attroff(COLOR_PAIR(COLOR_COMPONENT));
        
        attron(COLOR_PAIR(COLOR_LOW));
        mvprintw(preview_y, 20, "Low");
        attroff(COLOR_PAIR(COLOR_LOW));
        
        attron(COLOR_PAIR(COLOR_MEDIUM));
        mvprintw(preview_y, 30, "Medium");
        attroff(COLOR_PAIR(COLOR_MEDIUM));
        
        attron(COLOR_PAIR(COLOR_HIGH));
        mvprintw(preview_y, 40, "High");
        attroff(COLOR_PAIR(COLOR_HIGH));
        
        // Footer
        int footer_y = LINES - 2;
        for (int i = 0; i < COLS; i++) {
            mvaddch(footer_y, i, '-');
        }
        
        attron(COLOR_PAIR(COLOR_HEADER));
        mvprintw(LINES - 1, 0, "UP/DOWN: Select | ENTER: Apply | Q: Cancel");
        for (int i = 37; i < COLS; i++) {
            mvaddch(LINES - 1, i, ' ');
        }
        attroff(COLOR_PAIR(COLOR_HEADER));
        
        ch = getch();
        
        // If not confirmed, restore the original theme
        if (ch != 10) { // Not Enter key
            apply_theme(old_theme);
        }
        
        switch (ch) {
            case KEY_UP:
                selected = (selected - 1 + NUM_THEMES) % NUM_THEMES;
                break;
                
            case KEY_DOWN:
                selected = (selected + 1) % NUM_THEMES;
                break;
                
            case 10: // Enter key
			{
                // Keep the selected theme (already applied for preview)
                char message[100];
                sprintf(message, "Theme changed to: %s", themes[current_theme].name);
                display_message(message, COLOR_SUCCESS);
                getch();
                return;}
        }
        
    } while (ch != 'q' && ch != 'Q');
}

// Toggle between light and dark theme (original function kept for backward compatibility)
void toggle_theme() {
    select_theme(); // Now calls the theme selector directly
}