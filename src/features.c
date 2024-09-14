/**
 * @file features.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief Contains all of the functions which represent a single feature
 * 
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
#include "../include/requirements.h"


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
    renderMutation(&linkTrackerPage, false);
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
    renderMutation(&linkTrackerPage, false);

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
    renderMutation(&imageTrackerPage, false);
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

void syncSheetToDrl(command cmd){
    log_message(LOG_DEBUG, "Entering function syncSheetToDRL");

    refreshOAuthToken();

    pageList* drlPage = (pageList*) malloc(sizeof(pageList)); // Allocate memory for linkTrackerPage
    if (drlPage == NULL) {
        log_message(LOG_ERROR, "Memory allocation failed for drlPage");
        return;
    }
    char *sheetId;

    if(strcmp(cmd.argument_1, "ST")==0){
        drlPage->id = "420";
        sheetId = "ST!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "PR")==0){
        drlPage->id = "414";
        sheetId = "PR!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "FD")==0){
        drlPage->id = "416";
        sheetId = "FD!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "RE")==0){
        drlPage->id = "419";
        sheetId = "RE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "GS")==0){
        drlPage->id = "417";
        sheetId = "GS!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "AV")==0){
        drlPage->id = "421";
        sheetId = "AV!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "TE")==0){
        drlPage->id = 
        sheetId = "TE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "PL")==0){
        drlPage->id = "418";
        sheetId = "PL!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "GE")==0){
        drlPage->id = "415";
        sheetId = "GE!A3:AT300";
    }



    drlPage = getPage(&drlPage); //get content and updated at values

    cJSON *requirementList = cJSON_CreateObject();

    cJSON *documentName = NULL;
    documentName = cJSON_CreateString(getDocId(drlPage->path));
    cJSON_AddItemToObject(requirementList, "documentName", documentName);

    cJSON *requirements = NULL;
    requirements = cJSON_CreateArray();
    cJSON_AddItemToObject(requirementList, "requirements", requirements);

    parseRequirementsList(requirements, drlPage->content);

    char* output = parseJSONRequirementListInToArray(requirements);

    log_message(LOG_DEBUG, "calling batchUpdateSheet with values set to: %s", output);

    batchUpdateSheet("1i_PTwIqLuG9IUI73UaGuOvx8rVTDV1zIS7gmXNjMs1I", sheetId, output);
    
    log_message(LOG_DEBUG, "deallocating memory");
    cJSON_Delete(requirementList);
    free(output);
    
    log_message(LOG_DEBUG, "Exiting function syncSheetToDRL");
    return;
}

void syncDrlToSheet(command cmd){
    log_message(LOG_DEBUG, "Entering function syncDrlToSheet");

    refreshOAuthToken();

    char *sheetId;
    char *drlPageId;

    if(strcmp(cmd.argument_1, "ST")==0){
        drlPageId = "420";
        sheetId = "ST!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "PR")==0){
        drlPageId = "414";
        sheetId = "PR!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "FD")==0){
        drlPageId = "416";
        sheetId = "FD!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "RE")==0){
        drlPageId = "419";
        sheetId = "RE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "GS")==0){
        drlPageId = "417";
        sheetId = "GS!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "AV")==0){
        drlPageId = "421";
        sheetId = "AV!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "TE")==0){
        drlPageId = 
        sheetId = "TE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "PL")==0){
        drlPageId = "418";
        sheetId = "PL!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "GE")==0){
        drlPageId = "415";
        sheetId = "GE!A3:AT300";
    }
    
    batchGetSheet("1i_PTwIqLuG9IUI73UaGuOvx8rVTDV1zIS7gmXNjMs1I", sheetId);

    cJSON *requirementList = parseArrayIntoJSONRequirementList(chunk.response); 
    char *DRL = buildDrlFromJSONRequirementList(requirementList, cmd.argument_1);

    pageList* drlPage = NULL;
    drlPage = addPageToList(&drlPage, drlPageId, "", "", "", "", "", "", "");
    drlPage->content = DRL;


    drlPage->content = replaceWord(drlPage->content, "\n", "\\\\n");
    drlPage->content = replaceWord(drlPage->content, "\"", "\\\\\\\"");

    updatePageContentMutation(drlPage);
    renderMutation(&drlPage, false);

    freePageList(&drlPage);
    cJSON_Delete(requirementList);
    free(DRL);
    
    log_message(LOG_DEBUG, "Exiting function syncDrlToSheet");
    return;
}

void updateVcdPage(command cmd){
    log_message(LOG_DEBUG, "Entering function updateVcdPage");
    
    refreshOAuthToken();

    char *sheetId;
    char *vcdPageId;

    if(strcmp(cmd.argument_1, "ST")==0){
        vcdPageId = "1186";
        sheetId = "ST!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "PR")==0){
        vcdPageId = "1187";
        sheetId = "PR!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "FD")==0){
        vcdPageId = "1182";
        sheetId = "FD!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "RE")==0){
        vcdPageId = "1185";
        sheetId = "RE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "GS")==0){
        vcdPageId = "1183";
        sheetId = "GS!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "AV")==0){
        vcdPageId = "1181";
        sheetId = "AV!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "TE")==0){
        vcdPageId = 
        sheetId = "TE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "PL")==0){
        vcdPageId = "1184";
        sheetId = "PL!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "GE")==0){
        vcdPageId = "1180";
        sheetId = "GE!A3:AT300";
    }

    batchGetSheet("1i_PTwIqLuG9IUI73UaGuOvx8rVTDV1zIS7gmXNjMs1I", sheetId);
    log_message(LOG_DEBUG, "chunk.response: %s", chunk.response);
    cJSON *requirementList = parseArrayIntoJSONRequirementList(chunk.response);

    int verificationStatusCount[3];
    countVerificationStatus(requirementList, verificationStatusCount);
    char *pieChart = createVcdPieChart(verificationStatusCount);

    char *VCD = pieChart;
    char *listOfRequirements = buildVcdList(requirementList, cmd.argument_1);
    VCD = appendToString(VCD, listOfRequirements);
    
    free(listOfRequirements);

    pageList* vcdPage = NULL;
    vcdPage = addPageToList(&vcdPage, vcdPageId, "", "", "", VCD, "", "", "");

    vcdPage->content = replaceWord(vcdPage->content, "\n", "\\\\n");
    vcdPage->content = replaceWord(vcdPage->content, "\"", "\\\\\\\"");

    updatePageContentMutation(vcdPage);
    renderMutation(&vcdPage, false);

    /* Stacked Area chart
    char *extractedText = extractText(vcdPage->content, "<!-- Status history -->\\n```kroki\\nvegalite\\n", "\\n```\\n", false, false);
    extractedText = replaceWord(extractedText, "\\n", "\n");
    extractedText = replaceWord(extractedText, "\\\"", "\"");
    char *stackedAreaChart = updateVcdStackedAreaChart(extractedText, "20/24", 33, 33, 34);    
    stackedAreaChart = createCombinedString("<!-- Status history -->\n```kroki\nvegalite\n", stackedAreaChart);
    stackedAreaChart = appendToString(stackedAreaChart, "\n```\n");
    fprintf(stderr, "\n\n%s\n\n", stackedAreaChart);
    free(stackedAreaChart);
    //free(extractedText);
    */
    
    
    cJSON_Delete(requirementList);
    freePageList(&vcdPage);

    free(VCD);

    log_message(LOG_DEBUG, "Exiting function updateVcdPage");
    return;
}

