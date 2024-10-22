#include "common.h"
#include "config.h"
#include <stdbool.h>

void buildLinksTracker() {
    log_message(LOG_DEBUG, "Starting the buildLinksTracker function");
    
    pageList* linkTrackerPage = NULL;
    linkTrackerPage = addPageToList(&linkTrackerPage, LINK_TRACKER_PAGE_ID, "", "", "", "", "", "", "");
    log_message(LOG_DEBUG, "linkTrackerPage id set");
    linkTrackerPage = getPage(&linkTrackerPage);

    linkTrackerPage->content = "";

    log_message(LOG_DEBUG, "fetched linkTrackerPage content");
    pageList* listOfAllPages = NULL;

    log_message(LOG_DEBUG, "about to fetch list of all pages");
    listOfAllPages = populatePageList(&listOfAllPages, "path", "none"); // Parse JSON and populate linked list
    log_message(LOG_DEBUG, "populated list of pages");
    
    while (listOfAllPages != NULL) {
        log_message(LOG_DEBUG, "about to fetch title: %s", listOfAllPages->title);

        if(strcmp(listOfAllPages->id, LINK_TRACKER_PAGE_ID) == 0  || strcmp(listOfAllPages->id, BROKEN_LINKS_TRACKER_PAGE_ID) == 0){
            listOfAllPages = listOfAllPages->next;
            continue;
        }

        else{
            listOfAllPages = getPage(&listOfAllPages);
            pageList* links;
            links = findPageLinks(listOfAllPages->content, &links);


            linkTrackerPage->content = createList(linkTrackerPage->content, &listOfAllPages, links);
            listOfAllPages = listOfAllPages->next;
            freePageList(&links);

        }
    }
    
    log_message(LOG_DEBUG, "Finished searching all of the pages");
    log_message(LOG_DEBUG, "linkTrackerPage content: %s", linkTrackerPage->content);
    log_message(LOG_DEBUG, "about to update linkTrackerPage");


    updatePageContentMutation(linkTrackerPage);
    renderMutation(&linkTrackerPage, false);
    // Free the memory used by the linked list
    freePageList(&linkTrackerPage);// Free the memory used by the linked list
    freePageList(&listOfAllPages);// Free the memory used by the linked list

    log_message(LOG_DEBUG, "Exiting the buildLinksTracker function");
}