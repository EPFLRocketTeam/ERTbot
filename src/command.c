/**
 * @file command.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief Contains functions used to handle commands.
 */
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "slackAPI.h"
#include "wikiAPI.h"
#include "sheetAPI.h"
#include "ERTbot_common.h"
#include "ERTbot_features.h"
#include "pageListHelpers.h"
#include "timeHelpers.h"
#include "ERTbot_command.h"

#define MAX_ARGUMENTS 10

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

static PeriodicCommand* addDailyCommand(PeriodicCommand** headOfPeriodicCommands, command* command, char* time_str) {
    log_message(LOG_DEBUG, "Entering function scheduleDailyCommand");

    int hours, minutes;
    sscanf(time_str, "%d:%d", &hours, &minutes); // Parse the time string


    PeriodicCommand* newCommand = (PeriodicCommand*)malloc(sizeof(PeriodicCommand));
    newCommand->command = command;  // Duplicate the command string
    newCommand->period = 86400;
    newCommand->next = NULL;

    time_t now = time(NULL);
    struct tm* now_tm = localtime(&now);

    struct tm next_execution = *now_tm;
    next_execution.tm_hour = hours;
    next_execution.tm_min = minutes;
    next_execution.tm_sec = 0;

    time_t next_execution_time = mktime(&next_execution);

    // If the execution time has already passed today, schedule it for tomorrow
    if (next_execution_time <= now) {
        next_execution_time += 24 * 60 * 60; // Add 24 hours in seconds
    }

    newCommand->next_time = next_execution_time;  // Set the next execution time

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

    log_message(LOG_DEBUG, "Exiting function scheduleDailyCommand");

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
        log_message(LOG_DEBUG, "checking if it is time to queue:%s", periodicCommand->command->function);
        
        /*
        // Allocate space for the time strings
        char strNextTime[26];
        char strCurrentTime[26];
        // Copy the ctime results into separate buffers
        strncpy(strNextTime, ctime(&(periodicCommand->next_time)), sizeof(strNextTime) - 1);
        strncpy(strCurrentTime, ctime(&currentTime), sizeof(strCurrentTime) - 1);
        // Ensure null termination
        strNextTime[sizeof(strNextTime) - 1] = '\0';
        strCurrentTime[sizeof(strCurrentTime) - 1] = '\0';
        log_message(LOG_DEBUG, "Comparing times for periodic commands, next time: %s, current time: %s", strNextTime, strCurrentTime);
        */
       
        if (periodicCommand->next_time <= currentTime) {
            // Enqueue the command into the commandQueue
            command cmd = *periodicCommand->command;
            *commandQueue = addCommandToQueue(commandQueue, cmd.function, cmd.argument_1, cmd.argument_2, cmd.argument_3, cmd.argument_4, cmd.argument_5, cmd.argument_6, cmd.argument_7, cmd.argument_8, cmd.argument_9);

            log_message(LOG_DEBUG, "Periodic command added to queue:%s", cmd.function);

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
    slackMessage* slackMsg = (slackMessage*)malloc(sizeof(slackMessage));;

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
        sendMessageToSlack("Command added to queue");
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
    headOfCommandQueue = checkAndEnqueuePeriodicCommands(headOfCommandQueue, headOfPeriodicCommands);

    log_message(LOG_DEBUG, "Exiting function checkForCommand");
    return headOfCommandQueue;
}

PeriodicCommand** initalizePeriodicCommands(PeriodicCommand** headOfPeriodicCommands){
    log_message(LOG_DEBUG, "Entering function initializePeriodicCommands");

    command* getRyansHomePage = (command*)malloc(sizeof(command));
    breakdownCommand("updateStatsPage", getRyansHomePage);

    command* updateVCD_ST =(command*)malloc(sizeof(command));
    breakdownCommand("updateVCD ST", updateVCD_ST);

    command* updateVCD_GE =(command*)malloc(sizeof(command));
    breakdownCommand("updateVCD GE", updateVCD_GE);

    command* updateVCD_PR =(command*)malloc(sizeof(command));
    breakdownCommand("updateVCD PR", updateVCD_PR);

    command* updateVCD_FD =(command*)malloc(sizeof(command));
    breakdownCommand("updateVCD FD", updateVCD_FD);

    command* updateVCD_RE =(command*)malloc(sizeof(command));
    breakdownCommand("updateVCD RE", updateVCD_RE);

    command* updateVCD_GS =(command*)malloc(sizeof(command));
    breakdownCommand("updateVCD GS", updateVCD_GS);

    command* updateVCD_AV =(command*)malloc(sizeof(command));
    breakdownCommand("updateVCD AV", updateVCD_AV);

    command* updateVCD_PL =(command*)malloc(sizeof(command));
    breakdownCommand("updateVCD PL", updateVCD_PL);

    command* updateReq_ST =(command*)malloc(sizeof(command));
    breakdownCommand("updateReq ST", updateReq_ST);

    command* updateReq_GE =(command*)malloc(sizeof(command));
    breakdownCommand("updateReq GE", updateReq_GE);

    command* updateReq_PR =(command*)malloc(sizeof(command));
    breakdownCommand("updateReq PR", updateReq_PR);

    command* updateReq_FD =(command*)malloc(sizeof(command));
    breakdownCommand("updateReq FD", updateReq_FD);

    command* updateReq_RE =(command*)malloc(sizeof(command));
    breakdownCommand("updateReq RE", updateReq_RE);

    command* updateReq_GS =(command*)malloc(sizeof(command));
    breakdownCommand("updateReq GS", updateReq_GS);

    command* updateReq_AV =(command*)malloc(sizeof(command));
    breakdownCommand("updateReq AV", updateReq_AV);

    command* updateReq_PL =(command*)malloc(sizeof(command));
    breakdownCommand("updateReq PL", updateReq_PL);

    command* updateDRL_ST =(command*)malloc(sizeof(command));
    breakdownCommand("updateDRL ST", updateDRL_ST);

    command* updateDRL_GE =(command*)malloc(sizeof(command));
    breakdownCommand("updateDRL GE", updateDRL_GE);

    command* updateDRL_PR =(command*)malloc(sizeof(command));
    breakdownCommand("updateDRL PR", updateDRL_PR);

    command* updateDRL_FD =(command*)malloc(sizeof(command));
    breakdownCommand("updateDRL FD", updateDRL_FD);

    command* updateDRL_RE =(command*)malloc(sizeof(command));
    breakdownCommand("updateDRL RE", updateDRL_RE);

    command* updateDRL_GS =(command*)malloc(sizeof(command));
    breakdownCommand("updateDRL GS", updateDRL_GS);

    command* updateDRL_AV =(command*)malloc(sizeof(command));
    breakdownCommand("updateDRL AV", updateDRL_AV);

    command* updateDRL_PL =(command*)malloc(sizeof(command));
    breakdownCommand("updateDRL PL", updateDRL_PL);

    headOfPeriodicCommands = (PeriodicCommand**)malloc(sizeof(PeriodicCommand*));
    *headOfPeriodicCommands = NULL;
    *headOfPeriodicCommands = addPeriodicCommand(headOfPeriodicCommands, getRyansHomePage, 3600);
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateVCD_ST, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateVCD_GE, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateVCD_PR, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateVCD_FD, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateVCD_RE, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateVCD_GS, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateVCD_AV, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateVCD_PL, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateReq_ST, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateReq_GE, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateReq_PR, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateReq_FD, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateReq_RE, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateReq_GS, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateReq_AV, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateReq_PL, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateDRL_ST, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateDRL_GE, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateDRL_PR, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateDRL_FD, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateDRL_RE, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateDRL_GS, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateDRL_AV, "06:00");
    *headOfPeriodicCommands = addDailyCommand(headOfPeriodicCommands, updateDRL_PL, "06:00");

    log_message(LOG_DEBUG, "Exiting function initializePeriodicCommands");
    return headOfPeriodicCommands;
}

