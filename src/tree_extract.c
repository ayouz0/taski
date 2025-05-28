#include "../include/tree_extract.h"
#include <ctype.h>

// Extract priority from a description string
int extract_priority_from_description(char* description, int default_priority) {
    if (description == NULL) return default_priority;
    
    char *priority_tags[] = {"#low", "#medium", "#high"};
    int priorities[] = {1, 2, 3};
    
    for (int i = 0; i < 3; i++) {
        char *tag_pos = strstr(description, priority_tags[i]);
        if (tag_pos != NULL) {
            // Remove the tag from the description
            memmove(tag_pos, tag_pos + strlen(priority_tags[i]), 
                    strlen(tag_pos + strlen(priority_tags[i])) + 1);
            
            // Trim any resulting double spaces
            char *p = strstr(description, "  ");
            while (p != NULL) {
                memmove(p, p + 1, strlen(p + 1) + 1);
                p = strstr(description, "  ");
            }
            
            return priorities[i];
        }
    }
    
    return default_priority;
}

// Extract assignee from a description string
char* extract_assignee_from_description(char* description, char* assignee_buffer) {
    if (description == NULL || assignee_buffer == NULL) {
        if (assignee_buffer) assignee_buffer[0] = '\0';
        return assignee_buffer;
    }
    
    assignee_buffer[0] = '\0';
    
    // Look for @username format
    char *at_symbol = strchr(description, '@');
    if (at_symbol == NULL) {
        return assignee_buffer;
    }
    
    // Extract username after @
    int i = 0;
    char *ptr = at_symbol + 1;
    
    // Copy until space, non-alphanumeric, or end of string
    while (*ptr && (isalnum(*ptr) || *ptr == '_') && i < MAX_ASSIGNEE_LENGTH - 1) {
        assignee_buffer[i++] = *ptr++;
    }
    assignee_buffer[i] = '\0';
    
    // If we found an assignee, remove it from the description
    if (strlen(assignee_buffer) > 0) {
        int tag_len = strlen(assignee_buffer) + 1; // +1 for @ symbol
        memmove(at_symbol, at_symbol + tag_len, strlen(at_symbol + tag_len) + 1);
        
        // Trim any resulting double spaces
        char *p = strstr(description, "  ");
        while (p != NULL) {
            memmove(p, p + 1, strlen(p + 1) + 1);
            p = strstr(description, "  ");
        }
    }
    
    return assignee_buffer;
}