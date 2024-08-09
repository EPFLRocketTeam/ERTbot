

#include "../headerFiles/struct.h"
#include "../headerFiles/api.h"
#include "../headerFiles/config.h"
#include "../headerFiles/features.h"
#include "../headerFiles/githubAPI.h"
#include "../headerFiles/helperFunctions.h"
#include "../headerFiles/markdownToPDF.h"
#include "../headerFiles/slackAPI.h"
#include "../headerFiles/stringTools.h"
#include "../headerFiles/wikiAPI.h"
#include "../headerFiles/sheetAPI.h"

//Status: UP
//builds a wikimap starting from cmd.argument_1 all the way down
char* buildMap(command cmd) {
    pageList* listOfDaughterPages = NULL;
    printf("about to call populatePageList\n");
    listOfDaughterPages = populatePageList(&listOfDaughterPages, "path",  cmd.argument_1); // Parse JSON and populate linked list
    sendMessageToSlack("Finished gathering pages");
    char *map = createMapWBS(&listOfDaughterPages);
    freePageList(&listOfDaughterPages); // Free the memory used by the linked list
    return map;
}

//Status: UP
//Finds all of the page links on the wiki and saves the to LINK_TRACKER_PAGE_ID
void buildLinksTracker(command cmd) {
    
    pageList* linkTrackerPage = NULL;
    linkTrackerPage = addPageToList(&linkTrackerPage, LINK_TRACKER_PAGE_ID, "", "", "", "", "", "");
    fprintf(stderr,"linkTrackerPage id set\n");
    linkTrackerPage = getPage(&linkTrackerPage);

    linkTrackerPage->content = "";

    fprintf(stderr,"fetched linkTrackerPage content\n");
    pageList* listOfAllPages = NULL;

    fprintf(stderr,"about to fetch list of all pages");
    listOfAllPages = populatePageList(&listOfAllPages, "path", "none"); // Parse JSON and populate linked list
    fprintf(stderr,"populated list of pages\n");

    
    while (listOfAllPages != NULL) {
        fprintf(stderr,"about to fetch title: %s.\n", listOfAllPages->title);

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

            //Used for debugging
            //updatePageContentMutation(linkTrackerPage);
        }
    }
    

    fprintf(stderr,"Finished searching all of the pages\n");

    fprintf(stderr,"linkTrackerPage content: %s\n", linkTrackerPage->content);

    fprintf(stderr,"about to update linkTrackerPage\n");


    updatePageContentMutation(linkTrackerPage);
    renderMutation(&linkTrackerPage);
    // Free the memory used by the linked list
    freePageList(&linkTrackerPage);// Free the memory used by the linked list
    freePageList(&listOfAllPages);// Free the memory used by the linked list
}

//Status: code written but requires more tests
//Finds all of the page links on the wiki and saves the to LINK_TRACKER_PAGE_ID
void updateLinksTracker(command cmd) {
    
    pageList* linkTrackerPage = (pageList*) malloc(sizeof(pageList)); // Allocate memory for linkTrackerPage
    if (linkTrackerPage == NULL) {
        fprintf(stderr, "Memory allocation failed for linkTrackerPage\n");
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
                fprintf(stderr, "links are: %s\n", links->path);
                links = links->next;
            }
            
            freePageList(&links); //free the linked list which stores the links
            fprintf(stderr, "links freed\n");

            if(listOfAllPages->next == NULL){break;}
            else{listOfAllPages = listOfAllPages->next;}//go to next page
            

        }
    }
    
    fprintf(stderr, "Making sure linkTrackerPage has the right amount of backslashes\n");
    linkTrackerPage->content = replaceWord(linkTrackerPage->content, "\\n", "\\\\n");

    //fprintf(stderr,"Finished searching all of the pages\n");
    //fprintf(stderr,"linkTrackerPage content: %s\n", linkTrackerPage->content);
    fprintf(stderr,"about to update linkTrackerPage\n");
    

    updatePageContentMutation(linkTrackerPage);
    renderMutation(&linkTrackerPage);

    // Free the memory used by the linked list
    freePageList(&linkTrackerPage);// Free the memory used by the linked list
    //freePageList(&listOfAllPages);// Free the memory used by the linked list
}


