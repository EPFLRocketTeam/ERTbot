#include <stdbool.h>
#include "ERTbot_common.h"
#include "ERTbot_config.h"
#include "stringHelpers.h"
#include "wikiAPI.h"
#include "pageListHelpers.h"
#include "wikiLinksHelpers.h"

void movePage(command cmd){
    log_message(LOG_DEBUG, "Entering function movePage");
    
    cmd.argument_1 = replaceWord(cmd.argument_1, "\\", "");
    cmd.argument_2 = replaceWord(cmd.argument_2, "\\", "");

    pageList* subjectPage = NULL;
    subjectPage = populatePageList(&subjectPage, "exact path", cmd.argument_1);
    subjectPage->path = cmd.argument_2;
    movePageMutation(&subjectPage);

    pageList* linkTrackerPage = NULL;
    linkTrackerPage = addPageToList(&linkTrackerPage, LINK_TRACKER_PAGE_ID, "", "", "", "", "", "", "");
    log_message(LOG_DEBUG, "linkTrackerPage id set");
    linkTrackerPage = getPage(&linkTrackerPage);

    pageList* IncomingLinks = NULL;
    log_message(LOG_DEBUG, "cmd.argument_1:%s.\n", cmd.argument_1);
    IncomingLinks = findIncomingLinks(&IncomingLinks, linkTrackerPage->content, cmd.argument_1);

    char *originalPathWithParentheseAndSlash = createCombinedString("(/", cmd.argument_1);
    char *newPathWithParentheseAndSlash = createCombinedString("(/", cmd.argument_2);

    log_message(LOG_DEBUG, "Going to start modifying target pages");
    while(IncomingLinks != NULL){
        pageList* temporaryPage = NULL;
        temporaryPage = populatePageList(&temporaryPage, "exact path", IncomingLinks->path);
        temporaryPage = getPage(&temporaryPage);
        log_message(LOG_DEBUG, "Going to change %s's content", temporaryPage->path);
        temporaryPage->content = replaceWord(temporaryPage->content, originalPathWithParentheseAndSlash, newPathWithParentheseAndSlash);
        temporaryPage->content = replaceWord(temporaryPage->content, "\\", "\\\\");
        temporaryPage->content = replaceWord(temporaryPage->content, "\"", "\\\"");
        updatePageContentMutation(temporaryPage);
        log_message(LOG_DEBUG, "Updated Page");
        renderMutation(&temporaryPage, false);
        log_message(LOG_DEBUG, "Rendered Page");
        IncomingLinks = IncomingLinks->next;
        freePageList(&temporaryPage);
    }


    char *originalPathWithLineReturn = createCombinedString(cmd.argument_1, "\\n");
    char *newPathWithLineReturn = createCombinedString(cmd.argument_2, "\\n");

    linkTrackerPage->content = replaceWord(linkTrackerPage->content, originalPathWithParentheseAndSlash, newPathWithParentheseAndSlash);
    linkTrackerPage->content = replaceWord(linkTrackerPage->content, originalPathWithLineReturn, newPathWithLineReturn);

    linkTrackerPage->content = replaceWord(linkTrackerPage->content, "\\n", "\\\\n");
    updatePageContentMutation(linkTrackerPage);
    log_message(LOG_DEBUG, "Updated Page");
    renderMutation(&linkTrackerPage, false);
    log_message(LOG_DEBUG, "Rendered Page");

    freePageList(&subjectPage);
    freePageList(&linkTrackerPage);

    free(originalPathWithParentheseAndSlash);
    free(newPathWithParentheseAndSlash);
    free(originalPathWithLineReturn);
    free(newPathWithLineReturn);

    
    log_message(LOG_DEBUG, "Exiting function movePage");
    return;
}