#include "common.h"
#include <stdbool.h>

/**
 * @brief Replaces occurrences of a string in the content of all pages in a linked list, updates and rerenders the pages on the wiki.
 * 
 * @param head Pointer to the head of the linked list of pages.
 * @param oldString The string to be replaced in the page content.
 * @param newString The string to replace `oldString` with.
 * 
 * @details This function iterates through each page in the linked list, retrieves the page content using `getPage`, replaces occurrences
 *          of `oldString` with `newString`, escapes backslashes and quotes in the content, and updates the page content on the wiki
 *          using `updatePageContentMutation`. It then performs a render mutation for each page to apply the changes using `renderMutation`.
 *          After processing all pages, it frees the memory allocated for the page list using `freePageList` and sends a message to Slack
 *          indicating which pages have been updated.
 */
static void replaceStringInWiki(pageList** head, char* oldString, char* newString);

void replaceText(command cmd) {
    log_message(LOG_DEBUG, "Entering function replaceText");
    
    pageList* head;
    head = populatePageList(&head, "path", cmd.argument_1); // Parse JSON and populate linked list
    replaceStringInWiki(&head, cmd.argument_2,cmd.argument_3);
    // Free the memory used by the linked list
    freePageList(&head);
    
    log_message(LOG_DEBUG, "Exiting function replaceText");
}

static void replaceStringInWiki(pageList** head, char* oldString, char* newString) {
    log_message(LOG_DEBUG, "Entering function replaceStringInWiki");
    
    pageList* current = *head;
    while (current != NULL) {
        current = getPage(&current);
        current->content = replaceWord(current->content, oldString, newString);
        current->content = replaceWord(current->content, "\\", "\\\\");
        current->content = replaceWord(current->content, "\"", "\\\"");
        updatePageContentMutation(current);
        renderMutation(&current, false);
        current = current->next;
    }

    freePageList(head);
    
    log_message(LOG_DEBUG, "Exiting function replaceStringInWiki");
}