void breakdownCommand(char* sentence, command* cmd) {
    log_message(LOG_DEBUG, "Entering function breakdownCommand");
    
    char* words[MAX_ARGUMENTS];
    char* token;
    int word_count = 0;

    // Copy the sentence to avoid modifying the original string
    char* sentence_copy = strdup(sentence);

    // Tokenize the sentence
    token = strtok(sentence_copy, " ");
    while (token != NULL && word_count < MAX_ARGUMENTS) {
        words[word_count++] = token;
        token = strtok(NULL, " ");
    }

    // Check if the sentence has more than ten words
    if (word_count > MAX_ARGUMENTS) {
        log_message(LOG_ERROR, "Error: Sentence contains more than ten words.");
        free(sentence_copy);
        return;
    }

    // Initialize the command struct fields to NULL
    cmd->function = NULL;
    cmd->argument_1 = NULL;
    cmd->argument_2 = NULL;
    cmd->argument_3 = NULL;
    cmd->argument_4 = NULL;
    cmd->argument_5 = NULL;
    cmd->argument_6 = NULL;
    cmd->argument_7 = NULL;
    cmd->argument_8 = NULL;
    cmd->argument_9 = NULL;

    // Copy words into struct fields
    for (int i = 0; i < word_count; i++) {
        switch (i) {
            case 0:
                cmd->function = strdup(words[i]);
                break;
            case 1:
                cmd->argument_1 = strdup(words[i]);
                break;
            case 2:
                cmd->argument_2 = strdup(words[i]);
                break;
            case 3:
                cmd->argument_3 = strdup(words[i]);
                break;
            case 4:
                cmd->argument_4 = strdup(words[i]);
                break;
            case 5:
                cmd->argument_5 = strdup(words[i]);
                break;
            case 6:
                cmd->argument_6 = strdup(words[i]);
                break;
            case 7:
                cmd->argument_7 = strdup(words[i]);
                break;
            case 8:
                cmd->argument_8 = strdup(words[i]);
                break;
            case 9:
                cmd->argument_9 = strdup(words[i]);
                break;
            default:
                break;
        }
    }

    free(sentence_copy);
    
    log_message(LOG_DEBUG, "Exiting function breakdownCommand");
}