void updateRequirementPage(command cmd){
    log_message(LOG_DEBUG, "Entering function updateRequirementPages");

    refreshOAuthToken();
    char *sheetId;
    pageList* requirementPagesHead = NULL;
    char *path = cmd.argument_1;
    path = replaceWord(path, "\\", "");


    
    if(strstr(cmd.argument_1, "ST")){
        sheetId = "ST!A3:AT300";
        if(strcmp(cmd.argument_1, "ST")==0){
            path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_ST_DRL/";
        }
    }
    
    if(strstr(cmd.argument_1, "PR")){
        sheetId = "PR!A3:AT300";
        if(strcmp(cmd.argument_1, "PR")==0){
            path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/";
        }
    }
    
    if(strstr(cmd.argument_1, "FD")){
        sheetId = "FD!A3:AT300";
        if(strcmp(cmd.argument_1, "FD")==0){
            path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_FD_DRL/";
        }
    }
    
    if(strstr(cmd.argument_1, "RE")){
        sheetId = "RE!A3:AT300";
        if(strcmp(cmd.argument_1, "RE")==0){
            path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_RE_DRL/";
        }
    }
    
    if(strstr(cmd.argument_1, "GS")){
        sheetId = "GS!A3:AT300";
        if(strcmp(cmd.argument_1, "GS")==0){
            path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_GS_DRL/";
        }
    }
    
    if(strstr(cmd.argument_1, "AV")){
        sheetId = "AV!A3:AT300";
        if(strcmp(cmd.argument_1, "AV")==0){
            path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_AV_DRL/";
        }
    }
    
    if(strstr(cmd.argument_1, "TE")){
        sheetId = "TE!A3:AT300";
        if(strcmp(cmd.argument_1, "TE")==0){
            path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_TE_DRL/";
        }
    }
    
    if(strstr(cmd.argument_1, "PL")){
        sheetId = "PL!A3:AT300";
        if(strcmp(cmd.argument_1, "PL")==0){
            path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PL_DRL/";
        }
    }
    
    if(strstr(cmd.argument_1, "GE")){
        sheetId = "GE!A3:AT300";
        if(strcmp(cmd.argument_1, "GE")==0){
            path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_GE_DRL/";
        }
    }

    requirementPagesHead = populatePageList(&requirementPagesHead, "path", path);


    batchGetSheet("1i_PTwIqLuG9IUI73UaGuOvx8rVTDV1zIS7gmXNjMs1I", sheetId);
    //log_message(LOG_DEBUG, "chunk.response: %s", chunk.response);
    
    cJSON *requirementList = parseArrayIntoJSONRequirementList(chunk.response);

    // Get the requirements array from the requirementList object
    cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");
    if (!cJSON_IsArray(requirements)) {
        log_message(LOG_ERROR, "Error: requirements is not a JSON array");
    }

    pageList* currentReqPage = requirementPagesHead;

    // Iterate over each requirement object in the requirements array
    int num_reqs = cJSON_GetArraySize(requirements);

    while (currentReqPage){
        for (int i = 0; i < num_reqs; i++) {
            cJSON *requirement = cJSON_GetArrayItem(requirements, i);

            if (!cJSON_IsObject(requirement)) {
                log_message(LOG_ERROR, "Error: requirement is not a JSON object");
                continue;
            }

            // Get and print each item of the requirement object
            cJSON *id = cJSON_GetObjectItemCaseSensitive(requirement, "ID");

            if (cJSON_IsString(id) && id->valuestring && strcmp(id->valuestring, currentReqPage->title) == 0){
                currentReqPage = getPage(&currentReqPage);
                currentReqPage->content = replaceWord(currentReqPage->content, "\\n", "\n");
                char* importedRequirementInformation = buildRequirementPageFromJSONRequirementList(requirement);
                char* flag = "<!--";
                flag = appendToString(flag, id->valuestring);
                flag = appendToString(flag, "-->");

                log_message(LOG_DEBUG, "initialising pointer to flags");
                char* start = currentReqPage->content;
                char* end;

                log_message(LOG_DEBUG, "Looking for flag: %s in currentReqPage->content: %s", flag, currentReqPage->content);
                start = strstr(start, flag);
                start = start + strlen(flag);

                log_message(LOG_DEBUG, "Looking for flag: %s in start+1: %s", flag, start+1);
                end = strstr(start + 1, flag);
                end--;
                
                currentReqPage->content = replaceParagraph(currentReqPage->content, importedRequirementInformation, start, end);

                currentReqPage->content = replaceWord(currentReqPage->content, "\n", "\\\\n");
                currentReqPage->content = replaceWord(currentReqPage->content, "\"", "\\\\\\\"");
                log_message(LOG_DEBUG, "About to update page:%s", currentReqPage->path);
                //getchar();
                updatePageContentMutation(currentReqPage);
                renderMutation(&currentReqPage, false);

                free(flag);

                break;
            }

        }


        currentReqPage = currentReqPage->next;
    }
    
    cJSON_Delete(requirementList);
    freePageList(&requirementPagesHead);
    
    log_message(LOG_DEBUG, "Exiting function updateRequirementPage");
    return;
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
            map = createCombinedString("\n", map);
            targetPage->content = replaceParagraph(targetPage->content, map, wikiCommand->pointerToEndOfFirstMarker, wikiCommand->pointerToBeginningOfSecondMarker);

            wasPageModified = 1;
            free(map);
        }

        log_message(LOG_DEBUG, "going to send message to slack");
        

        wikiCommand = wikiCommand->next;
    }

    if(wasPageModified){
        targetPage->content = replaceWord(targetPage->content, "\n", "\\n");
        targetPage->content = replaceWord(targetPage->content, "\\", "\\\\");
        targetPage->content = replaceWord(targetPage->content, "\"", "\\\"");
        updatePageContentMutation(targetPage);
        renderMutation(&targetPage, false);
        freePageList(&targetPage);
        sendMessageToSlack("onPageUpdate called on page id:");
        sendMessageToSlack(cmd.argument_1);
    }

    freeWikiFlagList(&wikiCommand);
    
    log_message(LOG_DEBUG, "Exiting function onPageUpdate");
}

