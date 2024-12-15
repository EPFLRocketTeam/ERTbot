/**
 * @file main.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief Contains the program initalisation and loop functions
 *
 * @details Goal of this file is to initalise the program, search for incoming commands (from slack chat,
 *          inline commands on wiki), recognise the command, call the appropriate
 *          function in the features.c file.
 *
 * @warning replaceWord function is still causing memory leaks (responsible for 1/3 of memory leaks)
 */

#include <stdio.h>
#include <unistd.h>
#include "ERTbot_common.h"
#include "ERTbot_command.h"
#include "apiHelpers.h"
#include "timeHelpers.h"
#include "slackAPI.h"

memory chunk;

pageList default_page = {"DefaultID", "DefaultTitle", "DefaultPath", "DefaultDescription", "DefaultContent", "DefaultUpdatedAt", NULL};

char *lastPageRefreshCheck;

command **headOfCommandQueue;

#ifndef TESTING
int main(){
    log_message(LOG_DEBUG, __func__, "\n\nStarting program\n\n");

    initializeApiTokenVariables();
    initialiseSlackCommandStatusMessage();
    lastPageRefreshCheck = getCurrentEDTTimeString();
    headOfCommandQueue = (command **)malloc(sizeof(command *));
    *headOfCommandQueue = NULL;
    int cyclesSinceLastCommand = 0; // reduce number of API calls when "Idling"

    sendMessageToSlack("Wiki-Toolbox is Online");

    while (1){

        headOfCommandQueue = checkForCommand(headOfCommandQueue);
        sleep(1);

        if (*headOfCommandQueue){
            cyclesSinceLastCommand = 0;
            log_message(LOG_DEBUG, __func__, "command received");
            headOfCommandQueue = executeCommand(headOfCommandQueue);
        }

        else{
            cyclesSinceLastCommand++;
            log_message(LOG_DEBUG, __func__, "No command received.");
        }

        if (cyclesSinceLastCommand > 20){
            sleep(1);
        }

        if (cyclesSinceLastCommand > 200){
            sleep(28);
        }
    }
}
#endif