//Build local graph
void buildLocalGraph(command cmd) {
    // Allocate memory for linkTrackerPage
    pageList* linkTrackerPage = NULL;
    linkTrackerPage = addPageToList(&linkTrackerPage, LINK_TRACKER_PAGE_ID, "", "", "", "", "", "");
    fprintf(stderr,"linkTrackerPage id set\n");
    linkTrackerPage = getPage(&linkTrackerPage);

    pageList* subjectPage = NULL;

    subjectPage = populatePageList(&subjectPage, "exact path", cmd.argument_1);

    pageList* IncomingLinks;
    pageList* OutgoingLinks;

    IncomingLinks = findIncomingLinks(&IncomingLinks, linkTrackerPage->content, subjectPage->path);
    OutgoingLinks = findOutgoingLinks(&OutgoingLinks, linkTrackerPage->content, subjectPage->path);

    char *tempGraph = createLocalGraphMindMap(&subjectPage, &IncomingLinks, &OutgoingLinks);

    sendMessageToSlack(tempGraph);
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


//Build accronyme list and save locally
void buildAcronymsList(command cmd) {
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
}

//Status: UP
void getPages(command cmd) {
    pageList* head = NULL;
    printf("about to call populatePageList\n");
    head = populatePageList(&head, "path", cmd.argument_1); // Parse JSON and populate linked list
    printf("about to printPages\n");
    printPages(&head);
    sendMessageToSlack("All Pages Printed\n");
    // Free the memory used by the linked list
    freePageList(&head);
}

// Mass replace all strings
void replaceText(command cmd) {
    pageList* head;
    head = populatePageList(&head, "path", cmd.argument_1); // Parse JSON and populate linked list
    replaceStringInWiki(&head, cmd.argument_2,cmd.argument_3);
    // Free the memory used by the linked list
    freePageList(&head);
}

//Moves the page at argument_1 to argument_2 and then replaces all occurence of argument_1 with argument_2 on all pages in the wiki
void movePage(command cmd){

    cmd.argument_1 = replaceWord(cmd.argument_1, "\\", "");
    cmd.argument_2 = replaceWord(cmd.argument_2, "\\", "");

    pageList* subjectPage = NULL;
    subjectPage = populatePageList(&subjectPage, "exact path", cmd.argument_1);
    subjectPage->path = cmd.argument_2;
    movePageContentMutation(&subjectPage);

    pageList* linkTrackerPage = NULL;
    linkTrackerPage = addPageToList(&linkTrackerPage, LINK_TRACKER_PAGE_ID, "", "", "", "", "", "");
    fprintf(stderr,"linkTrackerPage id set\n");
    linkTrackerPage = getPage(&linkTrackerPage);

    pageList* IncomingLinks = NULL;
    fprintf(stderr, "cmd.argument_1:%s.\n", cmd.argument_1);
    IncomingLinks = findIncomingLinks(&IncomingLinks, linkTrackerPage->content, cmd.argument_1);

    char *originalPathWithParentheseAndSlash = appendStrings("(/", cmd.argument_1);
    char *newPathWithParentheseAndSlash = appendStrings("(/", cmd.argument_2);

    fprintf(stderr, "Going to start modifying target pages\n");
    while(IncomingLinks != NULL){
        pageList* temporaryPage = NULL;
        temporaryPage = populatePageList(&temporaryPage, "exact path", IncomingLinks->path);
        temporaryPage = getPage(&temporaryPage);
        fprintf(stderr, "Going to change %s's content \n", temporaryPage->path);
        temporaryPage->content = replaceWord(temporaryPage->content, originalPathWithParentheseAndSlash, newPathWithParentheseAndSlash);
        //fprintf(stderr, "%s", temporaryPage->content);
        fprintf(stderr, "\n\nIs it ok?");
        getchar();
        temporaryPage->content = replaceWord(temporaryPage->content, "\\", "\\\\");
        temporaryPage->content = replaceWord(temporaryPage->content, "\"", "\\\"");
        updatePageContentMutation(temporaryPage);
        fprintf(stderr, "\nUpdated Page\n");
        renderMutation(&temporaryPage);
        fprintf(stderr, "\nRendered Page\n");
        IncomingLinks = IncomingLinks->next;
        freePageList(&temporaryPage);
    }


    char *originalPathWithLineReturn = appendStrings(cmd.argument_1, "\\n");
    char *newPathWithLineReturn = appendStrings(cmd.argument_2, "\\n");

    linkTrackerPage->content = replaceWord(linkTrackerPage->content, originalPathWithParentheseAndSlash, newPathWithParentheseAndSlash);
    linkTrackerPage->content = replaceWord(linkTrackerPage->content, originalPathWithLineReturn, newPathWithLineReturn);

    linkTrackerPage->content = replaceWord(linkTrackerPage->content, "\\n", "\\\\n");
    updatePageContentMutation(linkTrackerPage);
    fprintf(stderr, "\nUpdated Page\n");
    renderMutation(&linkTrackerPage);
    fprintf(stderr, "\nRendered Page\n");

    freePageList(&subjectPage);
    freePageList(&linkTrackerPage);

    return;
}

void syncSheetToDrl(command cmd){

    pageList* drlPage = (pageList*) malloc(sizeof(pageList)); // Allocate memory for linkTrackerPage
    if (drlPage == NULL) {
        fprintf(stderr, "Memory allocation failed for drlPage\n");
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

    fprintf(stderr, "parseJSONRequirementListInToArray\n");
    char* output = parseJSONRequirementListInToArray(requirements);

    fprintf(stderr, "batchUpdateSheet\n");
    batchUpdateSheet("14vOyP1Oc5O_7JY1vnY7pQPTJoOB8oi4nNRMEsodIvtU", "Sheet1!A1:D36", output);

    fprintf(stderr, "deallocating memory\n");
    cJSON_Delete(requirementList);
    free(output);

    return;
}

void syncDrlToSheet(command cmd){

    batchGetSheet("14vOyP1Oc5O_7JY1vnY7pQPTJoOB8oi4nNRMEsodIvtU", "NewVersion!A4:AI67");
    fprintf(stderr, "chunk.response: %s\n", chunk.response);

    cJSON *requirementList = parseArrayIntoJSONRequirementList(chunk.response); 
    char *DRL = buildDrlFromJSONRequirementList(requirementList);

    pageList* drlPage = NULL;
    drlPage = addPageToList(&drlPage, TEST_DRL_PAGE_ID, "", "", "", "", "", "");
    drlPage->content = DRL;

    drlPage->content = replaceWord(drlPage->content, "\n", "\\\\n");
    drlPage->content = replaceWord(drlPage->content, "\"", "\\\"");

    updatePageContentMutation(drlPage);
    renderMutation(&drlPage);

    freePageList(&drlPage);
    cJSON_Delete(requirementList);
    free(DRL);

    return;
}

void createRequirementPage(command cmd){

    batchGetSheet("14vOyP1Oc5O_7JY1vnY7pQPTJoOB8oi4nNRMEsodIvtU", "NewVersion!A4:AI67");
    fprintf(stderr, "chunk.response: %s\n", chunk.response);
    cJSON *requirementList = parseArrayIntoJSONRequirementList(chunk.response);
    pageList* reqPage = buildRequirementPageFromJSONRequirementList(requirementList, cmd.argument_1);
    reqPage->content = replaceWord(reqPage->content, "\n", "\\\\n");
    reqPage->content = replaceWord(reqPage->content, "\"", "\\\"");
    updatePageContentMutation(reqPage);
    renderMutation(&reqPage);
    cJSON_Delete(requirementList);
    freePageList(&reqPage);
    return;
}

void createVcdPage(command cmd){

    batchGetSheet("14vOyP1Oc5O_7JY1vnY7pQPTJoOB8oi4nNRMEsodIvtU", "NewVersion!A4:AI67");
    fprintf(stderr, "chunk.response: %s\n", chunk.response);
    cJSON *requirementList = parseArrayIntoJSONRequirementList(chunk.response);

    char *pieChart = createVcdPieChart("20", "30", "40");
    
    pageList* C_ST_VCD_DRAFT = NULL;
    C_ST_VCD_DRAFT = addPageToList(&C_ST_VCD_DRAFT, "1130", "", "", "", "", "", "");
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

    //cJSON_Delete(requirementList);
    freePageList(&C_ST_VCD_DRAFT);
    return;
}