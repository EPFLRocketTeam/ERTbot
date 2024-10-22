#include "common.h"
#include <stdlib.h>
#include <stdbool.h>
#include "config.h"

void appendMentionedIn(pageList** head){
    log_message(LOG_DEBUG, "Entering function appendMentionedIn");
    

    pageList* subjectPage = *head;

    pageList* linkTrackerPage = NULL;
    linkTrackerPage = addPageToList(&linkTrackerPage, LINK_TRACKER_PAGE_ID, "", "", "", "", "", "", "");
    log_message(LOG_DEBUG, "linkTrackerPage id set");
    linkTrackerPage = getPage(&linkTrackerPage);

    pageList* IncomingLinks = findIncomingLinks(&IncomingLinks, linkTrackerPage->content, subjectPage->path);

    subjectPage->content = appendToString(subjectPage->content, "\n# Mentiond in\n");
    while (IncomingLinks != NULL) {
        subjectPage->content = appendToString(subjectPage->content, "- [");
        subjectPage->content = appendToString(subjectPage->content, IncomingLinks->title);
        subjectPage->content = appendToString(subjectPage->content, "](");
        subjectPage->content = appendToString(subjectPage->content, IncomingLinks->path);
        subjectPage->content = appendToString(subjectPage->content, ")\n");
        IncomingLinks = IncomingLinks->next;
    }

    subjectPage->content = appendToString(subjectPage->content, "{.links-list}\n");

    subjectPage->content = replaceWord(subjectPage->content, "\n", "\\\\n");
    subjectPage->content = replaceWord(subjectPage->content, "\"", "\\\"");
    updatePageContentMutation(subjectPage);
    renderMutation(&subjectPage, false);
    
    log_message(LOG_DEBUG, "Exiting function appendMentionedIn");
    
    return;
}