void updateStatsPage(command cmd){
    log_message(LOG_DEBUG, "Entering function updateStatsPage");
    
    pageList* head = NULL;
    head = populatePageList(&head, "time", "none");

    pageList* current = head;

    char* linksListOfPages = "# Recently Edited Pages\n";

    for(int i = 0; i < 5; i++){
        linksListOfPages = appendToString(linksListOfPages, "- [");
        linksListOfPages = appendToString(linksListOfPages, current->title);
        linksListOfPages = appendToString(linksListOfPages, "](/");
        linksListOfPages = appendToString(linksListOfPages, current->path);
        linksListOfPages = appendToString(linksListOfPages, ")\n**Updated on:** ");
        linksListOfPages = appendToString(linksListOfPages, convert_timestamp_to_cest(current->updatedAt));
        linksListOfPages = appendToString(linksListOfPages, "\n");
        current = current->next;
    }

    linksListOfPages = appendToString(linksListOfPages, "{.links-list}");

    linksListOfPages = appendToString(linksListOfPages, "\n\n# Page Distribution\n");
    linksListOfPages = appendToString(linksListOfPages, buildPageDistributionPieChart(head));

    freePageList(&head);
    linksListOfPages = replaceWord(linksListOfPages, "\n", "\\\\n");
    linksListOfPages = replaceWord(linksListOfPages, "\"", "\\\\\\\"");

    pageList* statsPage = NULL;
    statsPage = addPageToList(&statsPage, "1178", "", "", "", linksListOfPages, "", "", "");
    log_message(LOG_DEBUG, "statsPage id set");
    updatePageContentMutation(statsPage);
    renderMutation(&statsPage, false);
    sendMessageToSlack("stats page updated");

    free(linksListOfPages);

    log_message(LOG_DEBUG, "Exiting function updateStatsPage");
}

