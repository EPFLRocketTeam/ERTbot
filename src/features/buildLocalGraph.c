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

char* buildLocalGraph(command cmd) {
    log_message(LOG_DEBUG, "Entering function buildLocalGraph");
    
    // Allocate memory for linkTrackerPage
    pageList* linkTrackerPage = NULL;
    linkTrackerPage = addPageToList(&linkTrackerPage, LINK_TRACKER_PAGE_ID, "", "", "", "", "", "", "");
    log_message(LOG_DEBUG, "linkTrackerPage id set");
    linkTrackerPage = getPage(&linkTrackerPage);

    pageList* subjectPage = NULL;

    subjectPage = populatePageList(&subjectPage, "exact path", cmd.argument_1);

    pageList* IncomingLinks;
    pageList* OutgoingLinks;

    IncomingLinks = findIncomingLinks(&IncomingLinks, linkTrackerPage->content, subjectPage->path);
    OutgoingLinks = findOutgoingLinks(&OutgoingLinks, linkTrackerPage->content, subjectPage->path);

    char *tempGraph = createLocalGraphMindMap(&subjectPage, &IncomingLinks, &OutgoingLinks);

    return tempGraph;
    
    log_message(LOG_DEBUG, "Exiting function buildLocalGraph");
}

static pageList* findOutgoingLinks(pageList** head, char *linkTrackerContent, char *subjectPagePath) {
    log_message(LOG_DEBUG, "Entering function findOutgoingLinks");
    
    pageList *outgoingLinks = *head;
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

        *titleEnd = '\0';
        *pathEnd = '\0';

        
        if(strcmp(pathStart, subjectPagePath) == 0){
            char *link = strstr(linksStart+1, "n/");
        
            while (link != NULL && link <= nextSectionStart) {
                link += 2;
                char *linkEnd = strstr(link, "\\n");
                *linkEnd = '\0';
                if (linkEnd == NULL) {
                    linkEnd = contentCopy + strlen(contentCopy);
                }
                outgoingLinks = addPageToList(&outgoingLinks, "", "", link, "", "", "", "", "");
                link = strstr(linkEnd + 1, "n/");

            }
        
            break;
        
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
    
    log_message(LOG_DEBUG, "Exiting function findOutgoingLinks");
    return outgoingLinks;
}

static char* createLocalGraphMindMap(pageList** tempPage, pageList** incomingPaths, pageList** outgoingPaths){
    log_message(LOG_DEBUG, "Entering function createLocalGraphMindMap");
    
    pageList *currentIncomingLink = *incomingPaths;
    pageList *currentOutgoingLink = *outgoingPaths;
    char *localGraph;
    localGraph = "```plantuml\n@startmindmap\n<style>\nmindmapDiagram {\n  .myStyle*{\n Linecolor black\n	BackGroundColor white\n  hyperlinkColor black\n}\n}\n</style>";;
    localGraph = appendToString(localGraph, "\n+");
    localGraph = appendToString(localGraph, " [[https://rocket-team.epfl.ch/en/");
    localGraph = appendToString(localGraph, (*tempPage)->path);
    localGraph = appendToString(localGraph, " ");
    localGraph = appendToString(localGraph, (*tempPage)->title);
    localGraph = appendToString(localGraph, "]]\n");

    while (currentIncomingLink != NULL) {
        localGraph = appendToString(localGraph, "\n--");
        localGraph = appendToString(localGraph, " [[https://rocket-team.epfl.ch/en/");
        localGraph = appendToString(localGraph, currentIncomingLink->path);
        localGraph = appendToString(localGraph, " ");
        localGraph = appendToString(localGraph, currentIncomingLink->title);
        localGraph = appendToString(localGraph, "]]\n");
        currentIncomingLink = currentIncomingLink->next;

    }

    currentOutgoingLink = currentOutgoingLink->next;

    while(currentOutgoingLink != NULL){
        localGraph = appendToString(localGraph, "\n++");
        localGraph = appendToString(localGraph, " [[https://rocket-team.epfl.ch/en/");
        localGraph = appendToString(localGraph, currentOutgoingLink->path);
        localGraph = appendToString(localGraph, " ");
        localGraph = appendToString(localGraph, getDocId(currentOutgoingLink->path));
        localGraph = appendToString(localGraph, "]]\n");
        currentOutgoingLink = currentOutgoingLink->next;
    }

    localGraph = appendToString(localGraph, "\n\n\n@endmindmap\n```\n\n\n");

    freePageList(&currentIncomingLink);
    freePageList(&currentOutgoingLink);
    
    log_message(LOG_DEBUG, "Exiting function createLocalGraphMindMap");
    return localGraph;
}
