#include "../headerFiles/struct.h"
#include "../headerFiles/api.h"
#include "../headerFiles/config.h"
#include "../headerFiles/features.h"
#include "../headerFiles/githubAPI.h"
#include "../headerFiles/helperFunctions.h"
#include "../headerFiles/markdownToPDF.h"
#include "../headerFiles/slackAPI.h"
#include "../headerFiles/stringTools.h"
#include "../headerFiles/wikiAPI.h"
#include "../headerFiles/sheetAPI.h"

//gcc -o wikiToolbox sourceFiles/main.c sourceFiles/api.c sourceFiles/features.c sourceFiles/githubAPI.c sourceFiles/helperFunctions.c sourceFiles/markdownToPDF.c sourceFiles/slackAPI.c sourceFiles/stringTools.c sourceFiles/wikiAPI.c sourceFiles/sheetAPI.c -I../headerFiles -lcurl -lcjson

//searches for commmands on the slack channel, parses the command and calls the appropriate function in features.c

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
            buildLinksTracker(cmd);
            sendMessageToSlack("Page created.");
            goto complete;
        }
        
        else if (cmd.function && strcmp(cmd.function, "updateLinksTracker") == 0){ //Works
            updateLinksTracker(cmd);
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
