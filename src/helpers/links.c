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

char* createList(char *list, pageList** sectionTitle, pageList* links){
    log_message(LOG_DEBUG, "Entering function createList");
    
    char *tempList = list;
    tempList = appendToString(tempList, "\\\\n\\\\n");
    tempList = appendToString(tempList, "## [");
    tempList = appendToString(tempList, (*sectionTitle)->title);
    tempList = appendToString(tempList, "](/");
    tempList = appendToString(tempList, (*sectionTitle)->path);
    tempList = appendToString(tempList, ")");
    //tempList = appendToString(tempList, "\\\\n");
    while (links != NULL) {
        tempList = appendToString(tempList, "\\\\n");
        tempList = appendToString(tempList, links->path);
        links = links->next;
    }

    freePageList(&links);

    log_message(LOG_DEBUG, "Exiting function createList");

    return tempList;
}

pageList* findPageLinks(char *content, pageList **links) {
    log_message(LOG_DEBUG, "Entering function findPageLinks");
    
    char *startFlag1 = "[";
    char *startFlag2 = "](";
    char *endFlag = ")";
    char *ptr = content;

    while (*ptr) {
        // Find the closest "]("
        char *linkStart = strstr(ptr, startFlag2);
        if (!linkStart) break; // No more links

        // Find the closest ")"
        char *linkEnd = strstr(linkStart, endFlag);
        if (!linkEnd) break; // No more links

        // Find the closest "["
        char *titleStart = linkStart;
        while (titleStart > content && *titleStart != '[') {
            titleStart--;
        }
        if (titleStart <= content) break; // No valid "[" found

        titleStart++; // Move past the '['

        // Calculate title length and link length
        int titleLength = linkStart - titleStart;
        int linkLength = linkEnd - (linkStart + strlen(startFlag2));

        if (titleLength <= 0 || linkLength <= 0) {
            ptr = linkEnd + strlen(endFlag);
            continue;
        }

        // Allocate memory for the title and link
        char *title = (char *)malloc((titleLength + 1) * sizeof(char));
        char *link = (char *)malloc((linkLength + 1) * sizeof(char));
        if (!title || !link) {
            log_message(LOG_ERROR, "Memory allocation error");
            if (title) free(title);
            if (link) free(link);
            freePageList(links);
            return *links;
        }

        // Copy title and link
        strncpy(title, titleStart, titleLength);
        title[titleLength] = '\0';
        strncpy(link, linkStart + strlen(startFlag2), linkLength);
        link[linkLength] = '\0';

        // Check if link is valid (you might need to refine this check)
        if (strstr(link, ".") == NULL) {
            *links = addPageToList(links, "", title, link, "", "", "", "", "");
            log_message(LOG_DEBUG, "found link %s %s", link, title);
        }

        free(title);
        free(link);
        ptr = linkEnd + strlen(endFlag);
    }

    
    log_message(LOG_DEBUG, "Exiting function findPageLinks");
    return *links;
}

pageList* findImageLinks(char *input, pageList** head) {
    log_message(LOG_DEBUG, "Entering function findImageLinks");
    
    pageList* imageLinks = *head;
    int count = 0;
    char *startFlag1 = "![";
    char *startFlag2 = "](";
    char *endFlag = ")";
    char *ptr = input;

    while (*ptr) {
        ptr = strstr(ptr, startFlag1);
        if (!ptr) break;

        ptr += strlen(startFlag1);
        char *linkStart = strstr(ptr, startFlag2);
        if (!linkStart) {
            log_message(LOG_ERROR, "Error: Missing startFlag2 after startFlag1");
            freePageList(&imageLinks);
            return imageLinks;
        }

        linkStart += strlen(startFlag2);
        char *linkEnd = strstr(linkStart, endFlag);
        if (!linkEnd) {
            log_message(LOG_ERROR, "Error: Missing endFlag after linkStart");
            freePageList(&imageLinks);
            return imageLinks;
        }

        // Allocate memory for the link substring
        int linkLen = linkEnd - linkStart;
        char *link = (char *)malloc((linkLen + 1) * sizeof(char));
        if (!link) {
            log_message(LOG_ERROR, "Memory allocation error");
            freePageList(&imageLinks);
            return imageLinks;
        }

        strncpy(link, linkStart, linkLen);
        link[linkLen] = '\0';
        imageLinks = addPageToList(&imageLinks, link, "", "", "", "", "", "", "");

        count++ ;

        ptr = linkEnd + strlen(endFlag);
    }
    
    log_message(LOG_DEBUG, "Exiting function findImageLinks");
    return imageLinks;
}

pageList* findIncomingLinks(pageList** head, char *linkTrackerContent, char *subjectPagePath) {
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
