/**
 * @file command.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief 
 * 
 * @details
 * 
 * 
 * 
 * @warning AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHH!
 * - Out here dereferencing NULL Pointers 
 * @todo
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

static PeriodicCommand* addPeriodicCommand(PeriodicCommand** headOfPeriodicCommands, command* command, int period) {
    PeriodicCommand* newCommand = (PeriodicCommand*)malloc(sizeof(PeriodicCommand));
    newCommand->command = command;  // Duplicate the command string
    newCommand->period = period;
    newCommand->next_time = time(NULL) + period;  // Set the next execution time
    newCommand->next = NULL;

    // If the list is empty, make the new node the first node
    if (*headOfPeriodicCommands == NULL) {
        *headOfPeriodicCommands = newCommand;
        return *headOfPeriodicCommands;
    }

    // Traverse the list to find the last node
    PeriodicCommand* lastNode = *headOfPeriodicCommands;
    while (lastNode->next != NULL) {
        lastNode = lastNode->next;
    }

    // Link the new node after the last node
    lastNode->next = newCommand;
    return *headOfPeriodicCommands;
}

static command* addCommmandToQueue(command** head,  char *function, char *argument_1, char *argument_2, char *argument_3, char *argument_4, char *argument_5, char *argument_6, char *argument_7, char *argument_8, char *argument_9) {
    command* newNode = (command *)malloc(sizeof(command));
    if (!newNode) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }

    // Initialize all pointers to NULL
    newNode->function = NULL;
    newNode->argument_1 = NULL;
    newNode->argument_2 = NULL;
    newNode->argument_3 = NULL;
    newNode->argument_4 = NULL;
    newNode->argument_5 = NULL;
    newNode->argument_6 = NULL;
    newNode->argument_7 = NULL;
    newNode->argument_8 = NULL;
    newNode->argument_9 = NULL;

    // Allocate memory and copy the path and id
    if(function && function != NULL){
    fprintf(stderr, "function added to command\n");
    newNode->function = malloc(strlen(function) + 1);
    strcpy(newNode->function, function);
    }

    if(argument_1 && argument_1 != NULL){
        fprintf(stderr, "argument 1 added to command\n");
        newNode->argument_1 = malloc(strlen(argument_1) + 1);
        strcpy(newNode->argument_1, argument_1);
    }

    if(argument_2 && argument_2 != NULL){
        fprintf(stderr, "argument 2 added to command\n");
        newNode->argument_2 = malloc(strlen(argument_2) + 1);
        strcpy(newNode->argument_2, argument_2);
    }

    if(argument_3 && argument_3 != NULL){
        fprintf(stderr, "argument 3 added to command\n");
        newNode->argument_3 = malloc(strlen(argument_3) + 1);
        strcpy(newNode->argument_3, argument_3);
    }

    if(argument_4 && argument_4 != NULL){
        newNode->argument_4 = malloc(strlen(argument_4) + 1);
        strcpy(newNode->argument_4, argument_4);
    }

    if(argument_5 && argument_5 != NULL){
        newNode->argument_5 = malloc(strlen(argument_5) + 1);
        strcpy(newNode->argument_5, argument_5);
    }

    if(argument_6 && argument_6 != NULL){
        newNode->argument_6 = malloc(strlen(argument_6) + 1);
        strcpy(newNode->argument_6, argument_6);
    }

    if(argument_7 && argument_7 != NULL){
        newNode->argument_7 = malloc(strlen(argument_7) + 1);
        strcpy(newNode->argument_7, argument_7);
    }

    if(argument_8 && argument_8 != NULL){
        newNode->argument_8 = malloc(strlen(argument_8) + 1);
        strcpy(newNode->argument_8, argument_8);
    }

    if(argument_9 && argument_9 != NULL){
        newNode->argument_9 = malloc(strlen(argument_9) + 1);
        strcpy(newNode->argument_9, argument_9);
    }


    fprintf(stderr, "All arguments added to command Queue\n");

    newNode->next = NULL;  // New node will be the last node

    // If the list is empty, make the new node the first node
    if (*head == NULL) {
        *head = newNode;
        return *head;
    }

    // Traverse the list to find the last node
    command* lastNode = *head;
    while (lastNode->next != NULL) {
        lastNode = lastNode->next;
    }

    // Link the new node after the last node
    lastNode->next = newNode;

    fprintf(stderr, "Command added to queue\n");

    return *head;
}

void removeFirstCommand(command **head) {

    fprintf(stderr, "About to remove first command\n");

    if (*head == NULL) {
        // The list is already empty, nothing to remove
        return;
    }

    fprintf(stderr, "Creating a temporary pointer to a command\n");

    // Save the current head node to free it later
    command* temp = *head;

    fprintf(stderr, "Placing the head on to the next command\n");
    // Update the head to point to the next node
    *head = (*head)->next;

    // Free the memory of the removed node
    
    if (temp->function && temp->function != NULL){
        free(temp->function);
        fprintf(stderr, "Freeing function: %p\n", (void*)temp->function);
    }

    if (temp->argument_1 && temp->argument_1 != NULL){
        free(temp->argument_1);
        fprintf(stderr, "Freeing argument_1: %p\n", (void*)temp->argument_1);
    }

    fprintf(stderr, "About to free argument_2\n");
    if (temp->argument_2 && temp->argument_2 != NULL){
        free(temp->argument_2);
        fprintf(stderr, "Freeing argument_2: %p\n", (void*)temp->argument_2);
    }
    fprintf(stderr, "Freed argument_2\n");

    if (temp->argument_3 && temp->argument_3 != NULL){
        free(temp->argument_3);
        fprintf(stderr, "Freeing argument_3: %p\n", (void*)temp->argument_3);
    }

    if (temp->argument_4 && temp->argument_4 != NULL){
        free(temp->argument_4);
        fprintf(stderr, "Freeing argument_4: %p\n", (void*)temp->argument_4);
    }

    if (temp->argument_5 && temp->argument_5 != NULL){
        free(temp->argument_5);
        fprintf(stderr, "Freeing argument_5: %p\n", (void*)temp->argument_5);
    }

    if (temp->argument_6 && temp->argument_6 != NULL){
        free(temp->argument_6);
        fprintf(stderr, "Freeing argument_6: %p\n", (void*)temp->argument_6);
    }

    if (temp->argument_7 && temp->argument_7 != NULL){
        free(temp->argument_7);
        fprintf(stderr, "Freeing argument_7: %p\n", (void*)temp->argument_7);
    }

    if (temp->argument_8 && temp->argument_8 != NULL){
        free(temp->argument_8);
        fprintf(stderr, "Freeing argument_8: %p\n", (void*)temp->argument_8);
    }

    if (temp->argument_9 && temp->argument_9 != NULL){
        free(temp->argument_9);
        fprintf(stderr, "Freeing argument_9: %p\n", (void*)temp->argument_9);
    }

    fprintf(stderr, "freed all variables\n");
    
    free(temp);

    fprintf(stderr, "freed struct\n");
    
}

static command** checkAndEnqueuePeriodicCommands(command** commandQueue, PeriodicCommand** headOfPeriodicCommands) {
    time_t currentTime = time(NULL);
    PeriodicCommand* periodicCommand = (*headOfPeriodicCommands);

    while (periodicCommand != NULL) {
        if (periodicCommand->next_time <= currentTime) {
            // Enqueue the command into the commandQueue
            command cmd = *periodicCommand->command;
            *commandQueue = addCommmandToQueue(commandQueue, cmd.function, cmd.argument_1, cmd.argument_2, cmd.argument_3, cmd.argument_4, cmd.argument_5, cmd.argument_6, cmd.argument_7, cmd.argument_8, cmd.argument_9);

            // Update the next execution time
            periodicCommand->next_time = currentTime + periodicCommand->period;
        }
        periodicCommand = periodicCommand->next;
    }

    return commandQueue;
}

static command** lookForCommandOnSlack(command** headOfCommandQueue){
    command cmd;
    slackMessage* slackMsg = (slackMessage*)malloc(sizeof(slackMessage*));;

    slackMsg->message = malloc(200);
    if (slackMsg->message == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
    }


    slackMsg = getSlackMessage(slackMsg);
    
    //If received a message which was not sent by bot, breakdown message into command structure and return command 
    if(strcmp(slackMsg->sender, "U06RQCAT0H1") != 0){
        fprintf(stderr, "Received a command on slack\n");
        breakdownCommand(slackMsg->message, &cmd);
        fprintf(stderr, "Command broke down\n");
        *headOfCommandQueue = addCommmandToQueue(headOfCommandQueue, cmd.function, cmd.argument_1, cmd.argument_2, cmd.argument_3, cmd.argument_4, cmd.argument_5, cmd.argument_6, cmd.argument_7, cmd.argument_8, cmd.argument_9);
        fprintf(stderr, "Command added to queue\n");
    }
    
    //If last message was sent by bot, free allocated memory and return emtpy command
    else{fprintf(stderr, "No commands sent\n"); }//log.info
    
    if (chunk.response) {
        chunk.response = NULL;
        chunk.size = 0;
    }

    free(slackMsg->message);
    free(slackMsg->sender);
    free(slackMsg->timestamp);

    return headOfCommandQueue;

}

static command** lookForNewlyUpdatedPages(command** commandQueue){
    pageList* updatedPages = NULL;
    updatedPages = populatePageList(&updatedPages, "time", lastPageRefreshCheck);
    pageList* updatedPagesHead = updatedPages;

    while(updatedPages){
        *commandQueue = addCommmandToQueue(commandQueue, "onPageUpdate", updatedPages->id, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        updatedPages = updatedPages->next;
    }

    freePageList(&updatedPagesHead);
    lastPageRefreshCheck = getCurrentEDTTimeString();

    return commandQueue;
}

command** checkForCommand(command** headOfCommandQueue, PeriodicCommand** headOfPeriodicCommands){
    
    headOfCommandQueue = lookForCommandOnSlack(headOfCommandQueue);//done
    fprintf(stderr, "Checked Slack\n");
    headOfCommandQueue = lookForNewlyUpdatedPages(headOfCommandQueue);//done
    fprintf(stderr, "Checked wiki\n");
    headOfCommandQueue = checkAndEnqueuePeriodicCommands(headOfCommandQueue, headOfPeriodicCommands);//done
    fprintf(stderr, "Checked periodic commands\n");

    return headOfCommandQueue;
}

PeriodicCommand** initalizePeriodicCommands(PeriodicCommand** headOfPeriodicCommands){
    command* getRyansHomePage = (command*)malloc(sizeof(command));
    getRyansHomePage->function = "getPages";
    getRyansHomePage->argument_1 = "competition/firehorn/systems_engineering/other/ryan_homepage/bababoui";
    getRyansHomePage->argument_2 = NULL;
    getRyansHomePage->argument_3 = NULL;
    getRyansHomePage->argument_4 = NULL;
    getRyansHomePage->argument_5 = NULL;
    getRyansHomePage->argument_6 = NULL;
    getRyansHomePage->argument_7 = NULL;
    getRyansHomePage->argument_8 = NULL;
    getRyansHomePage->argument_9 = NULL;
    getRyansHomePage->next = NULL;

    headOfPeriodicCommands = (PeriodicCommand**)malloc(sizeof(PeriodicCommand*));
    *headOfPeriodicCommands = NULL;
    *headOfPeriodicCommands = addPeriodicCommand(headOfPeriodicCommands, getRyansHomePage, 10);

    return headOfPeriodicCommands;
}

command** executeCommand(command** commandQueue){
    //TerminalCommandFeatures
    if((*commandQueue)->function && strcmp((*commandQueue)->function, "getPages") == 0){ //works
        getPages(**commandQueue);
    }

    else if((*commandQueue)->function && strcmp((*commandQueue)->function, "shutdown") == 0){ //works
        sendMessageToSlack("Shutting down");
        exit(0);
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "buildLinksTracker") == 0){ //Works
        buildLinksTracker();
        sendMessageToSlack("Link tracker page created");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "updateLinksTracker") == 0){ //Works
        updateLinksTracker();
        sendMessageToSlack("Link tracker page updated");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "movePage") == 0){ //Works
        movePage(**commandQueue);
        sendMessageToSlack("Page has been moved and links have been updated");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "syncSheetToDRL") == 0){
        syncSheetToDrl(**commandQueue);
        sendMessageToSlack("Finished parsing.");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "syncDRLToSheet") == 0){
        syncDrlToSheet(**commandQueue);
        sendMessageToSlack("Finished parsing.");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "createRequirementPage") == 0){
        createRequirementPage(**commandQueue);
        sendMessageToSlack("Page created.");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "createVCDPage") == 0){
        createVcdPage(**commandQueue);
        sendMessageToSlack("Page created.");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "refreshOAuthToken") == 0){
        refreshOAuthToken();
        sendMessageToSlack("Token Refreshed");
    }
    
    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "onPageUpdate") == 0){
        //onPageUpdate(**commandQueue);
        sendMessageToSlack("onPageUpdate called on page id:");
        sendMessageToSlack((*commandQueue)->argument_1);
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "buildMap") == 0){ //works
        sendMessageToSlack(buildMap(**commandQueue));
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "buildLocalGraph") == 0){ //Works
        buildLocalGraph(**commandQueue);
    }

/*
    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "getPDF") == 0){
        sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
    }

    if((*commandQueue)->function && strcmp((*commandQueue)->function, "getZip") == 0){
        getZip(**commandQueue);
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "buildImageTracker") == 0){
        //buildImageTracker(**commandQueue);
        sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "updateImageTracker") == 0){
        sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "notifyMe") == 0){
        sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "stopNotifyMe") == 0){
        sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "updateAccronymTracker") == 0){
        sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "updateBrokenLinksTracker") == 0){
        sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "updateSync") == 0){
        sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "spellCheckAI") == 0){
        sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "summariesAI") == 0){
        sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "suggestionsAI") == 0){
        sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "replaceText") == 0){
        replaceText(**commandQueue);
    }
    */

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "help") == 0){
        sendMessageToSlack("Here is a list of the possible commands: ");
    }

    else{
        sendMessageToSlack("Uknown Command :rayane_side_eyeing:");
    }

    fprintf(stderr, "about to run removeFirstCommand\n");

    removeFirstCommand(commandQueue);

    return commandQueue;
    
}
