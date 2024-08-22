/**
 * @file features.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief Contains all of the functions which represent a single feature
 * 
 * @todo - Add "onPageUpdateFeature"
 */

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

char* buildMap(command cmd) {
    log_message(LOG_DEBUG, "Entering function buildMap");

    pageList* listOfDaughterPages = NULL;
    listOfDaughterPages = populatePageList(&listOfDaughterPages, "path",  cmd.argument_1); // Parse JSON and populate linked list
    sendMessageToSlack("Finished gathering pages");
    char *map = createMapWBS(&listOfDaughterPages);
    freePageList(&listOfDaughterPages); // Free the memory used by the linked list

    log_message(LOG_DEBUG, "Exiting the buildMap function");
    return map;
}

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
    renderMutation(&linkTrackerPage);
    // Free the memory used by the linked list
    freePageList(&linkTrackerPage);// Free the memory used by the linked list
    freePageList(&listOfAllPages);// Free the memory used by the linked list

    log_message(LOG_DEBUG, "Exiting the buildLinksTracker function");
}

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
    renderMutation(&linkTrackerPage);

    // Free the memory used by the linked list
    freePageList(&linkTrackerPage);// Free the memory used by the linked list
    //freePageList(&listOfAllPages);// Free the memory used by the linked list
    
    log_message(LOG_DEBUG, "Exiting function updateLinksTracker");
}

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

/*
//Finds all of the image links on the wiki and saves the to IMAGE_TRACKER_PAGE_ID
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
    renderMutation(&imageTrackerPage);
    // Free the memory used by the linked list
    freePageList(&imageTrackerPage);// Free the memory used by the linked list
    freePageList(&listOfAllPages);// Free the memory used by the linked list
    freePageList(&current);// Free the memory used by the linked list
}
*/

void buildAcronymsList(command cmd) {
    log_message(LOG_DEBUG, "Entering function buildAcronymsList");
    
    pageList* listOfAllPages;
    if (cmd.argument_1 == NULL){
        listOfAllPages = populatePageList(&listOfAllPages, "path", "none");
    }
    else{listOfAllPages = populatePageList(&listOfAllPages, "path", cmd.argument_1);}

    //Watch out maybe getPage whould not be done all in one go ?
    listOfAllPages = getPage(&listOfAllPages);
    pageList* current = listOfAllPages;
    listOfAllPages = getPage(&listOfAllPages);
    while (current != NULL) {
        printAcronymsToFile(ACCRONYM_LIST_PATH, current->content);
        current = current->next;
    }
    freePageList(&listOfAllPages);
    freePageList(&current);
    removeDuplicatesAndSort(ACCRONYM_LIST_PATH);
    
    log_message(LOG_DEBUG, "Exiting function buildAcronymsList");
}

void getPages(command cmd) {
    log_message(LOG_DEBUG, "Entering function getPages");
    
    pageList* head = NULL;
    head = populatePageList(&head, "path", cmd.argument_1); // Parse JSON and populate linked list
    printPages(&head);
    sendMessageToSlack("All Pages Printed\n");
    // Free the memory used by the linked list
    freePageList(&head);

    log_message(LOG_DEBUG, "Entering function getPages");
    
    return; 
}

// Mass replace all strings
void replaceText(command cmd) {
    log_message(LOG_DEBUG, "Entering function replaceText");
    
    pageList* head;
    head = populatePageList(&head, "path", cmd.argument_1); // Parse JSON and populate linked list
    replaceStringInWiki(&head, cmd.argument_2,cmd.argument_3);
    // Free the memory used by the linked list
    freePageList(&head);
    
    log_message(LOG_DEBUG, "Exiting function replaceText");
}

