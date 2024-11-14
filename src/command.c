/**
 * @file command.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief Contains functions used to handle commands.
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "slackAPI.h"
#include "wikiAPI.h"
#include "sheetAPI.h"
#include "ERTbot_common.h"
#include "ERTbot_features.h"
#include "pageListHelpers.h"
#include "timeHelpers.h"
#include "ERTbot_command.h"
#include "stringHelpers.h"


#define MAX_ARGUMENTS 10

static void setCommandArgument(char** dest, const char* src, const char* argName);

static PeriodicCommand* addPeriodicCommand(PeriodicCommand** headOfPeriodicCommands_Global, command* command, int period) {
    log_message(LOG_DEBUG, "Entering function addPeriodicCommand");

    PeriodicCommand* newCommand = (PeriodicCommand*)malloc(sizeof(PeriodicCommand));
    newCommand->command = command;  // Duplicate the command string
    newCommand->period = period;
    newCommand->next_time = time(NULL) + period;  // Set the next execution time
    newCommand->next = NULL;

    // If the list is empty, make the new node the first node
    if (*headOfPeriodicCommands_Global == NULL) {
        *headOfPeriodicCommands_Global = newCommand;
        return *headOfPeriodicCommands_Global;
    }

    // Traverse the list to find the last node
    PeriodicCommand* lastNode = *headOfPeriodicCommands_Global;
    while (lastNode->next != NULL) {
        lastNode = lastNode->next;
    }

    // Link the new node after the last node
    lastNode->next = newCommand;
    log_message(LOG_DEBUG, "Exiting function addPeriodicCommand");

    return *headOfPeriodicCommands_Global;
}

static PeriodicCommand* addDailyCommand(PeriodicCommand** headOfPeriodicCommands_Global, command* command, const char* time_str) {
    log_message(LOG_DEBUG, "Entering function scheduleDailyCommand");

    int hours;
    int minutes;
    sscanf(time_str, "%d:%d", &hours, &minutes); // Parse the time string


    PeriodicCommand* newCommand = (PeriodicCommand*)malloc(sizeof(PeriodicCommand));
    newCommand->command = command;  // Duplicate the command string
    newCommand->period = 86400;
    newCommand->next = NULL;

    time_t now = time(NULL);
    const struct tm* now_tm = localtime(&now);

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
    if (*headOfPeriodicCommands_Global == NULL) {
        *headOfPeriodicCommands_Global = newCommand;
        return *headOfPeriodicCommands_Global;
    }

    // Traverse the list to find the last node
    PeriodicCommand* lastNode = *headOfPeriodicCommands_Global;
    while (lastNode->next != NULL) {
        lastNode = lastNode->next;
    }

    // Link the new node after the last node
    lastNode->next = newCommand;

    log_message(LOG_DEBUG, "Exiting function scheduleDailyCommand");

    return *headOfPeriodicCommands_Global;
}

static command* addCommandToQueue(command** head, const char *function, const char *argument) {
    log_message(LOG_DEBUG, "Entering function addCommandToQueue");

    command* newNode = (command *)malloc(sizeof(command));
    if (!newNode) {
        log_message(LOG_ERROR, "Memory allocation error");
        exit(1);
    }

    // Initialize all pointers to NULL
    newNode->function = NULL;
    newNode->argument = NULL;

     // Set function and arguments using the helper function
    setCommandArgument(&newNode->function, function, "function");
    setCommandArgument(&newNode->argument, argument, "argument");

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

    if (temp->argument && temp->argument != NULL){
        free(temp->argument);
    }

    log_message(LOG_DEBUG, "freed all variables");

    free(temp);

    log_message(LOG_DEBUG, "freed struct");

    log_message(LOG_DEBUG, "Exiting function removeFirstCommand");
}

static command** checkAndEnqueuePeriodicCommands(command** commandQueue, PeriodicCommand** headOfPeriodicCommands_Global) {
    log_message(LOG_DEBUG, "Entering function checkAndEnqueuePeriodicCommands");

    time_t currentTime = time(NULL);
    PeriodicCommand* periodicCommand = (*headOfPeriodicCommands_Global);

    while (periodicCommand != NULL) {
        log_message(LOG_DEBUG, "checking if it is time to queue:%s", periodicCommand->command->function);

        if (periodicCommand->next_time <= currentTime) {
            // Enqueue the command into the commandQueue
            command cmd = *periodicCommand->command;
            *commandQueue = addCommandToQueue(commandQueue, cmd.function, cmd.argument);

            log_message(LOG_DEBUG, "Periodic command added to queue:%s", cmd.function);

            // Update the next execution time
            periodicCommand->next_time = currentTime + periodicCommand->period;
        }
        periodicCommand = periodicCommand->next;
    }

    log_message(LOG_DEBUG, "Exiting function checkAndEnquePeriodicCommands");
    return commandQueue;
}

static command** lookForCommandOnSlack(command** headOfPeriodicCommands_Global){
    log_message(LOG_DEBUG, "Entering function lookForCommandonSlack");

    command cmd;
    slackMessage* slackMsg = (slackMessage*)malloc(sizeof(slackMessage));

    if (chunk.response) {
        chunk.response = NULL;
        chunk.size = 0;
    }

    slackMsg = getSlackMessage(slackMsg);



    //If received a message which was not sent by bot, breakdown message into command structure and return command
    if(slackMsg->sender && strcmp(slackMsg->sender, "U06RQCAT0H1") != 0){
        breakdownCommand(slackMsg->message, &cmd);
        log_message(LOG_DEBUG, "Command broke down");
        *headOfPeriodicCommands_Global = addCommandToQueue(headOfPeriodicCommands_Global, cmd.function, cmd.argument);
        log_message(LOG_INFO, "Received a %s command on slack", cmd.function);
        log_message(LOG_DEBUG, "Command added to queue");
    }

    //If last message was sent by bot, free allocated memory and return emtpy command
    else{log_message(LOG_DEBUG, "No commands sent on slack"); }//log.info

    if (chunk.response) {
        chunk.response = NULL;
        chunk.size = 0;
    }

    if(slackMsg->message){
        free(slackMsg->message);
    }

    if(slackMsg->sender){
        free(slackMsg->sender);
    }

    if(slackMsg->timestamp){
        free(slackMsg->timestamp);
    }

    if(slackMsg){
        free(slackMsg);
    }


    log_message(LOG_DEBUG, "Exiting function lookForCommandOnSlack");
    return headOfPeriodicCommands_Global;

}

static command** lookForNewlyUpdatedPages(command** commandQueue){
    log_message(LOG_DEBUG, "Entering function lookForNewlyUpdatedPages");

    pageList* updatedPages = NULL;
    log_message(LOG_DEBUG, "Last Page refresh check happened at: %s", lastPageRefreshCheck);
    updatedPages = populatePageList(&updatedPages, "time", lastPageRefreshCheck);
    pageList* updatedPagesHead = updatedPages;

    while(updatedPages){

        *commandQueue = addCommandToQueue(commandQueue, "onPageUpdate", updatedPages->id);
        log_message(LOG_INFO, "onPageUpdate command has been added to queue for page id: %s", updatedPages->id);
        updatedPages = updatedPages->next;
    }

    freePageList(&updatedPagesHead);
    lastPageRefreshCheck = getCurrentEDTTimeString();

    log_message(LOG_DEBUG, "Exiting function lookForNewlyUpdatedPages");
    return commandQueue;
}

command** checkForCommand(command** headOfCommandQueue_Global, PeriodicCommand** headOfPeriodicCommands_Global){
    log_message(LOG_DEBUG, "Entering function checkForCommand");

    headOfCommandQueue_Global = lookForCommandOnSlack(headOfCommandQueue_Global);
    headOfCommandQueue_Global = checkAndEnqueuePeriodicCommands(headOfCommandQueue_Global, headOfPeriodicCommands_Global);

    log_message(LOG_DEBUG, "Exiting function checkForCommand");
    return headOfCommandQueue_Global;
}

PeriodicCommand** initalizePeriodicCommands(PeriodicCommand** headOfPeriodicCommands_Global){
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

    headOfPeriodicCommands_Global = (PeriodicCommand**)malloc(sizeof(PeriodicCommand*));
    *headOfPeriodicCommands_Global = NULL;
    *headOfPeriodicCommands_Global = addPeriodicCommand(headOfPeriodicCommands_Global, getRyansHomePage, 3600);
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateVCD_ST, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateVCD_GE, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateVCD_PR, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateVCD_FD, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateVCD_RE, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateVCD_GS, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateVCD_AV, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateVCD_PL, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateReq_ST, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateReq_GE, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateReq_PR, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateReq_FD, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateReq_RE, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateReq_GS, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateReq_AV, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateReq_PL, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateDRL_ST, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateDRL_GE, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateDRL_PR, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateDRL_FD, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateDRL_RE, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateDRL_GS, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateDRL_AV, "06:00");
    *headOfPeriodicCommands_Global = addDailyCommand(headOfPeriodicCommands_Global, updateDRL_PL, "06:00");

    log_message(LOG_DEBUG, "Exiting function initializePeriodicCommands");
    return headOfPeriodicCommands_Global;
}

void breakdownCommand(const char* sentence, command* cmd) {
    log_message(LOG_DEBUG, "Entering function breakdownCommand");

    char* words[MAX_ARGUMENTS];
    char* token;
    int word_count = 0;

    // Copy the sentence to avoid modifying the original string
    char* sentence_copy = duplicate_Malloc(sentence);

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
    cmd->argument = NULL;

    // Copy words into struct fields
    for (int i = 0; i < word_count; i++) {
        switch (i) {
            case 0:
                cmd->function = duplicate_Malloc(words[i]);
                break;
            case 1:
                cmd->argument = duplicate_Malloc(words[i]);
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

        exit(0);
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "updateDRL") == 0){
        sendStartingStatusMessage("updateDRL");

        syncDrlToSheet(**commandQueue);
        
        sendCompletedStatusMessage("updateDRL");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "updateReq") == 0){
        sendStartingStatusMessage("updateReq");

        updateRequirementPage(**commandQueue);

        sendCompletedStatusMessage("updateReq");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "updateVCD") == 0){
        sendStartingStatusMessage("updateVCD");
        
        updateVcdPage(**commandQueue);
        
        sendCompletedStatusMessage("updateVCD");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "createMissingRequirementPages") == 0){
        sendStartingStatusMessage("createMissingRequirementPages");

        createMissingRequirementPages(**commandQueue);

        sendCompletedStatusMessage("createMissingRequirementPages");
    }

    else if ((*commandQueue)->function && strcmp((*commandQueue)->function, "sync") == 0){
        sendStartingStatusMessage("sync");
        
        updateCommandStatusMessage("Starting createMissingRequirementPages");
        createMissingRequirementPages(**commandQueue);
        updateCommandStatusMessage("finished createMissingRequirementPages");

        updateCommandStatusMessage("Starting updateDRL");
        syncDrlToSheet(**commandQueue);
        updateCommandStatusMessage("finished updateDRL");

        updateCommandStatusMessage("Starting updateReq");
        updateRequirementPage(**commandQueue);
        updateCommandStatusMessage("finished updateReq");

        updateCommandStatusMessage("Starting updateVCD");
        updateVcdPage(**commandQueue);
        updateCommandStatusMessage("finished updateVCD");

        sendCompletedStatusMessage("sync");
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
        sendMessageToSlack("-----------------");
        sendMessageToSlack("createMissingRequirementPages");
        sendMessageToSlack("-> Argument (1) (oligatory): acronym of the subsystem you want to update");
        sendMessageToSlack("-> example: updateDRL ST");
        sendMessageToSlack("-----------------");
        sendMessageToSlack("sync");
        sendMessageToSlack("-> Description: Fully synchronises all of the requirements by running createMissingRequirementPages, updateDRL, updateReq and updateVCD on its own.");
        sendMessageToSlack("-> Argument (1) (oligatory): acronym of the subsystem you want to update");
        sendMessageToSlack("-> example: sync ST");
    }

    else{
        sendMessageToSlack("Unknown Command :rayane_side_eyeing:");
    }

    removeFirstCommand(commandQueue);


    log_message(LOG_DEBUG, "Exiting function executeCommand");
    return commandQueue;

}

static void setCommandArgument(char** dest, const char* src, const char* argName) {
    if (src && src[0] != '\0') {
        log_message(LOG_DEBUG, "%s added to command", argName);
        size_t len = strlen(src) + 1;
        *dest = malloc(len);
        if (*dest) {
            strlcpy(*dest, src, len);
        } else {
            log_message(LOG_ERROR, "Memory allocation failed for %s", argName);
        }
    }
}