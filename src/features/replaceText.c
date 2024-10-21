#include "../include/struct.h"
#include "../include/api.h"
#include "../include/config.h"
#include "../include/features.h"
#include "../include/githubAPI.h"
#include "../include/helperFunctions.h"
#include "../include/markdownToPDF.h"
#include "../include/slackAPI.h"
#include "../include/stringTools.h"
#include "../include/wikiAPI.h"
#include "../include/sheetAPI.h"
#include "../include/command.h"
#include "../include/log.h"
#include "../include/requirements.h"

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