void movePage(command cmd){
    log_message(LOG_DEBUG, "Entering function movePage");
    
    cmd.argument_1 = replaceWord(cmd.argument_1, "\\", "");
    cmd.argument_2 = replaceWord(cmd.argument_2, "\\", "");

    pageList* subjectPage = NULL;
    subjectPage = populatePageList(&subjectPage, "exact path", cmd.argument_1);
    subjectPage->path = cmd.argument_2;
    movePageContentMutation(&subjectPage);

    pageList* linkTrackerPage = NULL;
    linkTrackerPage = addPageToList(&linkTrackerPage, LINK_TRACKER_PAGE_ID, "", "", "", "", "", "", "");
    log_message(LOG_DEBUG, "linkTrackerPage id set");
    linkTrackerPage = getPage(&linkTrackerPage);

    pageList* IncomingLinks = NULL;
    log_message(LOG_DEBUG, "cmd.argument_1:%s.\n", cmd.argument_1);
    IncomingLinks = findIncomingLinks(&IncomingLinks, linkTrackerPage->content, cmd.argument_1);

    char *originalPathWithParentheseAndSlash = appendStrings("(/", cmd.argument_1);
    char *newPathWithParentheseAndSlash = appendStrings("(/", cmd.argument_2);

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
        renderMutation(&temporaryPage);
        log_message(LOG_DEBUG, "Rendered Page");
        IncomingLinks = IncomingLinks->next;
        freePageList(&temporaryPage);
    }


    char *originalPathWithLineReturn = appendStrings(cmd.argument_1, "\\n");
    char *newPathWithLineReturn = appendStrings(cmd.argument_2, "\\n");

    linkTrackerPage->content = replaceWord(linkTrackerPage->content, originalPathWithParentheseAndSlash, newPathWithParentheseAndSlash);
    linkTrackerPage->content = replaceWord(linkTrackerPage->content, originalPathWithLineReturn, newPathWithLineReturn);

    linkTrackerPage->content = replaceWord(linkTrackerPage->content, "\\n", "\\\\n");
    updatePageContentMutation(linkTrackerPage);
    log_message(LOG_DEBUG, "Updated Page");
    renderMutation(&linkTrackerPage);
    log_message(LOG_DEBUG, "Rendered Page");

    freePageList(&subjectPage);
    freePageList(&linkTrackerPage);

    
    log_message(LOG_DEBUG, "Exiting function movePage");
    return;
}

void syncSheetToDrl(command cmd){
    log_message(LOG_DEBUG, "Entering function syncSheetToDRL");
    
    pageList* drlPage = (pageList*) malloc(sizeof(pageList)); // Allocate memory for linkTrackerPage
    if (drlPage == NULL) {
        log_message(LOG_ERROR, "Memory allocation failed for drlPage");
        return;
    }

    drlPage->id = TEST_DRL_PAGE_ID; //Initialise linkTrackerPage
    drlPage = getPage(&drlPage); //get content and updated at values

    cJSON *requirementList = cJSON_CreateObject();

    cJSON *documentName = NULL;
    documentName = cJSON_CreateString(getDocId(drlPage->path));
    cJSON_AddItemToObject(requirementList, "documentName", documentName);

    cJSON *requirements = NULL;
    requirements = cJSON_CreateArray();
    cJSON_AddItemToObject(requirementList, "requirements", requirements);


    parseRequirementsList(requirements, drlPage->content);

    /*
    char *stringTest = NULL;
    stringTest = cJSON_Print(requirementList);
    if (stringTest == NULL){
        fprintf(stderr, "Failed to print monitor.\n");
    }
    stringTest = replaceWord(stringTest, "\"", "\\\"");
    sendMessageToSlack(stringTest);
    */

    char* output = parseJSONRequirementListInToArray(requirements);

    batchUpdateSheet("14vOyP1Oc5O_7JY1vnY7pQPTJoOB8oi4nNRMEsodIvtU", "Sheet1!A1:D36", output);

    log_message(LOG_DEBUG, "deallocating memory");
    cJSON_Delete(requirementList);
    free(output);
    
    log_message(LOG_DEBUG, "Exiting function syncSheetToDRL");
    return;
}

void syncDrlToSheet(command cmd){
    log_message(LOG_DEBUG, "Entering function syncDrlToSheet");
    
    batchGetSheet("14vOyP1Oc5O_7JY1vnY7pQPTJoOB8oi4nNRMEsodIvtU", "NewVersion!A4:AI67");
    log_message(LOG_DEBUG, "chunk.response: %s", chunk.response);

    cJSON *requirementList = parseArrayIntoJSONRequirementList(chunk.response); 
    char *DRL = buildDrlFromJSONRequirementList(requirementList);

    pageList* drlPage = NULL;
    drlPage = addPageToList(&drlPage, TEST_DRL_PAGE_ID, "", "", "", "", "", "", "");
    drlPage->content = DRL;

    drlPage->content = replaceWord(drlPage->content, "\n", "\\\\n");
    drlPage->content = replaceWord(drlPage->content, "\"", "\\\"");

    updatePageContentMutation(drlPage);
    renderMutation(&drlPage);

    freePageList(&drlPage);
    cJSON_Delete(requirementList);
    free(DRL);
    
    log_message(LOG_DEBUG, "Exiting function syncDrlToSheet");
    return;
}

