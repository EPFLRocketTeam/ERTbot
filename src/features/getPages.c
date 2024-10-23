#include <unistd.h>
#include "ERTbot_common.h"
#include "pageListHelpers.h"
#include "wikiAPI.h"
#include "slackAPI.h"



/**
 * @brief Prints the title and content of each page in the linked list to Slack.
 * 
 * This function iterates through the linked list of `pageList` nodes and sends the title and content of each page to Slack
 * using `sendMessageToSlack`. It also includes formatting to clearly separate each page's information.
 * 
 * @param head A pointer to a pointer to the head of the linked list of `pageList` structures. The function processes each
 *             node in the list and sends its title and content to Slack.
 * 
 * @details The function performs the following steps:
 *          - Iterates through each node in the linked list.
 *          - Retrieves the current page using `getPage` and processes the page's title and content.
 *          - Sends the page title and content to Slack using `sendMessageToSlack`, including appropriate labels and spacing.
 */
static void printPages(pageList** head);

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