/**
 * @file main.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief Contains the program initalisation and loop functions
 * 
 * @details Goal of this file is to initalise the program, search for incoming commands (from slack chat,
 *          inline commands on wiki and periodic commands), recognise the command, call the appropriate
 *          function in the features.c file.
 * 
 * @todo - Add an option to run through terminal commands instead of slack commands
 *       - Add Logging
 *       - Add Feature: appendToListOfPages and prependToListOfPages
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

//gcc -o wikiToolbox src/main.c src/api.c src/features.c src/githubAPI.c src/helperFunctions.c src/markdownToPDF.c src/slackAPI.c src/stringTools.c src/wikiAPI.c src/sheetAPI.c src/command.c src/log.c -I../include -lcurl -lcjson

memory chunk;

pageList default_page = {"DefaultID", "DefaultTitle", "DefaultPath", "DefauDefaultDescription", "DefaultContent", "DefaultUpdatedAt", NULL};

char *lastPageRefreshCheck;

PeriodicCommand** headOfPeriodicCommands;

command** headOfCommandQueue;


int main(){

    initializeApiTokenVariables();
    headOfPeriodicCommands = initalizePeriodicCommands(headOfPeriodicCommands);
    lastPageRefreshCheck = "none";

    headOfCommandQueue = (command**)malloc(sizeof(command*));
    *headOfCommandQueue = NULL;

    sendMessageToSlack("Wiki-Toolbox is Online");

    log_message(LOG_INFO, "This is an information log test");
    log_message(LOG_ERROR, "This is an error log test");
    log_message(LOG_DEBUG, "This is an debug log test");
    
    while(1){
        fprintf(stderr, "Going to check for commands\n");
        headOfCommandQueue = checkForCommand(headOfCommandQueue, headOfPeriodicCommands);
        fprintf(stderr, "Going to execute command\n");

        if(*headOfCommandQueue){
            fprintf(stderr, "command received\n");
            headOfCommandQueue = executeCommand(headOfCommandQueue);
        }

        else{fprintf(stderr, "No command received.\n");}

        sleep(5);
    }

    sendMessageToSlack("Shutting Down");
    fprintf(stderr, "Shutting Down");
    return 0;
}