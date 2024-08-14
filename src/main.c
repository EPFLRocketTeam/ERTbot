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

//gcc -o wikiToolbox src/main.c src/api.c src/features.c src/githubAPI.c src/helperFunctions.c src/markdownToPDF.c src/slackAPI.c src/stringTools.c src/wikiAPI.c src/sheetAPI.c src/command.c -I../include -lcurl -lcjson

memory chunk;

pageList default_page = {"DefaultID", "DefaultTitle", "DefaultPath", "DefauDefaultDescription", "DefaultContent", "DefaultUpdatedAt", NULL};

char *lastPageRefreshCheck;


int main(){
    initialise();
    loop();
    sendMessageToSlack("Shutting Down");
    fprintf(stderr, "Shutting Down");
    return 0;
}

void initialise(){
    initializeApiTokenVariables(); 
    initalizePeriodicCommands();
    lastPageRefreshCheck = "none";

    return;
}

void loop(){
    command* commandQueue = NULL;

    while(1){
        commandQueue = checkForCommand(&commandQueue);
        commandQueue = executeCommand(&commandQueue);
    }

    return;
}