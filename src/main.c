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
#include "../include/requirements.h"


//gcc -o wikiToolbox src/main.c src/api.c src/features.c src/githubAPI.c src/helperFunctions.c src/markdownToPDF.c src/slackAPI.c src/stringTools.c src/wikiAPI.c src/sheetAPI.c src/command.c src/log.c src/requirements.c -I../include -lcurl -lcjson

memory chunk;

pageList default_page = {"DefaultID", "DefaultTitle", "DefaultPath", "DefauDefaultDescription", "DefaultContent", "DefaultUpdatedAt", NULL};

char *lastPageRefreshCheck;

PeriodicCommand** headOfPeriodicCommands;

command** headOfCommandQueue;


int main(){
    log_message(LOG_DEBUG, "\n\n\n\n\n\nStarting program\n\n");
    
    initializeApiTokenVariables();
    lastPageRefreshCheck = getCurrentEDTTimeString();
    headOfPeriodicCommands = initalizePeriodicCommands(headOfPeriodicCommands);
    

    headOfCommandQueue = (command**)malloc(sizeof(command*));
    *headOfCommandQueue = NULL;

    sendMessageToSlack("Wiki-Toolbox is Online");
    
    int cyclesSinceLastCommand = 0; //reduce number of API calls when "Idling"

    while(1){
        
        headOfCommandQueue = checkForCommand(headOfCommandQueue, headOfPeriodicCommands);
        
        if(*headOfCommandQueue){
            cyclesSinceLastCommand = 0;
            log_message(LOG_DEBUG, "command received");
            headOfCommandQueue = executeCommand(headOfCommandQueue);
        } 

        else{
            cyclesSinceLastCommand ++;
            log_message(LOG_DEBUG, "No command received.");
        }
        
        if(cyclesSinceLastCommand>20){
            sleep(2);
        }

        if(cyclesSinceLastCommand>200){
            sleep(28);
        }
    }

    sendMessageToSlack("Shutting Down");
    fprintf(stderr, "Shutting Down");
    return 0;
    
    log_message(LOG_DEBUG, "Exiting function main");
}