void createRequirementPage(command cmd){
    log_message(LOG_DEBUG, "Entering function createRequirementPages");

    batchGetSheet("14vOyP1Oc5O_7JY1vnY7pQPTJoOB8oi4nNRMEsodIvtU", "NewVersion!A4:AI67");
    log_message(LOG_DEBUG, "chunk.response: %s", chunk.response);
    cJSON *requirementList = parseArrayIntoJSONRequirementList(chunk.response);
    pageList* reqPage = buildRequirementPageFromJSONRequirementList(requirementList, cmd.argument_1);
    reqPage->content = replaceWord(reqPage->content, "\n", "\\\\n");
    reqPage->content = replaceWord(reqPage->content, "\"", "\\\"");
    updatePageContentMutation(reqPage);
    renderMutation(&reqPage);
    cJSON_Delete(requirementList);
    freePageList(&reqPage);
    
    log_message(LOG_DEBUG, "Exiting function createRequirementPage");
    return;
}

void createVcdPage(command cmd){
    log_message(LOG_DEBUG, "Entering function createVcdPage");
    
    batchGetSheet("14vOyP1Oc5O_7JY1vnY7pQPTJoOB8oi4nNRMEsodIvtU", "NewVersion!A4:AI67");
    log_message(LOG_DEBUG, "chunk.response: %s", chunk.response);
    cJSON *requirementList = parseArrayIntoJSONRequirementList(chunk.response);

    char *pieChart = createVcdPieChart("20", "30", "40");
    
    pageList* C_ST_VCD_DRAFT = NULL;
    C_ST_VCD_DRAFT = addPageToList(&C_ST_VCD_DRAFT, "1130", "", "", "", "", "", "", "");
    getPage(&C_ST_VCD_DRAFT);

    char *extractedText = extractText(C_ST_VCD_DRAFT->content, "<!-- Status history -->\\n```kroki\\nvegalite\\n", "\\n```\\n", false, false);

    extractedText = replaceWord(extractedText, "\\n", "\n");
    extractedText = replaceWord(extractedText, "\\\"", "\"");


    char *stackedAreaChart = updateVcdStackedAreaChart(extractedText, "20/24", 33, 33, 34);    
    stackedAreaChart = appendStrings("<!-- Status history -->\n```kroki\nvegalite\n", stackedAreaChart);
    stackedAreaChart = appendStrings(stackedAreaChart, "\n```\n");
    fprintf(stderr, "\n\n%s\n\n", stackedAreaChart);
    free(stackedAreaChart);
    
    
    //free(extractedText);

    cJSON_Delete(requirementList);
    freePageList(&C_ST_VCD_DRAFT);
    return;
    
    log_message(LOG_DEBUG, "Exiting function createVcdPage");
}

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
            map = appendStrings("\n", map);
            targetPage->content = replaceParagraph(targetPage->content, map, wikiCommand->pointerToEndOfFirstMarker, wikiCommand->pointerToBeginningOfSecondMarker);

            wasPageModified = 1;
        }

        log_message(LOG_DEBUG, "going to send message to slack");
        

        wikiCommand = wikiCommand->next;
    }

    if(wasPageModified){
        targetPage->content = replaceWord(targetPage->content, "\n", "\\n");
        targetPage->content = replaceWord(targetPage->content, "\\", "\\\\");
        targetPage->content = replaceWord(targetPage->content, "\"", "\\\"");
        updatePageContentMutation(targetPage);
        renderMutation(&targetPage);
        freePageList(&targetPage);
        sendMessageToSlack("onPageUpdate called on page id:");
        sendMessageToSlack(cmd.argument_1);
    }
    
    log_message(LOG_DEBUG, "Exiting function onPageUpdate");
}