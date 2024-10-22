#include "common.h"
#include <stdbool.h>

/**
 * @brief Parses a text to extract and link wiki flags based on specific comment markers.
 * 
 * This function scans the provided text for comments marked with "<!--" and "-->". It extracts command information 
 * from these comments and creates a linked list of `wikiFlag` structures. The `wikiFlag` structures are populated 
 * with pointers to the extracted text segments and associated commands.
 * 
 * @param text The input text to be parsed for wiki flags.
 * @param flag A `wikiFlag` structure used as a template for new flag entries. It is not modified but serves as a base for memory allocation.
 * 
 * @return A pointer to the head of a linked list of `wikiFlag` structures. Each node in the list contains extracted command information and text pointers.
 * 
 * @details The function iterates through the text looking for comment markers. It extracts text between "<!--" and "-->" markers, creates a `wikiFlag` 
 *          structure for each pair of markers, and links these structures in a list. Odd-numbered flags are associated with pointers starting 
 *          from the end of the comment, while even-numbered flags are associated with pointers starting from the beginning of the comment. 
 *          The function handles multiple pairs of markers and continues parsing until the end of the text is reached.
 */
static wikiFlag *parseFlags(char* text);

static void freeWikiFlagList(wikiFlag** head);

void onPageUpdate(command cmd){
    log_message(LOG_DEBUG, "Entering function onPageUpdate");

    log_message(LOG_DEBUG, "initalising page");
    int wasPageModified = 0;

    pageList* targetPage = NULL;
    targetPage = addPageToList(&targetPage, cmd.argument_1, "", "", "", "", "", "", "");
    log_message(LOG_DEBUG, "targetPage id set");
    targetPage = getPage(&targetPage);

    if(strcmp(targetPage->authorId, "1") == 0){
        freePageList(&targetPage);
        return;
    }

    log_message(LOG_DEBUG, "authorID: %s\n", targetPage->authorId);

    log_message(LOG_DEBUG, "initalising wikiFlage pointer");
    wikiFlag* wikiCommand = (wikiFlag*)malloc(sizeof(wikiFlag));

    log_message(LOG_DEBUG, "calling parseFlags");
    wikiCommand = parseFlags(targetPage->content);

    
    while(wikiCommand){
        
        if(wikiCommand->cmd.function && strcmp(wikiCommand->cmd.function, "buildMap") == 0){
            char* map = buildMap(wikiCommand->cmd);
            map = createCombinedString("\n", map);
            targetPage->content = replaceParagraph(targetPage->content, map, wikiCommand->pointerToEndOfFirstMarker, wikiCommand->pointerToBeginningOfSecondMarker);

            wasPageModified = 1;
            free(map);
        }

        log_message(LOG_DEBUG, "going to send message to slack");
        

        wikiCommand = wikiCommand->next;
    }

    if(wasPageModified){
        targetPage->content = replaceWord(targetPage->content, "\n", "\\n");
        targetPage->content = replaceWord(targetPage->content, "\\", "\\\\");
        targetPage->content = replaceWord(targetPage->content, "\"", "\\\"");
        updatePageContentMutation(targetPage);
        renderMutation(&targetPage, false);
        freePageList(&targetPage);
        sendMessageToSlack("onPageUpdate called on page id:");
        sendMessageToSlack(cmd.argument_1);
    }

    freeWikiFlagList(&wikiCommand);
    
    log_message(LOG_DEBUG, "Exiting function onPageUpdate");
}

static void freeWikiFlagList(wikiFlag** head) {
    log_message(LOG_DEBUG, "Entering function freeWikiFlagList");
    
    while (*head) {
        wikiFlag* temp = *head;
        *head = (*head)->next;
        
        // Debugging prints
        if (temp->pointerToBeginningOfSecondMarker){
            free(temp->pointerToBeginningOfSecondMarker);
        }
        if (temp->pointerToBeginningOfSecondMarker){
            free(temp->pointerToBeginningOfSecondMarker);
        }
        
        log_message(LOG_DEBUG, "about to free temp");
        free(temp);
        log_message(LOG_DEBUG, "freed temp");
    }

    
    log_message(LOG_DEBUG, "Exiting function freeWikiFlagList");
}

static wikiFlag* parseFlags(char* text) {
    log_message(LOG_DEBUG, "Entering function parseFlags");
    
    wikiFlag* head = NULL;
    wikiFlag* current = NULL;
    char* start = text;
    char* end = text;
    int flagCount = 0;
    command cmd;
    wikiFlag *newFlag = (wikiFlag*)malloc(sizeof(wikiFlag));
    
    while (*end != '\0') {
        //log_message(LOG_DEBUG, "Current chars: %c%c%c%c", *end, *(end+1), *(end+2), *(end+3));
        if (*end == '<' && *(end + 1) == '!' && *(end + 2) == '-' && *(end + 3) == '-') {
            log_message(LOG_DEBUG, "Found a comment");

            flagCount ++;
            start = end;
            end += 4; // Move to the first character after '<!--'
            
            // Finding the end of the comment
            while (*end != '\0' && !(*end == '-' && *(end + 1) == '-' && *(end + 2) == '>')) {
                end++;
            }
            
            if (*end == '\0') break; // Reached the end of the text
            
            breakdownCommand(extractParagraphWithPointerDelimiters(text, start+4 , end-1), &cmd);

            log_message(LOG_DEBUG, "Wiki command found in when parsing the flags Function:\"%s\", Command\"%s\"", cmd.function, cmd.argument_1);
            
            
            newFlag->cmd = cmd;

            if (flagCount % 2 != 0){ //odd
                newFlag->pointerToEndOfFirstMarker = (char*)(end + 3);
                log_message(LOG_DEBUG, "Set pointerToEndOfFirstMarker to: \"%ld\"", (long)newFlag->pointerToEndOfFirstMarker);
            }

            if (flagCount % 2 == 0){ //even
                newFlag->pointerToBeginningOfSecondMarker = (char*)(start - 1);
                log_message(LOG_DEBUG, "Set pointerToBeginningOfSecondMarker to: \"%ld\"", (long)newFlag->pointerToBeginningOfSecondMarker);
                log_message(LOG_DEBUG, "pointerToEndOfFirstMarker is: \"%ld\"", (long)newFlag->pointerToEndOfFirstMarker);

                // Adding to the linked list
                newFlag->next = NULL;
                if (head == NULL) {
                    head = newFlag;
                    current = newFlag;
                    wikiFlag *newFlag = (wikiFlag*)malloc(sizeof(wikiFlag));
                } else {
                    current->next = newFlag;
                    current = newFlag;
                    wikiFlag *newFlag = (wikiFlag*)malloc(sizeof(wikiFlag));
                }
            }
            
        }
        end++;
    }
    
    log_message(LOG_DEBUG, "Exiting function parseFlags");
    return head;
}
