#include <string.h>
#include "ERTbot_common.h"
#include "pageListHelpers.h"
#include "stringHelpers.h"

char* createList(char *list, pageList** sectionTitle, pageList* links){
    log_message(LOG_DEBUG, "Entering function createList");

    char *tempList = list;
    tempList = appendToString(tempList, "\\\\n\\\\n");
    tempList = appendToString(tempList, "## [");
    tempList = appendToString(tempList, (*sectionTitle)->title);
    tempList = appendToString(tempList, "](/");
    tempList = appendToString(tempList, (*sectionTitle)->path);
    tempList = appendToString(tempList, ")");
    while (links != NULL) {
        tempList = appendToString(tempList, "\\\\n");
        tempList = appendToString(tempList, links->path);
        links = links->next;
    }

    freePageList(&links);

    log_message(LOG_DEBUG, "Exiting function createList");

    return tempList;
}


pageList* findIncomingLinks(pageList** head, const char *linkTrackerContent, const char *subjectPagePath) {
    log_message(LOG_DEBUG, "Entering function findIncomingLinks");

    pageList *incomingLinks = *head;
    char *contentCopy = strdup(linkTrackerContent);
    if (contentCopy == NULL) {
        log_message(LOG_ERROR, "Memory allocation failed");
        exit(1);
    }

    char *sectionStart = contentCopy;
    sectionStart = strstr(sectionStart, "## [");
    sectionStart += 4;
    char *nextSectionStart = strstr(sectionStart, "## [");

    while (sectionStart != NULL) {
        char *titleStart = sectionStart + 4;
        char *titleEnd = strstr(titleStart, "](/");
        if (titleEnd == NULL) break;

        char *pathStart = titleEnd + 3;

        char *pathEnd = strstr(pathStart, ")");
        if (pathEnd == NULL) break;

        char *linksStart = pathEnd;
        char *link = strstr(linksStart, "\\n/");

        while (linksStart != NULL && link != NULL && link <= nextSectionStart) {
            link += 3;
            char *linkEnd = strstr(link, "\\n");
            *linkEnd = '\0';

            if (linkEnd == NULL) {
                linkEnd = contentCopy + strlen(contentCopy);
            }

            if (strcmp(link, subjectPagePath) == 0) {
                *titleEnd = '\0';
                *pathEnd = '\0';
                log_message(LOG_DEBUG, "Found a reference in title: %s, path:%s\n", titleStart, pathStart);
                incomingLinks = addPageToList(&incomingLinks, "", titleStart, pathStart, "", "", "", "", "");
                break;
            }
            link = strstr(linkEnd + 1, "\\n/");
            linksStart = strstr(linksStart, "\\n/");

        }

        sectionStart = nextSectionStart;

        if(nextSectionStart != NULL){
            nextSectionStart = strstr(nextSectionStart + 1, "## [");
        }

        else{
            break;
        }


    }

    free(contentCopy);

    log_message(LOG_DEBUG, "Exiting function findIncomingLinks");
    return incomingLinks;
}