command** executeCommand(command** commandQueue){
    log_message(LOG_DEBUG, "Entering function executeCommand");

    if((*commandQueue)->function && strcmp((*commandQueue)->function, "shutdown") == 0){ //works
        sendMessageToSlack("Shutting down");

        if(chunk.response){
            free(chunk.response);
        }

        exit(0);
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "updateDRL") == 0){
        syncDrlToSheet(**commandQueue);
        sendMessageToSlack("Finished parsing.");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "updateReq") == 0){
        updateRequirementPage(**commandQueue);
        sendMessageToSlack("Pages updated.");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "updateVCD") == 0){
        updateVcdPage(**commandQueue);
        sendMessageToSlack("VCD page updated");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "refreshOAuthToken") == 0){
        refreshOAuthToken();
        sendMessageToSlack("Token Refreshed");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "createMissingRequirementPages") == 0){
        createMissingRequirementPages(**commandQueue);
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "help") == 0){
        sendMessageToSlack("Here is a list of commands: ");
        sendMessageToSlack("shutdown");
        sendMessageToSlack("-> Description: Will shutdown the ERTbot once all of the commands in the queue are complete.");
        sendMessageToSlack("-----------------");
        sendMessageToSlack("updateVCD");
        sendMessageToSlack("-> Argument (1) (oligatory): acronym of the subsystem you want to update");
        sendMessageToSlack("-> example: updateVCD ST");
        sendMessageToSlack("-----------------");
        sendMessageToSlack("updateReq");
        sendMessageToSlack("-> Argument (1) (oligatory): acronym of the subsystem you want to update (will update all of the requirement pages of the subsystem)");
        sendMessageToSlack("-> or");
        sendMessageToSlack("-> Argument (oligatory): ID of the requirement you want to update");
        sendMessageToSlack("-> example 1: updateReq ST");
        sendMessageToSlack("-> example 2: updateReq 2024_C_SE_ST_REQ_01");
        sendMessageToSlack("-----------------");
        sendMessageToSlack("updateDRL");
        sendMessageToSlack("-> Argument (1) (oligatory): acronym of the subsystem you want to update");
        sendMessageToSlack("-> example: updateDRL ST");
    }

    else{
        sendMessageToSlack("Uknown Command :rayane_side_eyeing:");
    }

    removeFirstCommand(commandQueue);

    log_message(LOG_DEBUG, "Exiting function executeCommand");
    return commandQueue;

}
