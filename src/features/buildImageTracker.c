#include <stdbool.h>
#include "ERTbot_common.h"
#include "ERTbot_config.h"
#include "wikiAPI.h"
#include "pageListHelpers.h"
#include "wikiLinksHelpers.h"

void buildImageTracker(command cmd) {
    pageList* imageTrackerPage;
    imageTrackerPage->content = "";
    imageTrackerPage->id = IMAGE_TRACKER_PAGE_ID;
    
    pageList* listOfAllPages;
    listOfAllPages = populatePageList(&listOfAllPages, "path", "none"); // Parse JSON and populate linked list

    pageList* current = listOfAllPages;
    while (current != NULL) {
        pageList* links;
        links = findImageLinks(current->content, &links);
        imageTrackerPage->content = createList(imageTrackerPage->content, &current, &links);
        current = current->next;
        freePageList(&links);
    } 
    updatePageContentMutation(&imageTrackerPage);
    renderMutation(&imageTrackerPage, false);
    // Free the memory used by the linked list
    freePageList(&imageTrackerPage);// Free the memory used by the linked list
    freePageList(&listOfAllPages);// Free the memory used by the linked list
    freePageList(&current);// Free the memory used by the linked list
}