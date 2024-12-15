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

static command* addCommandToQueue(command** head, const char *function, const char *argument) {
    log_function_entry(__func__);

    command* newNode = (command *)malloc(sizeof(command));
    if (!newNode) {
        log_message(LOG_ERROR, __func__, "Memory allocation error");
        exit(1);
    }

    // Initialize all pointers to NULL
    newNode->function = NULL;
    newNode->argument = NULL;

     // Set function and arguments using the helper function
    setCommandArgument(&newNode->function, function, "function");
    setCommandArgument(&newNode->argument, argument, "argument");

    log_message(LOG_DEBUG, __func__, "All arguments added to command struct");
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

    log_message(LOG_DEBUG, __func__, "Command added to queue");

    log_function_exit(__func__);
    return *head;
}

void removeFirstCommand(command **head) {
    log_function_entry(__func__);

    log_message(LOG_DEBUG, __func__, "About to remove first command");

    if (*head == NULL) {
        // The list is already empty, nothing to remove
        return;
    }

    log_message(LOG_DEBUG, __func__, "Creating a temporary pointer to a command");
    // Save the current head node to free it later
    command* temp = *head;

    log_message(LOG_DEBUG, __func__, "Placing the head on to the next command");
    // Update the head to point to the next node
    *head = (*head)->next;

    // Free the memory of the removed node

    if (temp->function && temp->function != NULL){
        free(temp->function);
    }

    if (temp->argument && temp->argument != NULL){
        free(temp->argument);
    }

    log_message(LOG_DEBUG, __func__, "freed all variables");

    free(temp);

    log_message(LOG_DEBUG, __func__, "freed struct");

    log_function_exit(__func__);
}

static command** lookForCommandOnSlack(command** headOfCommandQueue_Global){
    log_function_entry(__func__);

    command cmd;
    slackMessage* slackMsg = (slackMessage*)malloc(sizeof(slackMessage));

    if (chunk.response) {
        chunk.response = NULL;
        chunk.size = 0;
    }

    slackMsg = getSlackMessage(slackMsg);



    //If received a message which was not sent by bot, breakdown message into command structure and return command
    if(slackMsg->message && slackMsg->timestamp && slackMsg->sender  && strcmp(slackMsg->sender, "U06RQCAT0H1") != 0){
        breakdownCommand(slackMsg->message, &cmd);
        log_message(LOG_DEBUG, __func__, "Command broke down");
        *headOfCommandQueue_Global = addCommandToQueue(headOfCommandQueue_Global, cmd.function, cmd.argument);
        log_message(LOG_INFO, __func__, "Received a %s command on slack", cmd.function);
        log_message(LOG_DEBUG, __func__, "Command added to queue");
    }

    //If last message was sent by bot, free allocated memory and return emtpy command
    else{log_message(LOG_DEBUG, __func__, "No commands sent on slack"); }//log.info

    if (chunk.response) {
        chunk.response = NULL;
        chunk.size = 0;
    }

    if(slackMsg->message){
        free(slackMsg->message);
        slackMsg->message = NULL;
    }

    if(slackMsg->sender){
        free(slackMsg->sender);
        slackMsg->sender = NULL;
    }

    if(slackMsg->timestamp){
        free(slackMsg->timestamp);
        slackMsg->timestamp = NULL;
    }

    if(slackMsg){
        free(slackMsg);
    }


    log_function_exit(__func__);
    return headOfCommandQueue_Global;

}

command** checkForCommand(command** headOfCommandQueue_Global){
    log_function_entry(__func__);

    headOfCommandQueue_Global = lookForCommandOnSlack(headOfCommandQueue_Global);

    log_function_exit(__func__);
    return headOfCommandQueue_Global;
}

void breakdownCommand(const char* sentence, command* cmd) {
    log_function_entry(__func__);

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
        log_message(LOG_ERROR, __func__, "Error: Sentence contains more than ten words.");
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

    log_function_exit(__func__);
}

command** executeCommand(command** commandQueue){
    log_function_entry(__func__);

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


    log_function_exit(__func__);
    return commandQueue;

}

static void setCommandArgument(char** dest, const char* src, const char* argName) {
    if (src && src[0] != '\0') {
        log_message(LOG_DEBUG, __func__, "%s added to command", argName);
        size_t len = strlen(src) + 1;
        *dest = malloc(len);
        if (*dest) {
            strlcpy(*dest, src, len);
        } else {
            log_message(LOG_ERROR, __func__, "Memory allocation failed for %s", argName);
        }
    }
}
