#include "common.h"
#include "config.h"
#include <stdbool.h>

/**
 * @brief Updates a specific section in a Markdown list with new links.
 * 
 * This function updates a section of a Markdown list by replacing an old list with a new one. 
 * The section to be updated is identified by a path, and the new list of links is appended to this section.
 * 
 * @param list The original Markdown list as a string. This string will be modified to include the updated list.
 * @param sectionTitle A pointer to a `pageList` structure representing the section to be updated. The path in this structure is used to identify the section.
 * @param links A pointer to a `pageList` structure containing the new list of links to be added under the section.
 * 
 * @return The updated Markdown list as a dynamically allocated string. The caller is responsible for freeing this memory.
 * 
 * @details The function appends the new list of links to the specified section in the original Markdown list. It locates the section in the list using 
 *          the `sectionTitle` path and replaces the old list with the new one. If the section or end of the section is not found, it prints an error 
 *          message and returns the original list. The memory allocated for the new list is freed before returning.
 * 
 * @todo Should be replaced or at least rewritten so that it uses cJSON functions
 */
static char* updateList(char *list, pageList *sectionTitle, pageList* links);

void updateLinksTracker() {  
    log_message(LOG_DEBUG, "Entering function updateLinksTracker");
    
    pageList* linkTrackerPage = (pageList*) malloc(sizeof(pageList)); // Allocate memory for linkTrackerPage
    if (linkTrackerPage == NULL) {
        log_message(LOG_ERROR, "Memory allocation failed for linkTrackerPage");
        return;
    }

    linkTrackerPage->id = LINK_TRACKER_PAGE_ID; //Initialise linkTrackerPage
    linkTrackerPage = getPage(&linkTrackerPage); //get content and updated at values

    pageList* listOfAllPages = NULL; //Getch list of pages filtered by time and filtered in order to only get pages updated after linkTrackerPage

    listOfAllPages = populatePageList(&listOfAllPages, "time", linkTrackerPage->updatedAt); // Parse JSON and populate linked list

    
    //Iterate through list of pages recently modified
    while (listOfAllPages != NULL) {

        //Do not inspect link Tracker Pages
        if(strcmp(listOfAllPages->id, LINK_TRACKER_PAGE_ID) == 0  || strcmp(listOfAllPages->id, BROKEN_LINKS_TRACKER_PAGE_ID) == 0){
            listOfAllPages = listOfAllPages->next;
            continue;
        }

        //Inspect recently modified page
        else{
            listOfAllPages = getPage(&listOfAllPages); //get page content
            
            pageList* links; //initialise linked list to store links
            links = findPageLinks(listOfAllPages->content, &links); //find all of the links in the content

            if(compareTimes(linkTrackerPage->updatedAt, listOfAllPages->createdAt) != 1){
                linkTrackerPage->content = createList(linkTrackerPage->content, &listOfAllPages, links);
            }
            else{
                linkTrackerPage->content = updateList(linkTrackerPage->content, listOfAllPages, links); //update the link tracker page content
            }
            
            while(links != NULL){
                log_message(LOG_DEBUG, "links are: %s", links->path);
                links = links->next;
            }
            
            freePageList(&links); //free the linked list which stores the links
            log_message(LOG_DEBUG, "links freed");

            if(listOfAllPages->next == NULL){break;}
            else{listOfAllPages = listOfAllPages->next;}//go to next page
            

        }
    }
    
    log_message(LOG_DEBUG, "Making sure linkTrackerPage has the right amount of backslashes");
    linkTrackerPage->content = replaceWord(linkTrackerPage->content, "\\n", "\\\\n");

    log_message(LOG_DEBUG, "about to update linkTrackerPage");
    

    updatePageContentMutation(linkTrackerPage);
    renderMutation(&linkTrackerPage, false);

    // Free the memory used by the linked list
    freePageList(&linkTrackerPage);// Free the memory used by the linked list
    //freePageList(&listOfAllPages);// Free the memory used by the linked list
    
    log_message(LOG_DEBUG, "Exiting function updateLinksTracker");
}

static char* updateList(char *list, pageList *sectionTitle, pageList *links) {
    log_message(LOG_DEBUG, "Entering function updateList");
    
    // Ensure proper memory allocation for the tempList
    pageList *link = links;
    char *tempList = sectionTitle->path;
    log_message(LOG_DEBUG, "path:%s\n", sectionTitle->path);

    tempList = appendToString(tempList, ")\\n" );

    
    link = links;
    while (link != NULL) {

        tempList = appendToString(tempList, link->path);
        tempList = appendToString(tempList, "\\n");
        link = link->next;
    }

    char *temp = createCombinedString(sectionTitle->path, ")");

    // Find and replace old list
    char *startPtr = strstr(list, temp);
    //startPtr += 1;
    if (startPtr == NULL) {
        log_message(LOG_DEBUG, "Section title: %s not found\n", sectionTitle->title);
        free(tempList);
        return list;
    }

    char *endPtr = strstr(startPtr, "\\n\\n");
    endPtr +=1;
    if (endPtr == NULL) {
        log_message(LOG_ERROR, "End of section not found");
        free(tempList);
        return list;
    }

    log_message(LOG_DEBUG, "About to update %s to:\n %s\n", sectionTitle->title, tempList);
    list = replaceParagraph(list, tempList, startPtr, endPtr);

    freePageList(&links);
    free(tempList);
    
    log_message(LOG_DEBUG, "Exiting function updateList");
    return list;
}
