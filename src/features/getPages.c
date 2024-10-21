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

void getPages(command cmd) {
    log_message(LOG_DEBUG, "Entering function getPages");
    
    pageList* head = NULL;
    head = populatePageList(&head, "path", cmd.argument_1); // Parse JSON and populate linked list
    printPages(&head);
    sendMessageToSlack("All Pages Printed\n");
    // Free the memory used by the linked list
    freePageList(&head);

    log_message(LOG_DEBUG, "Entering function getPages");
    
    return; 
}

static void printPages(pageList** head) {
    log_message(LOG_DEBUG, "Entering function printPages");
    
    pageList* current = *head;
    while (current != NULL) {
        sleep(2);
        current = getPage(&current);
        sendMessageToSlack("Page Title: \n");
        sendMessageToSlack(current->title);
        sendMessageToSlack("Page Content: \n");
        sendMessageToSlack(current->content);
        sendMessageToSlack("\n\n");
        
        current = current->next;
    }
    
    log_message(LOG_DEBUG, "Exiting function printPages");
}