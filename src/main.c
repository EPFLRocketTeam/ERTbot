/**
 * @file main.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief Contains the program initalisation and loop functions
 * 
 * @details Goal of this file is to initalise the program, search for incoming commands (from slack chat,
 *          inline commands on wiki and periodic commands), recognise the command, call the appropriate
 *          function in the features.c file.
 * 
 * @todo Separate the different functions in order to only have a initialise and a loop function in 
 *       the main function, write functions for the two other command sending options which are check
 *       latest modified pages and check time, modify the featues.c functions to send back the text
 *       which can then be send to slack or added to a wiki page and add function to handle features.c 
 *       function returned text
 * 
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

//gcc -o wikiToolbox src/main.c src/api.c src/features.c src/githubAPI.c src/helperFunctions.c src/markdownToPDF.c src/slackAPI.c src/stringTools.c src/wikiAPI.c src/sheetAPI.c -I../include -lcurl -lcjson

memory chunk;

pageList default_page = {"DefaultID", "DefaultTitle", "DefaultPath", "DefauDefaultDescription", "DefaultContent", "DefaultUpdatedAt", NULL};

int main(){

    initializeApiTokenVariables();

    // Initialize command struct (used to pass commands from the slack to the program)
    command cmd;
    slackMessage slackMsg;
    slackMsg.message = malloc(200);
    if (slackMsg.message == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    int i = 0;

    sendMessageToSlack("Wiki-Toolbox is Online");

    while(1){

        while(i==0){ //While the last message is a message sent by Wiki-Toolbox

            slackMsg = getSlackMessage(slackMsg);

            if(strcmp(slackMsg.sender, "U06RQCAT0H1") != 0){
                i = 1;
            }
            else{
                printf("No commands sent\n");
                sleep(1);
                free(slackMsg.message);
                free(slackMsg.sender);
                free(slackMsg.timestamp);
            }
            
            if (chunk.response) {
                chunk.response = NULL;
                chunk.size = 0;
            }

        }

        breakdownCommand(slackMsg.message, &cmd);


        if(cmd.function && strcmp(cmd.function, "getPages") == 0){ //works
            getPages(cmd);
        }

        else if(cmd.function && strcmp(cmd.function, "shutdown") == 0){ //works
            sendMessageToSlack("Shutting down");
            return 0;
        }

        else if (cmd.function && strcmp(cmd.function, "buildMap") == 0){ //works
            sendMessageToSlack(buildMap(cmd));
            goto complete;         
        }

        else if (cmd.function && strcmp(cmd.function, "buildLinksTracker") == 0){ //Works
            buildLinksTracker();
            sendMessageToSlack("Page created.");
            goto complete;
        }
        
        else if (cmd.function && strcmp(cmd.function, "updateLinksTracker") == 0){ //Works
            updateLinksTracker( );
        }

        else if (cmd.function && strcmp(cmd.function, "buildLocalGraph") == 0){ //Works
            buildLocalGraph(cmd);
        }

        else if (cmd.function && strcmp(cmd.function, "movePage") == 0){ //Works
            movePage(cmd);
            sendMessageToSlack("Page has been moved and links have been updated");
            goto complete;
            
        }

        else if (cmd.function && strcmp(cmd.function, "syncSheetToDRL") == 0){
            syncSheetToDrl(cmd);
            sendMessageToSlack("Finished parsing.");
            goto complete;
        }

        else if (cmd.function && strcmp(cmd.function, "syncDRLToSheet") == 0){
            syncDrlToSheet(cmd);
            sendMessageToSlack("Finished parsing.");
            goto complete;
        }

        else if (cmd.function && strcmp(cmd.function, "createRequirementPage") == 0){
            createRequirementPage(cmd);
            sendMessageToSlack("Page created.");
            goto complete;
        }

        else if (cmd.function && strcmp(cmd.function, "createVCDPage") == 0){
            createVcdPage(cmd);
            sendMessageToSlack("Page created.");
            goto complete;
        }

        else if (cmd.function && strcmp(cmd.function, "refreshOAuthToken") == 0){
            refreshOAuthToken();
            sendMessageToSlack("Token Refreshed");
            goto complete;
        }

        /*
        else if (cmd.function && strcmp(cmd.function, "getPDF") == 0){
            sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
        }

        if(cmd.function && strcmp(cmd.function, "getZip") == 0){
            getZip(cmd);
        }

        else if (cmd.function && strcmp(cmd.function, "buildImageTracker") == 0){
            //buildImageTracker(cmd);
            sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
        }

        //update

        else if (cmd.function && strcmp(cmd.function, "updateImageTracker") == 0){
            sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
        }

        else if (cmd.function && strcmp(cmd.function, "updateLocalGraph") == 0){
            sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
        }

        else if (cmd.function && strcmp(cmd.function, "updateMap") == 0){
            sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
        }

        else if (cmd.function && strcmp(cmd.function, "notifyMe") == 0){
            sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
        }

        else if (cmd.function && strcmp(cmd.function, "stopNotifyMe") == 0){
            sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
        }

        else if (cmd.function && strcmp(cmd.function, "updateAccronymTracker") == 0){
            sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
        }

        else if (cmd.function && strcmp(cmd.function, "updateBrokenLinksTracker") == 0){
            sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
        }

        else if (cmd.function && strcmp(cmd.function, "updateSync") == 0){
            sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
        }

        else if (cmd.function && strcmp(cmd.function, "spellCheckAI") == 0){
            sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
        }

        else if (cmd.function && strcmp(cmd.function, "summariesAI") == 0){
            sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
        }

        else if (cmd.function && strcmp(cmd.function, "suggestionsAI") == 0){
            sendMessageToSlack("I'm sorry but this function is not ready yet :le_svobovs:");
        }

        else if (cmd.function && strcmp(cmd.function, "replaceText") == 0){
            replaceText(cmd);
        }
        */

        else if (cmd.function && strcmp(cmd.function, "help") == 0){
            sendMessageToSlack("Here is a list of the possible commands: ");
        }

        else{
            sendMessageToSlack("Uknown Command :rayane_side_eyeing:");
        }

    complete:

        free(cmd.function);
        free(cmd.argument_1);
        free(cmd.argument_2);
        free(cmd.argument_3);
        free(cmd.argument_4);
        free(cmd.argument_5);
        free(cmd.argument_6);
        free(cmd.argument_7);
        free(cmd.argument_8);
        free(cmd.argument_9);
        free(slackMsg.message);

        i = 0;
    }

    return 0;

}
