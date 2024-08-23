/**
 * @file command.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief Contains functions used to handle commands.
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


static PeriodicCommand* addPeriodicCommand(PeriodicCommand** headOfPeriodicCommands, command* command, int period) {
    log_message(LOG_DEBUG, "Entering function addPeriodicCommand");
    
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
    log_message(LOG_DEBUG, "Exiting function addPeriodicCommand");

    return *headOfPeriodicCommands;
}

static command* addCommandToQueue(command** head,  char *function, char *argument_1, char *argument_2, char *argument_3, char *argument_4, char *argument_5, char *argument_6, char *argument_7, char *argument_8, char *argument_9) {
    log_message(LOG_DEBUG, "Entering function addCommandToQueue");
        
    command* newNode = (command *)malloc(sizeof(command));
    if (!newNode) {
        log_message(LOG_ERROR, "Memory allocation error");
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
    log_message(LOG_DEBUG, "function added to command");
    newNode->function = malloc(strlen(function) + 1);
    strcpy(newNode->function, function);
    }

    if(argument_1 && argument_1 != NULL){
        log_message(LOG_DEBUG, "argument 1 added to command");
        newNode->argument_1 = malloc(strlen(argument_1) + 1);
        strcpy(newNode->argument_1, argument_1);
    }

    if(argument_2 && argument_2 != NULL){
        log_message(LOG_DEBUG, "argument 2 added to command");
        newNode->argument_2 = malloc(strlen(argument_2) + 1);
        strcpy(newNode->argument_2, argument_2);
    }

    if(argument_3 && argument_3 != NULL){
        log_message(LOG_DEBUG, "argument 1 added to command");
        newNode->argument_3 = malloc(strlen(argument_3) + 1);
        strcpy(newNode->argument_3, argument_3);
    }

    if(argument_4 && argument_4 != NULL){
        log_message(LOG_DEBUG, "argument 4 added to command");
        newNode->argument_4 = malloc(strlen(argument_4) + 1);
        strcpy(newNode->argument_4, argument_4);
    }

    if(argument_5 && argument_5 != NULL){
        log_message(LOG_DEBUG, "argument 5 added to command");
        newNode->argument_5 = malloc(strlen(argument_5) + 1);
        strcpy(newNode->argument_5, argument_5);
    }

    if(argument_6 && argument_6 != NULL){
        log_message(LOG_DEBUG, "argument 6 added to command");
        newNode->argument_6 = malloc(strlen(argument_6) + 1);
        strcpy(newNode->argument_6, argument_6);
    }

    if(argument_7 && argument_7 != NULL){
        log_message(LOG_DEBUG, "argument 7 added to command");
        newNode->argument_7 = malloc(strlen(argument_7) + 1);
        strcpy(newNode->argument_7, argument_7);
    }

    if(argument_8 && argument_8 != NULL){
        log_message(LOG_DEBUG, "argument 8 added to command");
        newNode->argument_8 = malloc(strlen(argument_8) + 1);
        strcpy(newNode->argument_8, argument_8);
    }

    if(argument_9 && argument_9 != NULL){
        log_message(LOG_DEBUG, "argument 9 added to command");
        newNode->argument_9 = malloc(strlen(argument_9) + 1);
        strcpy(newNode->argument_9, argument_9);
    }


    log_message(LOG_DEBUG, "All arguments added to command struct");
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

    log_message(LOG_DEBUG, "Command added to queue");
    
    log_message(LOG_DEBUG, "Exiting function addCommandToQueue");
    return *head;
}

void removeFirstCommand(command **head) {
    log_message(LOG_DEBUG, "Entering function removeFirstCommand");
    
    log_message(LOG_DEBUG, "About to remove first command");

    if (*head == NULL) {
        // The list is already empty, nothing to remove
        return;
    }

    log_message(LOG_DEBUG, "Creating a temporary pointer to a command");
    // Save the current head node to free it later
    command* temp = *head;

    log_message(LOG_DEBUG, "Placing the head on to the next command");
    // Update the head to point to the next node
    *head = (*head)->next;

    // Free the memory of the removed node
    
    if (temp->function && temp->function != NULL){
        free(temp->function);
    }

    if (temp->argument_1 && temp->argument_1 != NULL){
        free(temp->argument_1);
    }

    if (temp->argument_2 && temp->argument_2 != NULL){
        free(temp->argument_2);
    }

    if (temp->argument_3 && temp->argument_3 != NULL){
        free(temp->argument_3);
    }

    if (temp->argument_4 && temp->argument_4 != NULL){
        free(temp->argument_4);
    }

    if (temp->argument_5 && temp->argument_5 != NULL){
        free(temp->argument_5);
    }

    if (temp->argument_6 && temp->argument_6 != NULL){
        free(temp->argument_6);
    }

    if (temp->argument_7 && temp->argument_7 != NULL){
        free(temp->argument_7);
    }

    if (temp->argument_8 && temp->argument_8 != NULL){
        free(temp->argument_8);
    }

    if (temp->argument_9 && temp->argument_9 != NULL){
        free(temp->argument_9);
    }

    log_message(LOG_DEBUG, "freed all variables");
    
    free(temp);

    log_message(LOG_DEBUG, "freed struct");
    
    log_message(LOG_DEBUG, "Exiting function removeFirstCommand"); 
}

static command** checkAndEnqueuePeriodicCommands(command** commandQueue, PeriodicCommand** headOfPeriodicCommands) {
    log_message(LOG_DEBUG, "Entering function checkAndEnqueuePeriodicCommands");
    
    time_t currentTime = time(NULL);
    PeriodicCommand* periodicCommand = (*headOfPeriodicCommands);

    while (periodicCommand != NULL) {
        if (periodicCommand->next_time <= currentTime) {
            // Enqueue the command into the commandQueue
            command cmd = *periodicCommand->command;
            *commandQueue = addCommandToQueue(commandQueue, cmd.function, cmd.argument_1, cmd.argument_2, cmd.argument_3, cmd.argument_4, cmd.argument_5, cmd.argument_6, cmd.argument_7, cmd.argument_8, cmd.argument_9);

            // Update the next execution time
            periodicCommand->next_time = currentTime + periodicCommand->period;
        }
        periodicCommand = periodicCommand->next;
    }
    
    log_message(LOG_DEBUG, "Exiting function checkAndEnquePeriodicCommands");
    return commandQueue;
}

static command** lookForCommandOnSlack(command** headOfCommandQueue){
    log_message(LOG_DEBUG, "Entering function lookForCommandonSlack");

    command cmd;
    slackMessage* slackMsg = (slackMessage*)malloc(sizeof(slackMessage*));;

    slackMsg->message = malloc(200);
    if (slackMsg->message == NULL) {
        log_message(LOG_ERROR, "Memory allocation failed");
    }


    slackMsg = getSlackMessage(slackMsg);
    
    //If received a message which was not sent by bot, breakdown message into command structure and return command 
    if(strcmp(slackMsg->sender, "U06RQCAT0H1") != 0){
        breakdownCommand(slackMsg->message, &cmd);
        log_message(LOG_DEBUG, "Command broke down");
        *headOfCommandQueue = addCommandToQueue(headOfCommandQueue, cmd.function, cmd.argument_1, cmd.argument_2, cmd.argument_3, cmd.argument_4, cmd.argument_5, cmd.argument_6, cmd.argument_7, cmd.argument_8, cmd.argument_9);
        log_message(LOG_INFO, "Received a %s command on slack", cmd.function);
        log_message(LOG_DEBUG, "Command added to queue");
    }
    
    //If last message was sent by bot, free allocated memory and return emtpy command
    else{log_message(LOG_DEBUG, "No commands sent on slack"); }//log.info
    
    if (chunk.response) {
        chunk.response = NULL;
        chunk.size = 0;
    }

    free(slackMsg->message);
    free(slackMsg->sender);
    free(slackMsg->timestamp);
    
    log_message(LOG_DEBUG, "Exiting function lookForCommandOnSlack");
    return headOfCommandQueue;

}

static command** lookForNewlyUpdatedPages(command** commandQueue){
    log_message(LOG_DEBUG, "Entering function lookForNewlyUpdatedPages");
    
    pageList* updatedPages = NULL;
    log_message(LOG_DEBUG, "Last Page refresh check happened at: %s", lastPageRefreshCheck);
    updatedPages = populatePageList(&updatedPages, "time", lastPageRefreshCheck);
    pageList* updatedPagesHead = updatedPages;

    while(updatedPages){

        *commandQueue = addCommandToQueue(commandQueue, "onPageUpdate", updatedPages->id, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        log_message(LOG_INFO, "onPageUpdate command has been added to queue for page id: %s", updatedPages->id);
        updatedPages = updatedPages->next;
    }

    freePageList(&updatedPagesHead);
    lastPageRefreshCheck = getCurrentEDTTimeString();
    
    log_message(LOG_DEBUG, "Exiting function lookForNewlyUpdatedPages");
    return commandQueue;
}

command** checkForCommand(command** headOfCommandQueue, PeriodicCommand** headOfPeriodicCommands){
    log_message(LOG_DEBUG, "Entering function checkForCommand");
    
    headOfCommandQueue = lookForCommandOnSlack(headOfCommandQueue);
    headOfCommandQueue = lookForNewlyUpdatedPages(headOfCommandQueue);
    //headOfCommandQueue = checkAndEnqueuePeriodicCommands(headOfCommandQueue, headOfPeriodicCommands);
    
    log_message(LOG_DEBUG, "Exiting function checkForCommand");
    return headOfCommandQueue;
}

PeriodicCommand** initalizePeriodicCommands(PeriodicCommand** headOfPeriodicCommands){
    log_message(LOG_DEBUG, "Entering function initializePeriodicCommands");
    
    command* getRyansHomePage = (command*)malloc(sizeof(command));
    getRyansHomePage->function = "updateStatsPage";
    getRyansHomePage->argument_1 = NULL;
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
    *headOfPeriodicCommands = addPeriodicCommand(headOfPeriodicCommands, getRyansHomePage, 3600);
    
    log_message(LOG_DEBUG, "Exiting function initializePeriodicCommands");
    return headOfPeriodicCommands;
}

command** executeCommand(command** commandQueue){
    log_message(LOG_DEBUG, "Entering function executeCommand");
    
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
        onPageUpdate(**commandQueue);
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "buildMap") == 0){ //works
        sendMessageToSlack(buildMap(**commandQueue));
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "buildLocalGraph") == 0){ //Works
        buildLocalGraph(**commandQueue);
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "updateStatsPage") == 0){
        updateStatsPage(**commandQueue);
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

    removeFirstCommand(commandQueue);
    
    log_message(LOG_DEBUG, "Exiting function executeCommand");
    return commandQueue;
    
}