void createMissingRequirementPages(command cmd){
    refreshOAuthToken();
    char *sheetId;
    pageList* requirementPagesHead = NULL;
    char *path;

    path = cmd.argument_1;

    if(strcmp(cmd.argument_1, "ST")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_ST_DRL/";
        sheetId = "ST!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "PR")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/";
        sheetId = "PR!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "FD")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_FD_DRL/";
        sheetId = "FD!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "RE")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_RE_DRL/";
        sheetId = "RE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "GS")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_GS_DRL/";
        sheetId = "GS!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "AV")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_AV_DRL/";
        sheetId = "AV!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "TE")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_TE_DRL/";
        sheetId = "TE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "PL")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PL_DRL/";
        sheetId = "PL!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "GE")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_GE_DRL/";
        sheetId = "GE!A3:AT300";
    }

    requirementPagesHead = populatePageList(&requirementPagesHead, "path", path);
    batchGetSheet("1i_PTwIqLuG9IUI73UaGuOvx8rVTDV1zIS7gmXNjMs1I", sheetId);

    cJSON *requirementList = parseArrayIntoJSONRequirementList(chunk.response);

    // Get the requirements array from the requirementList object
    cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");
    if (!cJSON_IsArray(requirements)) {
        log_message(LOG_ERROR, "Error: requirements is not a JSON array");
    }

    
    // Iterate over each requirement object in the requirements array
    int num_reqs = cJSON_GetArraySize(requirements);

    for (int i = 0; i < num_reqs; i++) {
        cJSON *requirement = cJSON_GetArrayItem(requirements, i);

        if (!cJSON_IsObject(requirement)) {
            log_message(LOG_ERROR, "Error: requirement is not a JSON object");
            continue;
        }

        // Get and print each item of the requirement object
        cJSON *id = cJSON_GetObjectItemCaseSensitive(requirement, "ID");
        pageList* currentReqPage = requirementPagesHead;
        int foundPage = 0;

        log_message(LOG_DEBUG, "Looking for page corresponding to requiremet: %s", id->valuestring);

        if(!strstr(id->valuestring, "2024_")){
            log_message(LOG_DEBUG, "Found a group, skipping");
            continue;
        }

        while(currentReqPage){
            if (cJSON_IsString(id) && id->valuestring && strcmp(id->valuestring, currentReqPage->title) == 0){
                log_message(LOG_DEBUG, "Found %s, breaking", currentReqPage->title);
                foundPage = 1;
                break;
            }

            else{
                currentReqPage = currentReqPage->next;
            }

        }

        if (foundPage == 0){
            char *reqPath = createCombinedString(path, id->valuestring);
            char *reqContent = "<!--";
            reqContent = appendToString(reqContent, id->valuestring);
            reqContent = appendToString(reqContent, "-->\\\\n");
            reqContent = appendToString(reqContent, reqContent);
            log_message(LOG_DEBUG, "About to create new page path:%s\nTitle:%s", reqPath, id->valuestring);
            //getchar();
            createPageMutation(reqPath, reqContent, id->valuestring);

            free(reqPath);
            free(reqContent);
        }

    }

    
    cJSON_Delete(requirementList);
    freePageList(&requirementPagesHead);
    
    log_message(LOG_DEBUG, "Exiting function updateRequirementPage");
    sendMessageToSlack("Pages were created");
    return;
}