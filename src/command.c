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


command* checkForCommand(command** commandQueue){
    lookForCommandOnSlack(&commandQueue);//done
    lookForNewlyUpdatedPages(&commandQueue);//done
    checkAndEnqueuePeriodicCommands(&commandQueue, &listOfPeriodicCommands);//done
}

PeriodicCommand* initalizePeriodicCommands(){
    PeriodicCommand* listOfPeriodicCommands = NULL;
}

command* executeCommand(command** commandQueue){

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

    /*
    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "onPageUpdate") == 0){
        onPageUpdate(**commandQueue);
        sendMessageToSlack("Page commands have been executed.");
    }*/

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

    removeFirstCommand(commandQueue);

    return commandQueue;
    
}

static command* lookForCommandOnSlack(command** commandQueue){
    command cmd;
    slackMessage slackMsg;

    slackMsg = getSlackMessage(slackMsg);
    
    //If received a message which was not sent by bot, breakdown message into command structure and return command 
    if(strcmp(slackMsg.sender, "U06RQCAT0H1") != 0){
        breakdownCommand(slackMsg.message, &cmd);
        commandQueue = addCommmandToQueue(&commandQueue, cmd.function, cmd.argument_1, cmd.argument_2, cmd.argument_3, cmd.argument_4, cmd.argument_5, cmd.argument_6, cmd.argument_7, cmd.argument_8, cmd.argument_9);
    }
    
    //If last message was sent by bot, free allocated memory and return emtpy command
    else{printf("No commands sent\n"); }//log.info
    
    if (chunk.response) {
        chunk.response = NULL;
        chunk.size = 0;
    }

    free(slackMsg.message);
    free(slackMsg.sender);
    free(slackMsg.timestamp);

    return *commandQueue;

}

static command* lookForNewlyUpdatedPages(command** commandQueue){
    pageList* updatedPages = NULL;
    updatedPages = populatePageList(&updatedPages, "time", lastPageRefreshCheck);
    pageList* updatedPagesHead = updatedPages;

    while(updatedPages){
        commandQueue = addCommmandToQueue(&commandQueue, "onPageUpdate", updatedPages->id, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        updatedPages = updatedPages->next;
    }

    freePageList(updatedPagesHead);
    lastPageRefreshCheck = currentTime();

    return *commandQueue;
}

static command* checkAndEnqueuePeriodicCommands(command** commandQueue, PeriodicCommand** listOfPeriodicCommands) {
    time_t currentTime = time(NULL);
    PeriodicCommand* periodicCommand = listOfPeriodicCommands;

    while (periodicCommand != NULL) {
        if (periodicCommand->next_time <= currentTime) {
            // Enqueue the command into the commandQueue
            command cmd = *periodicCommand->command;
            commandQueue = addCommmandToQueue(&commandQueue, cmd.function, cmd.argument_1, cmd.argument_2, cmd.argument_3, cmd.argument_4, cmd.argument_5, cmd.argument_6, cmd.argument_7, cmd.argument_8, cmd.argument_9);

            // Update the next execution time
            periodicCommand->next_time = currentTime + periodicCommand->period;
        }
        periodicCommand = periodicCommand->next;
    }

    return &commandQueue;
}

static PeriodicCommand* addPeriodicCommand(PeriodicCommand** listOfPeriodicCommands, command* command, int period) {
    PeriodicCommand* newCommand = (PeriodicCommand*)malloc(sizeof(PeriodicCommand));
    newCommand->command = command;  // Duplicate the command string
    newCommand->period = period;
    newCommand->next_time = time(NULL) + period;  // Set the next execution time
    newCommand->next = NULL;

    // If the list is empty, make the new node the first node
    if (*listOfPeriodicCommands == NULL) {
        *listOfPeriodicCommands = newCommand;
        return *head;
    }

    // Traverse the list to find the last node
    PeriodicCommand* lastNode = *listOfPeriodicCommands;
    while (lastNode->next != NULL) {
        lastNode = lastNode->next;
    }

    // Link the new node after the last node
    lastNode->next = newCommand;
    return *listOfPeriodicCommands;
}

static command* addCommmandToQueue(command** head,  char *function, char *argument_1, char *argument_2, char *argument_3, char *argument_4, char *argument_5, char *argument_6, char *argument_7, char *argument_8, char *argument_9) {
    command* newNode = (command *)malloc(sizeof(command));
    if (!newNode) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }

    // Allocate memory and copy the path and id
    newNode->function = malloc(strlen(function) + 1);
    strcpy(newNode->function, function);

    newNode->argument_1 = malloc(strlen(argument_1) + 1);
    strcpy(newNode->argument_1, argument_1);

    newNode->argument_2 = malloc(strlen(argument_2) + 1);
    strcpy(newNode->argument_2, argument_2);

    newNode->argument_3 = malloc(strlen(argument_3) + 1);
    strcpy(newNode->argument_3, argument_3);

    newNode->argument_4 = malloc(strlen(argument_4) + 1);
    strcpy(newNode->argument_4, argument_4);

    newNode->argument_5 = malloc(strlen(argument_5) + 1);
    strcpy(newNode->argument_5, argument_5);

    newNode->argument_6 = malloc(strlen(argument_6) + 1);
    strcpy(newNode->argument_6, argument_6);

    newNode->argument_7 = malloc(strlen(argument_7) + 1);
    strcpy(newNode->argument_7, argument_7);

    newNode->argument_8 = malloc(strlen(argument_8) + 1);
    strcpy(newNode->argument_8, argument_8);

    newNode->argument_9 = malloc(strlen(argument_9) + 1);
    strcpy(newNode->argument_9, argument_9);


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
    return *head;
}

void removeFirstCommand(command **head) {
    if (*head == NULL) {
        // The list is already empty, nothing to remove
        return;
    }

    // Save the current head node to free it later
    command *temp = *head;

    // Update the head to point to the next node
    *head = (*head)->next;

    // Free the memory of the removed node
    free(temp->function);
    free(temp->argument_1);
    free(temp->argument_2);
    free(temp->argument_3);
    free(temp->argument_4);
    free(temp->argument_5);
    free(temp->argument_6);
    free(temp->argument_7);
    free(temp->argument_8);
    free(temp->argument_9);
    free(temp);
}
