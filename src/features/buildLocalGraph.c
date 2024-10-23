#include <string.h>
#include "ERTbot_common.h"
#include "ERTbot_config.h"
#include "pageListHelpers.h"
#include "wikiAPI.h"
#include "wikiLinksHelpers.h"
#include "stringHelpers.h"

/**
 * @brief Creates a PlantUML mind map representation of a page's "local" graph, which is a graph which shows all of
 *        the incoming and outgoing links.
 * 
 * This function generates a PlantUML mind map diagram for a given page. It includes the page itself, along with its 
 * incoming and outgoing links. The diagram is formatted using PlantUML syntax and includes links with associated titles 
 * and paths.
 * 
 * @param tempPage A pointer to a `pageList` structure representing the central page for which the mind map is created.
 * @param incomingPaths A pointer to a linked list of `pageList` structures representing the pages that link to the central page.
 * @param outgoingPaths A pointer to a linked list of `pageList` structures representing the pages linked from the central page.
 * 
 * @return A dynamically allocated string containing the PlantUML mind map diagram.
 * 
 * @details The function constructs the PlantUML mind map starting with the central page, then iterates through the incoming 
 *          and outgoing link lists to add corresponding entries. The mind map uses "+" for the central node, "--" for incoming 
 *          links, and "++" for outgoing links. Each link is represented with its path and title. The function also handles memory 
 *          cleanup for the incoming and outgoing link lists after generating the diagram.
 * 
 * @todo rewrite with new cJSON structured link tracker
 */
static char* createLocalGraphMindMap(pageList** tempPage, pageList** incomingPaths, pageList** outgoingPaths);

/**
 * @brief Finds and adds outgoing links to a linked list. The links must be in a specific subject page's content.
 * 
 * This function searches through the provided content to find sections where the path matches the `subjectPagePath`. It
 * extracts links from these sections and adds them to the `outgoingLinks` list. The function processes a copy of the
 * content to avoid modifying the original content.
 * 
 * @param head A pointer to a pointer to the head of the linked list where the outgoing links will be added.
 * @param linkTrackerContent A string containing the content to be searched for outgoing links.
 * @param subjectPagePath The path of the subject page to be matched in the content.
 * 
 * @return A pointer to the updated list of outgoing links, which includes any newly found links that reference the
 *         `subjectPagePath`.
 * 
 * @details The function performs the following steps:
 *          - Creates a copy of the `linkTrackerContent` to process.
 *          - Searches for sections in the content that start with "## [".
 *          - Extracts titles, paths, and links from these sections.
 *          - If the path matches `subjectPagePath`, it extracts and adds the links found in that section to the
 *            `outgoingLinks` list.
 *          - Updates the pointers to continue searching through the content.
 *          - Frees the allocated memory for the content copy before returning.
 * 
 * @todo replace with a cJSON version
 */
static pageList* findOutgoingLinks(pageList **head, char *linkTrackerContent, char *subjectPagePath);

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
