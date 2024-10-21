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

char* buildMap(command cmd) {
    log_message(LOG_DEBUG, "Entering function buildMap");

    pageList* listOfDaughterPages = NULL;
    listOfDaughterPages = populatePageList(&listOfDaughterPages, "path",  cmd.argument_1); // Parse JSON and populate linked list
    sendMessageToSlack("Finished gathering pages");
    char *map = createMapWBS(&listOfDaughterPages);
    freePageList(&listOfDaughterPages); // Free the memory used by the linked list

    log_message(LOG_DEBUG, "Exiting the buildMap function");
    return map;
}

static char* createMapWBS(pageList** paths) {
    log_message(LOG_DEBUG, "Entering function createMapWBS");
    
    pageList* current = *paths;
    pageList* previous = *paths;
    int isFirstLoop = 1;
    int numberOfParentFolders = 0;
    int baseDepth, currentDepth, numberOfStars;
    
    const char *map_header = "```plantuml\n@startwbs\n<style>\nwbsDiagram {\n  Linecolor black\nBackGroundColor white\n  hyperlinkColor black\n}\n</style>";

    char *map = (char *)malloc(strlen(map_header) + 1);

    strcpy(map, map_header);

    baseDepth = countSlashes(current->path);
    while (current != NULL) {
        currentDepth = countSlashes(current->path);

        if(isFirstLoop == 0){
            //if the current path is not a daughter page of the previous path and the current path is not a sister page of the previous path
            if(strstr(current->path, previous->path) == NULL && strcmp(getDirPath(current->path), getDirPath(previous->path))!=0){
                char *dummyPath = current->path;
                while(strstr(previous->path, dummyPath) == NULL && baseDepth != countSlashes(dummyPath)){
                    numberOfParentFolders++;
                    dummyPath = getDirPath(dummyPath);
                }

                for(int i = numberOfParentFolders - 1; i > 0; i--){
                    char *dirPath = getDirPath(current->path);
                    for(int j = i - 2; j > 0; j--){
                        dirPath = getDirPath(dirPath);
                    }
                    numberOfStars = currentDepth - baseDepth - i + 1;
                    map = appendToString(map, "\n");
                    for(numberOfStars; numberOfStars > 0; --numberOfStars){
                        map = appendToString(map, "*");
                    }
                    map = appendToString(map, "_ ");
                    map = appendToString(map, " ");
                    map = appendToString(map, getDocId(dirPath));
                    map = appendToString(map, "\n");

                }

                numberOfParentFolders = 0;
            }
        }


        numberOfStars = currentDepth - baseDepth + 1;
        map = appendToString(map, "\n");
        for(numberOfStars; numberOfStars > 0; --numberOfStars){
            map = appendToString(map, "*");
        }
        map = appendToString(map, " [[https://rocket-team.epfl.ch/en/");
        map = appendToString(map, current->path);
        map = appendToString(map, " ");
        map = appendToString(map, current->title);
        map = appendToString(map, "]]\n");

        current = current->next;
        if(isFirstLoop == 1){
            isFirstLoop = 0;
            continue;
        }
        previous = previous->next;
    }
    map = appendToString(map, "\n\n\n@endwbs\n```\n\n\n");
    
    log_message(LOG_DEBUG, "Exiting function createMapWBS");
    return map;
}

static int countSlashes(char *str) {
    log_message(LOG_DEBUG, "Entering function countSlashes");
    
    int count = 0;
    
    while (*str != '\0') {
        if (*str == '/') {
            count++;
        }
        str++;
    }

    
    log_message(LOG_DEBUG, "Exiting function countSlashes");
    return count;
}