/**
 * @file helperFunctions.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief Contains all of the general purpouse helper functions which can not be sorted into one of the other files.
 * 
 * @todo organise the document to make clearer which helper functions are used for which features and the type of helper function
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

#define MAX_ARGUMENTS 10
#define MAX_ARGUMENT_LENGTH 100

char *template_DRL = "# General Design Requirements List\n\n\n# table {.tabset}\n\n## General\n";
char *template_REQ = "";


pageList* addPageToList(pageList** head,  char *id, char *title, char *path, char *description, char *content, char *updatedAt, char *createdAt) {
    log_message(LOG_DEBUG, "Entering function addPageToList");
    
    pageList* newNode = (pageList *)malloc(sizeof(pageList));
    if (!newNode) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }

    // Allocate memory and copy the path and id
    newNode->id = malloc(strlen(id) + 1);
    strcpy(newNode->id, id);

    newNode->title = malloc(strlen(title) + 1);
    strcpy(newNode->title, title);

    newNode->path = malloc(strlen(path) + 1);
    strcpy(newNode->path, path);

    newNode->description = malloc(strlen(description) + 1);
    strcpy(newNode->description, description);

    newNode->content = malloc(strlen(content) + 1);
    strcpy(newNode->content, content);

    newNode->updatedAt = malloc(strlen(updatedAt) + 1);
    strcpy(newNode->updatedAt, updatedAt);

    newNode->createdAt = malloc(strlen(createdAt) + 1);
    strcpy(newNode->createdAt, createdAt);

    newNode->next = NULL;  // New node will be the last node

    // If the list is empty, make the new node the first node
    if (*head == NULL) {
        *head = newNode;
        return *head;
    }

    // Traverse the list to find the last node
    pageList* lastNode = *head;
    while (lastNode->next != NULL) {
        lastNode = lastNode->next;
    }

    // Link the new node after the last node
    lastNode->next = newNode;

    
    log_message(LOG_DEBUG, "Exiting function addPageToList");
    return *head;
}

int countSlashes(char *str) {
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

void prepend_file(char *source_filename, char *destination_filename) {
    log_message(LOG_DEBUG, "Entering function prepend_file");
    
    // Open source file for reading
    FILE *source_file = fopen(source_filename, "r");
    if (source_file == NULL) {
        log_message(LOG_ERROR, "Error opening source file.");
        return;
    }

    // Open destination file for reading and writing
    FILE *destination_file = fopen(destination_filename, "r+");
    if (destination_file == NULL) {
        log_message(LOG_ERROR, "Error opening destination file.");
        fclose(source_file);
        return;
    }

    // Move cursor to the end of destination file
    fseek(destination_file, 0, SEEK_END);

    // Get size of destination file
    long size = ftell(destination_file);

    // Allocate buffer to store destination file contents
    char *buffer = (char *)malloc(size);
    if (buffer == NULL) {
        log_message(LOG_ERROR, "Memory allocation failed.");
        fclose(source_file);
        fclose(destination_file);
        return;
    }

    // Read contents of destination file
    fseek(destination_file, 0, SEEK_SET);
    fread(buffer, 1, size, destination_file);

    // Move cursor back to the beginning of destination file
    fseek(destination_file, 0, SEEK_SET);

    // Write contents of source file to destination file
    char source_buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(source_buffer, 1, sizeof(source_buffer), source_file)) > 0) {
        fwrite(source_buffer, 1, bytesRead, destination_file);
    }

    // Write back the contents of the destination file
    fwrite(buffer, 1, size, destination_file);

    // Free allocated memory and close files
    free(buffer);
    fclose(source_file);
    fclose(destination_file);
    
    log_message(LOG_DEBUG, "Exiting function prepend_file");
}

int zipFolder(char *folderPath) {
    log_message(LOG_DEBUG, "Entering function zipFolder");
    
    char zipCommand[1024];
    snprintf(zipCommand, sizeof(zipCommand), "cd \"%s\" && zip -r \"%s.zip\" .", folderPath, folderPath);

    // Execute the zip command
    int result = system(zipCommand);
    if (result != 0) {
        log_message(LOG_ERROR, "Failed to create zip file for '%s'", folderPath);
        return 1;
    }

    log_message(LOG_ERROR, "Folder '%s' successfully zipped\n", folderPath);
    
    log_message(LOG_DEBUG, "Exiting function zipFolder");
    return 0;
}

void createMissingFolders(char *path) {
    log_message(LOG_DEBUG, "Entering function createMissingFolders");
    
    char *dup_path = strdup(path);  // Duplicate the path to modify
    dup_path = removeLastFolder(dup_path);
    char *token = strtok(dup_path, "/"); // Tokenize by "/"
    char curr_path[1024] = "";  // Current path being processed
    strcat(curr_path, "/");  // Starting with root directory

    // Iterate through each token
    while (token != NULL) {
        strcat(curr_path, token);  // Append the current token to the path
        strcat(curr_path, "/");    // Add a '/' for the next token

        // Create directory if it doesn't exist
        struct stat st = {0};
        if (stat(curr_path, &st) == -1) {
            if (mkdir(curr_path, 0755) != 0) {
                log_message(LOG_ERROR, "Failed to create directory: %s\n", curr_path);
                free(dup_path);
                return;
            } else {
                printf("Created directory: %s\n", curr_path);
            }
        }

        token = strtok(NULL, "/");  // Get the next token
    }

    
    log_message(LOG_DEBUG, "Exiting function createMissingFolders");
    free(dup_path);
}

char* getCurrentEDTTimeString() {
    log_message(LOG_DEBUG, "Entering function getCurrentEDTTimeString");
    
    // Allocate static memory for the ISO 8601 string
    static char iso8601[26];  // Size 26 for "YYYY-MM-DDTHH:MM:SS-04:00\0"
    
    // Get the current time
    time_t now = time(NULL);

    // Convert to local time (in EDT)
    struct tm *edtTime = localtime(&now);

    // Check if we are in Daylight Saving Time (EDT)
    if (edtTime->tm_isdst > 0) {
        // EDT timezone is UTC-4
        strftime(iso8601, sizeof(iso8601), "%Y-%m-%dT%H:%M:%S-04:00", edtTime);
    } else {
        // If not DST, fallback to EST (UTC-5)
        strftime(iso8601, sizeof(iso8601), "%Y-%m-%dT%H:%M:%S-05:00", edtTime);
    }

    
    log_message(LOG_DEBUG, "Exiting function getCurrentEDTTimeString");
    return iso8601;
}

int compareTimes(char* time1, char* time2) {
    log_message(LOG_DEBUG, "Entering function compareTimes");
    
    struct tm tm1, tm2;
    memset(&tm1, 0, sizeof(struct tm));
    memset(&tm2, 0, sizeof(struct tm));

    // Parse time1
    sscanf(time1, "%d-%d-%dT%d:%d:%d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday, 
                                         &tm1.tm_hour, &tm1.tm_min, &tm1.tm_sec);
    tm1.tm_year -= 1900;  // Years since 1900
    tm1.tm_mon--;         // Months are 0-11 in struct tm

    // Parse time2
    sscanf(time2, "%d-%d-%dT%d:%d:%d", &tm2.tm_year, &tm2.tm_mon, &tm2.tm_mday, 
                                         &tm2.tm_hour, &tm2.tm_min, &tm2.tm_sec);
    tm2.tm_year -= 1900;  // Years since 1900
    tm2.tm_mon--;         // Months are 0-11 in struct tm

    // Convert struct tm to time_t
    time_t time1_t = mktime(&tm1);
    time_t time2_t = mktime(&tm2);

    // Compare time_t values
    if (time1_t < time2_t){
        log_message(LOG_DEBUG, "Exiting function compareTimes");
        return -1;
    }
    else if (time1_t > time2_t){
        
        log_message(LOG_DEBUG, "Exiting function compareTimes");
        return 1;
    }
    else{
        
        log_message(LOG_DEBUG, "Exiting function compareTimes");
        return 0;
    }
}

void replaceStringInWiki(pageList** head, char* oldString, char* newString) {
    log_message(LOG_DEBUG, "Entering function replaceStringInWiki");
    
    pageList* current = *head;
    while (current != NULL) {
        current = getPage(&current);
        current->content = replaceWord(current->content, oldString, newString);
        current->content = replaceWord(current->content, "\\", "\\\\");
        current->content = replaceWord(current->content, "\"", "\\\"");
        updatePageContentMutation(current);
        renderMutation(&current);
        current = current->next;
    }

    freePageList(head);
    
    log_message(LOG_DEBUG, "Exiting function replaceStringInWiki");
}

char* createMapWBS(pageList** paths) {
    log_message(LOG_DEBUG, "Entering function createMapWBS");
    
    pageList* current = *paths;
    pageList* previous = *paths;
    int isFirstLoop = 1;
    int numberOfParentFolders = 0;
    int baseDepth, currentDepth, numberOfStars;
    
    char *map = "```plantuml\n@startwbs\n<style>\nwbsDiagram {\n  Linecolor black\nBackGroundColor white\n  hyperlinkColor black\n}\n</style>";
    baseDepth = countSlashes(current->path);
    while (current != NULL) {
        currentDepth = countSlashes(current->path);

        if(isFirstLoop == 0){
            //fprintf(stderr, "Going to check paths\n");
            //if the current path is not a daughter page of the previous path and the current path is not a sister page of the previous path
            if(strstr(current->path, previous->path) == NULL && strcmp(getDirPath(current->path), getDirPath(previous->path))!=0){
                //fprintf(stderr, "%s is not contained in %s\n", current->path, previous->path);
                char *dummyPath = current->path;
                while(strstr(previous->path, dummyPath) == NULL && baseDepth != countSlashes(dummyPath)){
                    //fprintf(stderr, "%s is not contained in %s\n", dummyPath, previous->path);
                    numberOfParentFolders++;
                    dummyPath = getDirPath(dummyPath);
                }

                for(int i = numberOfParentFolders - 1; i > 0; i--){
                    char *dirPath = getDirPath(current->path);
                    for(int j = i - 2; j > 0; j--){
                        dirPath = getDirPath(dirPath);
                    }
                    numberOfStars = currentDepth - baseDepth - i + 1;
                    map = appendStrings(map, "\n");
                    for(numberOfStars; numberOfStars > 0; --numberOfStars){
                        map = appendStrings(map, "*");
                    }
                    map = appendStrings(map, " ");
                    map = appendStrings(map, getDocId(dirPath));
                    map = appendStrings(map, "\n");

                }

                numberOfParentFolders = 0;
            }
        }


        numberOfStars = currentDepth - baseDepth + 1;
        map = appendStrings(map, "\n");
        for(numberOfStars; numberOfStars > 0; --numberOfStars){
            map = appendStrings(map, "*");
        }
        map = appendStrings(map, " [[https://rocket-team.epfl.ch/en/");
        map = appendStrings(map, current->path);
        map = appendStrings(map, " ");
        map = appendStrings(map, current->title);
        map = appendStrings(map, "]]\n");

        current = current->next;
        if(isFirstLoop == 1){
            isFirstLoop = 0;
            continue;
        }
        previous = previous->next;
    }
    map = appendStrings(map, "\n\n\n@endwbs\n```\n\n\n");
    
    log_message(LOG_DEBUG, "Exiting function createMapWBS");
    return map;
}

char* createList(char *list, pageList** sectionTitle, pageList* links){
    log_message(LOG_DEBUG, "Entering function createList");
    
    char *tempList = list;
    tempList = appendStrings(tempList, "\\\\n\\\\n");
    tempList = appendStrings(tempList, "## [");
    tempList = appendStrings(tempList, (*sectionTitle)->title);
    tempList = appendStrings(tempList, "](/");
    tempList = appendStrings(tempList, (*sectionTitle)->path);
    tempList = appendStrings(tempList, ")");
    //tempList = appendStrings(tempList, "\\\\n");
    while (links != NULL) {
        tempList = appendStrings(tempList, "\\\\n");
        tempList = appendStrings(tempList, links->path);
        links = links->next;
    }

    freePageList(&links);

    
    log_message(LOG_DEBUG, "Exiting function createList");

    return tempList;
}

char* updateList(char *list, pageList *sectionTitle, pageList *links) {
    log_message(LOG_DEBUG, "Entering function updateList");
    
    // Ensure proper memory allocation for the tempList
    pageList *link = links;
    char *tempList = sectionTitle->path;
    fprintf(stderr, "path:%s\n", sectionTitle->path);

    tempList = appendStrings(tempList, ")\\n" );

    
    link = links;
    while (link != NULL) {

        tempList = appendStrings(tempList, link->path);
        tempList = appendStrings(tempList, "\\n");
        link = link->next;
    }

    char *temp = appendStrings(sectionTitle->path, ")");

    // Find and replace old list
    char *startPtr = strstr(list, temp);
    //startPtr += 1;
    if (startPtr == NULL) {
        fprintf(stderr, "Section title: %s not found\n", sectionTitle->title);
        free(tempList);
        return list;
    }

    char *endPtr = strstr(startPtr, "\\n\\n");
    endPtr +=1;
    if (endPtr == NULL) {
        fprintf(stderr, "End of section not found\n");
        free(tempList);
        return list;
    }

    fprintf(stderr, "About to update %s to:\n %s\n", sectionTitle->title, tempList);

    list = replaceParagraph(list, tempList, startPtr, endPtr);

    //fprintf(stderr, "New list is %s", list);

    freePageList(&links);
    free(tempList);
    
    log_message(LOG_DEBUG, "Exiting function updateList");
    return list;
}

wikiFlag* parseFlags(char* text) {
    log_message(LOG_DEBUG, "Entering function parseFlags");
    
    wikiFlag* head = NULL;
    wikiFlag* current = NULL;
    char* start = text;
    char* end = text;
    int flagCount = 0;
    command cmd;
    wikiFlag *newFlag = (wikiFlag*)malloc(sizeof(wikiFlag));
    
    while (*end != '\0') {
        //log_message(LOG_DEBUG, "Current chars: %c%c%c%c", *end, *(end+1), *(end+2), *(end+3));
        if (*end == '<' && *(end + 1) == '!' && *(end + 2) == '-' && *(end + 3) == '-') {
            log_message(LOG_DEBUG, "Found a comment");

            flagCount ++;
            start = end;
            end += 4; // Move to the first character after '<!--'
            
            // Finding the end of the comment
            while (*end != '\0' && !(*end == '-' && *(end + 1) == '-' && *(end + 2) == '>')) {
                end++;
            }
            
            if (*end == '\0') break; // Reached the end of the text
            
            breakdownCommand(extractParagraphWithPointerDelimiters(text, start+4 , end-1), &cmd);

            log_message(LOG_DEBUG, "Wiki command found in when parsing the flags Function:\"%s\", Command\"%s\"", cmd.function, cmd.argument_1);
            
            
            newFlag->cmd = cmd;

            if (flagCount % 2 != 0){ //odd
                newFlag->pointerToEndOfFirstMarker = (char*)(end + 3);
                log_message(LOG_DEBUG, "Set pointerToEndOfFirstMarker to: \"%ld\"", (long)newFlag->pointerToEndOfFirstMarker);
            }

            if (flagCount % 2 == 0){ //even
                newFlag->pointerToBeginningOfSecondMarker = (char*)(start - 1);
                log_message(LOG_DEBUG, "Set pointerToBeginningOfSecondMarker to: \"%ld\"", (long)newFlag->pointerToBeginningOfSecondMarker);
                log_message(LOG_DEBUG, "pointerToEndOfFirstMarker is: \"%ld\"", (long)newFlag->pointerToEndOfFirstMarker);

                // Adding to the linked list
                newFlag->next = NULL;
                if (head == NULL) {
                    head = newFlag;
                    current = newFlag;
                    wikiFlag *newFlag = (wikiFlag*)malloc(sizeof(wikiFlag));
                } else {
                    current->next = newFlag;
                    current = newFlag;
                    wikiFlag *newFlag = (wikiFlag*)malloc(sizeof(wikiFlag));
                }
            }
            
        }
        end++;
    }
    
    log_message(LOG_DEBUG, "Exiting function parseFlags");
    return head;
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

char* createLocalGraphMindMap(pageList** tempPage, pageList** incomingPaths, pageList** outgoingPaths){
    log_message(LOG_DEBUG, "Entering function createLocalGraphMindMap");
    
    pageList *currentIncomingLink = *incomingPaths;
    pageList *currentOutgoingLink = *outgoingPaths;
    char *localGraph;
    localGraph = "```plantuml\n@startmindmap\n<style>\nmindmapDiagram {\n  .myStyle*{\n Linecolor black\n	BackGroundColor white\n  hyperlinkColor black\n}\n}\n</style>";;
    localGraph = appendStrings(localGraph, "\n+");
    localGraph = appendStrings(localGraph, " [[https://rocket-team.epfl.ch/en/");
    localGraph = appendStrings(localGraph, (*tempPage)->path);
    localGraph = appendStrings(localGraph, " ");
    localGraph = appendStrings(localGraph, (*tempPage)->title);
    localGraph = appendStrings(localGraph, "]]\n");

    while (currentIncomingLink != NULL) {
        localGraph = appendStrings(localGraph, "\n--");
        localGraph = appendStrings(localGraph, " [[https://rocket-team.epfl.ch/en/");
        localGraph = appendStrings(localGraph, currentIncomingLink->path);
        localGraph = appendStrings(localGraph, " ");
        localGraph = appendStrings(localGraph, currentIncomingLink->title);
        localGraph = appendStrings(localGraph, "]]\n");
        currentIncomingLink = currentIncomingLink->next;

    }
    //fprintf(stderr, "local graph: %s\n", localGraph);

    currentOutgoingLink = currentOutgoingLink->next;

    while(currentOutgoingLink != NULL){
        localGraph = appendStrings(localGraph, "\n++");
        localGraph = appendStrings(localGraph, " [[https://rocket-team.epfl.ch/en/");
        localGraph = appendStrings(localGraph, currentOutgoingLink->path);
        localGraph = appendStrings(localGraph, " ");
        localGraph = appendStrings(localGraph, getDocId(currentOutgoingLink->path));
        localGraph = appendStrings(localGraph, "]]\n");
        currentOutgoingLink = currentOutgoingLink->next;
    }

    localGraph = appendStrings(localGraph, "\n\n\n@endmindmap\n```\n\n\n");

    freePageList(&currentIncomingLink);
    freePageList(&currentOutgoingLink);

    //fprintf(stderr, "local graph: %s\n", localGraph);
    
    log_message(LOG_DEBUG, "Exiting function createLocalGraphMindMap");
    return localGraph;
}

void freePageList(pageList** head) {
    log_message(LOG_DEBUG, "Entering function freePageList");
    
    while (*head) {
        pageList* temp = *head;
        *head = (*head)->next;
        
        // Debugging prints
        if (temp->id){
            fprintf(stderr, "Freeing id: %p\n", (void*)temp->id);
            free(temp->id);
        }
        if (temp->title){
            fprintf(stderr, "Freeing title: %p\n", (void*)temp->title);
            free(temp->title);
        }
        if (temp->path){
            fprintf(stderr, "Freeing path: %p\n", (void*)temp->path);
            free(temp->path);
        }
        if (temp->description){
            fprintf(stderr, "Freeing description: %p\n", (void*)temp->description);
            free(temp->description);
        }
        if (temp->content){
            fprintf(stderr, "Freeing content: %p\n", (void*)temp->content);
            free(temp->content);
        }
        if (temp->updatedAt){
            fprintf(stderr, "Freeing updatedAt: %p\n", (void*)temp->updatedAt);
            free(temp->updatedAt);
        }
        if (temp->createdAt){ 
            printf("Freeing createdAt: %p\n", (void*)temp->createdAt);
            free(temp->createdAt);
        }
        
        fprintf(stderr, "about to free temp\n");
        free(temp);
        fprintf(stderr, "freed temp\n");
    }

    
    log_message(LOG_DEBUG, "Exiting function freePageList");
}

void printAcronymsToFile(char* pathToAccronymList, char *str) {
    log_message(LOG_DEBUG, "Entering function printAcronymsToFile");
    
    int len = strlen(str);
    int i;
    int j = 1;
    char acronym[11]; // Maximum acronym length is 10, plus 1 for null terminator

    for (i = 0; i < len-1; i++) {
        // Check if current character is uppercase letter
        if (str[i] == '('){
            while (str[i] != ')'){
                i++;
            }
        }

        if (str[i] == '`' && str[i+1] == '`' && str[i+2] == '`'){
            i++;
            while (!(str[i] == '`' && str[i+1] == '`' && str[i+2] == '`')){
                i++;
            }
        }

        if (isupper(str[i])) {
            // Check for words of length 2 to 10
            while(isupper(str[i+j]) && j < 9 && str[i+j] != ' '){
                j++;
            }
            if(j>1){
                strncpy(acronym, &str[i], j);
                acronym[j] = '\0';
            }
            i = i+j;
            j = 1;
        }
    }
    
    log_message(LOG_DEBUG, "Exiting function printAcronymsToFile");
}

void sortWords(char words[MAX_WORDS][MAX_WORD_LENGTH], int numWords) {
    log_message(LOG_DEBUG, "Entering function sortWords");
    
    // Bubble sort algorithm
    for (int i = 0; i < numWords - 1; i++) {
        for (int j = 0; j < numWords - i - 1; j++) {
            if (strcmp(words[j], words[j + 1]) > 0) {
                char temp[MAX_WORD_LENGTH];
                strcpy(temp, words[j]);
                strcpy(words[j], words[j + 1]);
                strcpy(words[j + 1], temp);
            }
        }
    }
    
    log_message(LOG_DEBUG, "Exiting function sortWords");
}

void removeDuplicatesAndSort(char *filename) {
    log_message(LOG_DEBUG, "Entering function removeDuplicatesAndSort");
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        log_message(LOG_ERROR, "Error opening file.");
        return;
    }

    char words[MAX_WORDS][MAX_WORD_LENGTH];
    int numWords = 0;

    // Read words from the file and store them in an array
    while (fgets(words[numWords], MAX_WORD_LENGTH, file) != NULL) {
        // Remove newline character
        words[numWords][strcspn(words[numWords], "\n")] = '\0';
        numWords++;
    }

    fclose(file);

    // Sort the words alphabetically
    sortWords(words, numWords);

    // Open file for writing
    file = fopen(filename, "w");
    if (file == NULL) {
        log_message(LOG_ERROR, "Error opening file.");
        return;
    }

    // Write unique words back to the file
    for (int i = 0; i < numWords; i++) {
        if (i == 0 || strcmp(words[i], words[i - 1]) != 0) {
            fprintf(file, "%s\n", words[i]);
        }
    }

    fclose(file);
    
    log_message(LOG_DEBUG, "Exiting function removeDuplicatesAndSort");
}

pageList* findPageLinks(char *content, pageList **links) {
    log_message(LOG_DEBUG, "Entering function findPageLinks");
    
    char *startFlag1 = "[";
    char *startFlag2 = "](";
    char *endFlag = ")";
    char *ptr = content;

    while (*ptr) {
        // Find the closest "]("
        char *linkStart = strstr(ptr, startFlag2);
        if (!linkStart) break; // No more links

        // Find the closest ")"
        char *linkEnd = strstr(linkStart, endFlag);
        if (!linkEnd) break; // No more links

        // Find the closest "["
        char *titleStart = linkStart;
        while (titleStart > content && *titleStart != '[') {
            titleStart--;
        }
        if (titleStart <= content) break; // No valid "[" found

        titleStart++; // Move past the '['

        // Calculate title length and link length
        int titleLength = linkStart - titleStart;
        int linkLength = linkEnd - (linkStart + strlen(startFlag2));

        if (titleLength <= 0 || linkLength <= 0) {
            ptr = linkEnd + strlen(endFlag);
            continue;
        }

        // Allocate memory for the title and link
        char *title = (char *)malloc((titleLength + 1) * sizeof(char));
        char *link = (char *)malloc((linkLength + 1) * sizeof(char));
        if (!title || !link) {
            fprintf(stderr, "Memory allocation error\n");
            if (title) free(title);
            if (link) free(link);
            freePageList(links);
            return *links;
        }

        // Copy title and link
        strncpy(title, titleStart, titleLength);
        title[titleLength] = '\0';
        strncpy(link, linkStart + strlen(startFlag2), linkLength);
        link[linkLength] = '\0';

        // Check if link is valid (you might need to refine this check)
        if (strstr(link, ".") == NULL) {
            *links = addPageToList(links, "", title, link, "", "", "", "");
            fprintf(stderr, "found link %s %s\n", link, title);
        }

        free(title);
        free(link);
        ptr = linkEnd + strlen(endFlag);
    }

    
    log_message(LOG_DEBUG, "Exiting function findPageLinks");
    return *links;
}

pageList* findImageLinks(char *input, pageList** head) {
    log_message(LOG_DEBUG, "Entering function findImageLinks");
    
    pageList* imageLinks = *head;
    int count = 0;
    char *startFlag1 = "![";
    char *startFlag2 = "](";
    char *endFlag = ")";
    char *ptr = input;

    while (*ptr) {
        ptr = strstr(ptr, startFlag1);
        if (!ptr) break;

        ptr += strlen(startFlag1);
        char *linkStart = strstr(ptr, startFlag2);
        if (!linkStart) {
            fprintf(stderr, "Error: Missing startFlag2 after startFlag1\n");
            freePageList(&imageLinks);
            return imageLinks;
        }

        linkStart += strlen(startFlag2);
        char *linkEnd = strstr(linkStart, endFlag);
        if (!linkEnd) {
            fprintf(stderr, "Error: Missing endFlag after linkStart\n");
            freePageList(&imageLinks);
            return imageLinks;
        }

        // Allocate memory for the link substring
        int linkLen = linkEnd - linkStart;
        char *link = (char *)malloc((linkLen + 1) * sizeof(char));
        if (!link) {
            fprintf(stderr, "Memory allocation error\n");
            freePageList(&imageLinks);
            return imageLinks;
        }

        strncpy(link, linkStart, linkLen);
        link[linkLen] = '\0';
        imageLinks = addPageToList(&imageLinks, link, "", "", "", "", "", "");

        count++ ;

        ptr = linkEnd + strlen(endFlag);
    }
    
    log_message(LOG_DEBUG, "Exiting function findImageLinks");
    return imageLinks;
}

void filterLinks(pageList** head) {
    log_message(LOG_DEBUG, "Entering function filterLinks");
    
    pageList* current = *head;
    while (current != NULL) {
        current->path = removeAfterSpace(current->path);
        current = current->next;
    }

    
    log_message(LOG_DEBUG, "Exiting function filterLinks");
}

void printPages(pageList** head) {
    log_message(LOG_DEBUG, "Entering function printPages");
    
    pageList* current = *head;
    while (current != NULL) {
        sleep(2);
        current = getPage(&current);
        sendMessageToSlack("Page Title: \n");
        sendMessageToSlack(current->title);
        sendMessageToSlack("Page Content: \n");
        sendMessageToSlack(current->content);
        sendMessageToSlack("\n\n");
        
        current = current->next;
    }
    
    log_message(LOG_DEBUG, "Exiting function printPages");
}

pageList* findIncomingLinks(pageList** head, char *linkTrackerContent, char *subjectPagePath) {
    log_message(LOG_DEBUG, "Entering function findIncomingLinks");
    
    pageList *incomingLinks = *head;
    char *contentCopy = strdup(linkTrackerContent);
    if (contentCopy == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    char *sectionStart = contentCopy;
    sectionStart = strstr(sectionStart, "## [");
    sectionStart += 4;
    char *nextSectionStart = strstr(sectionStart, "## [");

    while (sectionStart != NULL) {
        char *titleStart = sectionStart + 4;
        char *titleEnd = strstr(titleStart, "](/");
        if (titleEnd == NULL) break;

        char *pathStart = titleEnd + 3;
        
        char *pathEnd = strstr(pathStart, ")");
        if (pathEnd == NULL) break;

        char *linksStart = pathEnd;
        char *link = strstr(linksStart, "\\n/");
        
        while (linksStart != NULL && link != NULL && link <= nextSectionStart) {
            link += 3;
            char *linkEnd = strstr(link, "\\n");
            *linkEnd = '\0';

            if (linkEnd == NULL) {
                linkEnd = contentCopy + strlen(contentCopy);
            }

            if (strcmp(link, subjectPagePath) == 0) {
                *titleEnd = '\0';
                *pathEnd = '\0';
                fprintf(stderr, "Found a reference in title: %s, path:%s\n", titleStart, pathStart);
                incomingLinks = addPageToList(&incomingLinks, "", titleStart, pathStart, "", "", "", "");
                break;
            }
            link = strstr(linkEnd + 1, "\\n/");
            linksStart = strstr(linksStart, "\\n/");

        }
        
        sectionStart = nextSectionStart;

        if(nextSectionStart != NULL){
            nextSectionStart = strstr(nextSectionStart + 1, "## [");
        }

        else{
            break;
        }
        
        
    }

    free(contentCopy);
    
    log_message(LOG_DEBUG, "Exiting function findIncomingLinks");
    return incomingLinks;
}

pageList* findOutgoingLinks(pageList** head, char *linkTrackerContent, char *subjectPagePath) {
    log_message(LOG_DEBUG, "Entering function findOutgoingLinks");
    
    pageList *outgoingLinks = *head;
    char *contentCopy = strdup(linkTrackerContent);
    if (contentCopy == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    char *sectionStart = contentCopy;
    sectionStart = strstr(sectionStart, "## [");
    sectionStart += 4;
    char *nextSectionStart = strstr(sectionStart, "## [");

    while (sectionStart != NULL) {
        char *titleStart = sectionStart + 4;
        char *titleEnd = strstr(titleStart, "](/");
        if (titleEnd == NULL) break;

        char *pathStart = titleEnd + 3;
        
        char *pathEnd = strstr(pathStart, ")");
        if (pathEnd == NULL) break;

        char *linksStart = pathEnd;

        *titleEnd = '\0';
        *pathEnd = '\0';

        
        if(strcmp(pathStart, subjectPagePath) == 0){
            char *link = strstr(linksStart+1, "n/");
        
            while (link != NULL && link <= nextSectionStart) {
                link += 2;
                char *linkEnd = strstr(link, "\\n");
                *linkEnd = '\0';
                if (linkEnd == NULL) {
                    linkEnd = contentCopy + strlen(contentCopy);
                }
                outgoingLinks = addPageToList(&outgoingLinks, "", "", link, "", "", "", "");
                link = strstr(linkEnd + 1, "n/");

            }
        
            break;
        
        }
        sectionStart = nextSectionStart;

        if(nextSectionStart != NULL){
            nextSectionStart = strstr(nextSectionStart + 1, "## [");
        }
        else{
            break;
        }
    }
    free(contentCopy);
    
    log_message(LOG_DEBUG, "Exiting function findOutgoingLinks");
    return outgoingLinks;
}

void addRequirementToCjsonObject(cJSON *requirements, char *idStr, char *pathStr, char *nameStr, char *descriptionStr){
    log_message(LOG_DEBUG, "Entering function addRequirementToCjsonObject");
    
    cJSON *id = NULL;
    cJSON *path = NULL;
    cJSON *name = NULL;
    cJSON *description = NULL;
    cJSON *requirement = NULL;

    requirement = cJSON_CreateObject();

    id = cJSON_CreateString(idStr);
    path = cJSON_CreateString(pathStr);
    name = cJSON_CreateString(nameStr);
    description = cJSON_CreateString(descriptionStr);

    cJSON_AddItemToArray(requirements, requirement);

    cJSON_AddItemToObject(requirement, "id", id);
    cJSON_AddItemToObject(requirement, "path", path);
    cJSON_AddItemToObject(requirement, "name", name);
    cJSON_AddItemToObject(requirement, "description", description);

    
    log_message(LOG_DEBUG, "Exiting function addRequirementToCjsonObject");

}

void parseRequirementsList(cJSON* requirements, char *content) {
    log_message(LOG_DEBUG, "Entering function parseRequirementsList");
    
    char *idStartFlag = "\\n- [";
    char *pathStartFlag = "](/";
    char *nameStartFlag = ") **";
    char *descriptionStartFlag = "**\\n";
    char *descriptionEndFlag = "\\n";
    char *ptr = content;

    //fprintf(stderr, "Content received:\n\n%s\n\n", content);

    while (*ptr) {
        char *idStart = strstr(ptr, idStartFlag);
        if (idStart == NULL){
            fprintf(stderr, "did not find idStart, Breaking\n");
            break; // No more links
        }

        char *pathStart = strstr(idStart, pathStartFlag);
        if (pathStart == NULL){
            fprintf(stderr, "did not find pathStart, Breaking\n");
            break; // No more links
        }

        char *nameStart = strstr(pathStart, nameStartFlag);
        if (nameStart == NULL){
            fprintf(stderr, "did not find nameStart, Breaking\n");
            break; // No more links
        }

        char *descriptionStart = strstr(nameStart, descriptionStartFlag);
        if (descriptionStart == NULL){
            fprintf(stderr, "did not find descriptionStart, Breaking\n");
            break; // No more links
        }

        char *descriptionEnd = strstr(descriptionStart + strlen(descriptionStartFlag), descriptionEndFlag);
        if (descriptionEnd == NULL){
            fprintf(stderr, "did not find descriptionEnd, Breaking\n");
            break; // No more links
        }

        // Calculate title length and link length
        int idLength = pathStart - idStart - strlen(idStartFlag);
        int pathLength = nameStart - pathStart - strlen(pathStartFlag);
        int nameLength = descriptionStart - nameStart - strlen(nameStartFlag);
        int descriptionLength = descriptionEnd - descriptionStart - strlen(descriptionStartFlag);

        if (idLength <= 0 || pathLength <= 0 || nameLength <= 0 || descriptionLength <= 0) {
            ptr = descriptionEnd + strlen(descriptionEndFlag);
            continue;
        }

        // Allocate memory for the title and link
        char *id = (char *)malloc((idLength + 1) * sizeof(char));
        char *path = (char *)malloc((pathLength + 1) * sizeof(char));
        char *name = (char *)malloc((nameLength + 1) * sizeof(char));
        char *description = (char *)malloc((descriptionLength + 1) * sizeof(char));
        if (!id || !path || !name || !description) {
            fprintf(stderr, "Memory allocation error\n");
            if (id) free(id);
            if (path) free(path);
            if (name) free(name);
            if (description) free(description);
        }

        // Copy title and link
        strncpy(id, idStart + strlen(idStartFlag), idLength);
        id[idLength] = '\0';
        strncpy(path, pathStart + strlen(pathStartFlag), pathLength);
        path[pathLength] = '\0';
        strncpy(name, nameStart + strlen(nameStartFlag), nameLength);
        name[nameLength] = '\0';
        strncpy(description, descriptionStart + strlen(descriptionStartFlag), descriptionLength);
        description[descriptionLength] = '\0';

        
        //fprintf(stderr, "found requirement \nid: %s\npath: %s\nname: %s\ndescription: %s\n", id, path, name, description);
        addRequirementToCjsonObject(requirements, id, path, name, description);
        

        free(id);
        free(path);
        free(name);
        free(description);
        ptr = descriptionEnd;
    }
    
    log_message(LOG_DEBUG, "Exiting function parseRequirementsList");
    return;
}

char* parseJSONRequirementListInToArray(cJSON* requirements){
    log_message(LOG_DEBUG, "Entering function parseJSONRequirementListInToArray");
    

    // Get the number of requirements
    int req_count = cJSON_GetArraySize(requirements);

    // Calculate the required buffer size for the output string
    size_t buffer_size = 1024; // Initial size, will be adjusted as needed
    for (int i = 0; i < req_count; i++) {
        cJSON *req = cJSON_GetArrayItem(requirements, i);
        const char *id = cJSON_GetObjectItem(req, "id")->valuestring;
        const char *path = cJSON_GetObjectItem(req, "path")->valuestring;
        const char *name = cJSON_GetObjectItem(req, "name")->valuestring;
        const char *description = cJSON_GetObjectItem(req, "description")->valuestring;
        buffer_size += strlen(id) + strlen(path) + strlen(name) + strlen(description) + 20; // Adding extra space for formatting
    }

    // Allocate memory for the output string
    char *output_str = (char *)malloc(buffer_size * sizeof(char));
    if (!output_str) {
        log_message(LOG_ERROR, "Memory allocation failed");
        return NULL;
    }
    output_str[0] = '\0'; // Initialize the string

    // Build the output string
    strcat(output_str, "[");
    for (int i = 0; i < req_count; i++) {
        cJSON *req = cJSON_GetArrayItem(requirements, i);
        const char *id = cJSON_GetObjectItem(req, "id")->valuestring;
        const char *path = cJSON_GetObjectItem(req, "path")->valuestring;
        const char *name = cJSON_GetObjectItem(req, "name")->valuestring;
        const char *description = cJSON_GetObjectItem(req, "description")->valuestring;

        char req_str[512]; // Buffer for the current requirement string
        snprintf(req_str, sizeof(req_str), "[\"%s\", \"%s\", \"%s\", \"%s\"]", id, path, name, description);

        if (i > 0) {
            strcat(output_str, ", ");
        }
        strcat(output_str, req_str);
    }
    strcat(output_str, "]");

    // Clean up the JSON object

    //fprintf(stderr, "%s\n\n\n\n", output_str);

    
    log_message(LOG_DEBUG, "Exiting function parseJSONRequirementListInToArray");
    return output_str;

}

cJSON *parseArrayIntoJSONRequirementList(char *input_str) {
    log_message(LOG_DEBUG, "Entering function parseArrayIntoJSONRequirementList");
    

    fprintf(stderr, "input_str: %s\n", input_str);

    // Parse the input string as JSON
    cJSON *input_json = cJSON_Parse(input_str);
    if (!input_json) {
        log_message(LOG_ERROR, "Error parsing input string as JSON object");
        return NULL;
    }

    // Extract the "values" array from the JSON object
    cJSON *values_array = cJSON_GetObjectItemCaseSensitive(input_json, "values");
    if (!cJSON_IsArray(values_array)) {
        log_message(LOG_ERROR, "Error: values is not a JSON array");
        cJSON_Delete(input_json);
        return NULL;
    }

    // Create a JSON object to hold the requirements
    cJSON *json = cJSON_CreateObject();
    if (!json) {
        log_message(LOG_ERROR, "Error creating JSON object");
        cJSON_Delete(input_json);
        return NULL;
    }

    // Create a JSON array to hold the requirement objects
    cJSON *requirements = cJSON_CreateArray();
    if (!requirements) {
        log_message(LOG_ERROR, "Error creating JSON array");
        cJSON_Delete(json);
        cJSON_Delete(input_json);
        return NULL;
    }

    // Add the requirements array to the JSON object
    cJSON_AddItemToObject(json, "requirements", requirements);

    // Iterate over the input array of arrays and create JSON objects for each requirement
    int num_reqs = cJSON_GetArraySize(values_array);
    for (int i = 0; i < num_reqs; i++) {
        cJSON *req_array = cJSON_GetArrayItem(values_array, i);
        if (!cJSON_IsArray(req_array) || cJSON_GetArraySize(req_array) != 35) {
            log_message(LOG_ERROR, "Error: Each requirement should be an array of 35 strings, but the array size is: %d\n", cJSON_GetArraySize(req_array));
            cJSON_Delete(json);
            cJSON_Delete(input_json);
            return NULL;
        }

        // Create a JSON object for the current requirement
        cJSON *req = cJSON_CreateObject();
        if (!req) {
            log_message(LOG_ERROR, "Error creating JSON object for requirement");
            cJSON_Delete(json);
            cJSON_Delete(input_json);
            return NULL;
        }

        // Add the fields to the requirement JSON object
        cJSON_AddStringToObject(req, "ID", cJSON_GetArrayItem(req_array, REQ_ID_COL)->valuestring);
        cJSON_AddStringToObject(req, "Title", cJSON_GetArrayItem(req_array, REQ_TITLE_COL)->valuestring);
        cJSON_AddStringToObject(req, "Description", cJSON_GetArrayItem(req_array, REQ_DESCRIPTION_COL)->valuestring);
        cJSON_AddStringToObject(req, "Source", cJSON_GetArrayItem(req_array, REQ_SOURCE_COL)->valuestring);
        cJSON_AddStringToObject(req, "Author", cJSON_GetArrayItem(req_array, REQ_AUTHOR_COL)->valuestring);
        cJSON_AddStringToObject(req, "Justification", cJSON_GetArrayItem(req_array, REQ_JUSTIFICATION_COL)->valuestring);
        cJSON_AddStringToObject(req, "Criticality", cJSON_GetArrayItem(req_array, REQ_CRITICALITY_COL)->valuestring);
        cJSON_AddStringToObject(req, "Compliance", cJSON_GetArrayItem(req_array, REQ_COMPLIANCE_COL)->valuestring);
        cJSON_AddStringToObject(req, "Verification Status", cJSON_GetArrayItem(req_array, REQ_VERIFICATION_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Assignee", cJSON_GetArrayItem(req_array, REQ_ASSIGNEE_COL)->valuestring);
        
        cJSON_AddStringToObject(req, "Review 1 Verification 1 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_1_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 1 Verification 1 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_1_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 1 Verification 2 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_2_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 1 Verification 2 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_2_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 1 Verification 3 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_3_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 1 Verification 3 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_3_STATUS_COL)->valuestring);
        
        cJSON_AddStringToObject(req, "Review 2 Verification 1 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_1_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 2 Verification 1 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_1_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 2 Verification 2 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_2_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 2 Verification 2 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_2_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 2 Verification 3 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_3_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 2 Verification 3 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_3_STATUS_COL)->valuestring);
        
        cJSON_AddStringToObject(req, "Review 3 Verification 1 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_1_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 3 Verification 1 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_1_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 3 Verification 2 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_2_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 3 Verification 2 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_2_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 3 Verification 3 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_3_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 3 Verification 3 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_3_STATUS_COL)->valuestring);

        cJSON_AddStringToObject(req, "Review 4 Verification 1 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_1_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 4 Verification 1 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_1_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 4 Verification 2 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_2_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 4 Verification 2 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_2_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 4 Verification 3 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_3_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 4 Verification 3 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_3_STATUS_COL)->valuestring);

        cJSON_AddStringToObject(req, "Path", cJSON_GetArrayItem(req_array, REQ_PATH_COL)->valuestring);

        // Add the requirement object to the requirements array
        cJSON_AddItemToArray(requirements, req);
    }

    
    log_message(LOG_DEBUG, "Exiting function parseArrayIntoJSONRequirementList");

    return json;
}

char *buildDrlFromJSONRequirementList(cJSON *requirementList){
    log_message(LOG_DEBUG, "Entering function buildDrlFromJSONRequirementList");
    
    // Get the requirements array from the requirementList object
    cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");
    if (!cJSON_IsArray(requirements)) {
        log_message(LOG_ERROR, "Error: requirements is not a JSON array");
    }

    char *DRL = strdup(template_DRL);

    // Iterate over each requirement object in the requirements array
    int num_reqs = cJSON_GetArraySize(requirements);
    for (int i = 0; i < num_reqs; i++) {
        cJSON *requirement = cJSON_GetArrayItem(requirements, i);
        if (!cJSON_IsObject(requirement)) {
            log_message(LOG_ERROR, "Error: requirement is not a JSON object");
            continue;
        }

        // Get and print each item of the requirement object
        cJSON *id = cJSON_GetObjectItemCaseSensitive(requirement, "ID");
        cJSON *path = cJSON_GetObjectItemCaseSensitive(requirement, "Path");
        cJSON *title = cJSON_GetObjectItemCaseSensitive(requirement, "Title");
        cJSON *description = cJSON_GetObjectItemCaseSensitive(requirement, "Description");

        if (cJSON_IsString(id) && id->valuestring) {
            //printf("ID: %s\n", id->valuestring);
            DRL = appendStrings(DRL, "- [");
            DRL = appendStrings(DRL, id->valuestring);
            DRL = appendStrings(DRL, "](/");
        }
        if (cJSON_IsString(path) && path->valuestring) {
            //printf("Path: %s\n", path->valuestring);
            DRL = appendStrings(DRL, path->valuestring);
            DRL = appendStrings(DRL, ") **");
        }
        if (cJSON_IsString(title) && title->valuestring) {
            //printf("title: %s\n", title->valuestring);
            DRL = appendStrings(DRL, title->valuestring);
            DRL = appendStrings(DRL, "**\n");
        }
        if (cJSON_IsString(description) && description->valuestring) {
            //printf("Description: %s\n", description->valuestring);
            DRL = appendStrings(DRL, description->valuestring);
            DRL = appendStrings(DRL, "\n");
        }

    }

    DRL = appendStrings(DRL, "{.links-list}");
    
    log_message(LOG_DEBUG, "Exiting function buildDrlFromJSONRequirementList");

    return DRL;

}

pageList* buildRequirementPageFromJSONRequirementList(cJSON *requirementList, char *requirementId){
    log_message(LOG_DEBUG, "Entering function buildRequirementPageFromJSONRequirementList");
    
    // Get the requirements array from the requirementList object
    cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");
    if (!cJSON_IsArray(requirements)) {
        log_message(LOG_ERROR, "Error: requirements is not a JSON array");
    }

    pageList* reqPage = NULL;

    char *pageContent = strdup(template_REQ);

    // Iterate over each requirement object in the requirements array
    int num_reqs = cJSON_GetArraySize(requirements);
    for (int i = 0; i < num_reqs; i++) {
        cJSON *requirement = cJSON_GetArrayItem(requirements, i);
        if (!cJSON_IsObject(requirement)) {
            log_message(LOG_ERROR, "Error: requirement is not a JSON object");
            continue;
        }

        // Get and print each item of the requirement object
        cJSON *id = cJSON_GetObjectItemCaseSensitive(requirement, "ID");

        if (cJSON_IsString(id) && id->valuestring && strcmp(id->valuestring, requirementId) != 0){
            continue;
        }


        cJSON *title = cJSON_GetObjectItemCaseSensitive(requirement, "Title");
        cJSON *description = cJSON_GetObjectItemCaseSensitive(requirement, "Description");
        cJSON *source = cJSON_GetObjectItemCaseSensitive(requirement, "Source");
        cJSON *author = cJSON_GetObjectItemCaseSensitive(requirement, "Author");
        cJSON *justification = cJSON_GetObjectItemCaseSensitive(requirement, "Justification");
        cJSON *criticality = cJSON_GetObjectItemCaseSensitive(requirement, "Criticality");
        cJSON *compliance = cJSON_GetObjectItemCaseSensitive(requirement, "Compliance");
        cJSON *verification_status = cJSON_GetObjectItemCaseSensitive(requirement, "Verification Status");
        cJSON *assignee = cJSON_GetObjectItemCaseSensitive(requirement, "Assignee");

        cJSON *path = cJSON_GetObjectItemCaseSensitive(requirement, "Path");

        

        //TITLE
        if (cJSON_IsString(id) && id->valuestring) {
            //printf("ID: %s\n", id->valuestring);
            pageContent = appendStrings(pageContent, "# ");
            pageContent = appendStrings(pageContent, id->valuestring);
            pageContent = appendStrings(pageContent, ": ");
        }
        if (cJSON_IsString(title) && title->valuestring) {
            //printf("title: %s\n", title->valuestring);
            pageContent = appendStrings(pageContent, title->valuestring);
            pageContent = appendStrings(pageContent, "\n");
        }


        //DESCRIPTION
        if (cJSON_IsString(description) && description->valuestring) {
            //printf("Description: %s\n", description->valuestring);
            pageContent = appendStrings(pageContent, ">**Description**: ");
            pageContent = appendStrings(pageContent, description->valuestring);
            pageContent = appendStrings(pageContent, "\n");
        }


        //INFORMATION BOX: SOURCES AND ASSIGNEE
        if (cJSON_IsString(source) && source->valuestring && strcmp(source->valuestring, REQ_SHEET_EMPTY_VALUE) != 0) {
            //printf("Path: %s\n", path->valuestring);
            pageContent = appendStrings(pageContent, "\n>**Source**: ");
            pageContent = appendStrings(pageContent, source->valuestring);
            pageContent = appendStrings(pageContent, "\n");
        }
        if (cJSON_IsString(author) && author->valuestring && strcmp(author->valuestring, REQ_SHEET_EMPTY_VALUE) != 0) {
            //printf("Path: %s\n", path->valuestring);
            pageContent = appendStrings(pageContent, ">**Author**: ");
            pageContent = appendStrings(pageContent, author->valuestring);
            pageContent = appendStrings(pageContent, "\n");
        }
        if (cJSON_IsString(assignee) && assignee->valuestring && strcmp(assignee->valuestring, REQ_SHEET_EMPTY_VALUE) != 0) {
            //printf("Path: %s\n", path->valuestring);
            pageContent = appendStrings(pageContent, ">**Assignee**: ");
            pageContent = appendStrings(pageContent, assignee->valuestring);
            pageContent = appendStrings(pageContent, "\n");
        }
        if (strcmp(source->valuestring, REQ_SHEET_EMPTY_VALUE) != 0 || strcmp(author->valuestring, REQ_SHEET_EMPTY_VALUE) != 0 || strcmp(assignee->valuestring, REQ_SHEET_EMPTY_VALUE) != 0) {
            //printf("Path: %s\n", path->valuestring);
            pageContent = appendStrings(pageContent, "{.is-info}\n");
        }


        //JUSTIFICATION
        if (cJSON_IsString(justification) && justification->valuestring && strcmp(justification->valuestring, REQ_SHEET_EMPTY_VALUE) != 0) {
            //printf("Path: %s\n", path->valuestring);
            pageContent = appendStrings(pageContent, "\n## Justification\n");
            pageContent = appendStrings(pageContent, justification->valuestring);
            pageContent = appendStrings(pageContent, "\n");
        }


        //COMPLIANCE
        if (cJSON_IsString(compliance) && compliance->valuestring && strcmp(compliance->valuestring, REQ_SHEET_EMPTY_VALUE) != 0) {
            //printf("Path: %s\n", path->valuestring);
            
            if(strcmp(compliance->valuestring, "Compliant") == 0){
                pageContent = appendStrings(pageContent, "\n# Compliance\n");
                pageContent = appendStrings(pageContent, ":green_circle: Compliant\n");
            }
            if(strcmp(compliance->valuestring, "Unknown") == 0){
                pageContent = appendStrings(pageContent, "\n# Compliance\n");
                pageContent = appendStrings(pageContent, ":orange_circle: Unknown\n");
            }
            if(strcmp(compliance->valuestring, "Uncompliant") == 0){
                pageContent = appendStrings(pageContent, "\n# Compliance\n");
                pageContent = appendStrings(pageContent, ":red_circle: Uncompliant\n");
            }
        }


        //CRITICALITY
        if (cJSON_IsString(criticality) && criticality->valuestring && strcmp(criticality->valuestring, REQ_SHEET_EMPTY_VALUE) != 0) {
            //printf("Path: %s\n", path->valuestring);
            
            if(strcmp(criticality->valuestring, "Low") == 0){
                pageContent = appendStrings(pageContent, "\n# Criticality\n");
                pageContent = appendStrings(pageContent, ":green_circle: Low\n");
            }
            if(strcmp(criticality->valuestring, "Medium") == 0){
                pageContent = appendStrings(pageContent, "\n# Criticality\n");
                pageContent = appendStrings(pageContent, ":orange_circle: Medium\n");
            }
            if(strcmp(criticality->valuestring, "High") == 0){
                pageContent = appendStrings(pageContent, "\n# Criticality\n");
                pageContent = appendStrings(pageContent, ":red_circle: High\n");
            }
        }



        //VERIFICAITON
        int isVerification = 0;
        int verificationCount = 0;

        for(int reviewNumber = 1; reviewNumber <= MAXIMUM_NUMBER_OF_REVIEWS; reviewNumber++){
            for (int verificationNumber = 1; verificationNumber <= MAXIMUM_NUMBER_OF_VERIFICATIONS_PER_REVIEW; verificationNumber++){

                char JsonItemNameMethod[1024];
                snprintf(JsonItemNameMethod, sizeof(JsonItemNameMethod), "Review %d Verification %d Method", reviewNumber, verificationNumber);

                char JsonItemNameStatus[1024];
                snprintf(JsonItemNameStatus, sizeof(JsonItemNameStatus), "Review %d Verification %d Status", reviewNumber, verificationNumber);

                cJSON *verificationMethod = cJSON_GetObjectItemCaseSensitive(requirement, JsonItemNameMethod);
                cJSON *verificationStatus = cJSON_GetObjectItemCaseSensitive(requirement, JsonItemNameStatus);

                char *reviewName = REVIEW_1_NAME;

                if(reviewNumber = 1){reviewName = REVIEW_1_NAME;}
                if(reviewNumber = 2){reviewName = REVIEW_2_NAME;}
                if(reviewNumber = 3){reviewName = REVIEW_3_NAME;}
                if(reviewNumber = 4){reviewName = REVIEW_4_NAME;}

                if(strcmp(verificationMethod->valuestring, REQ_SHEET_EMPTY_VALUE) != 0){
                    
                    verificationCount++;

                    if(isVerification == 0){
                        pageContent = appendStrings(pageContent, "\n# Verification");
                        isVerification = 1;
                    }

                    char temp_verificationNumber[100];
                    snprintf(temp_verificationNumber, sizeof(temp_verificationNumber), "\n## Verification %d\n", verificationCount);
                    pageContent = appendStrings(pageContent, temp_verificationNumber);

                    pageContent = appendStrings(pageContent, "**Method**: ");
                    pageContent = appendStrings(pageContent, verificationMethod->valuestring);
                    pageContent = appendStrings(pageContent, "\n**Deadline**: ");
                    pageContent = appendStrings(pageContent, reviewName);
                    pageContent = appendStrings(pageContent, "\n");

                    if(strcmp(verificationStatus->valuestring, REQ_SHEET_EMPTY_VALUE) != 0){
                        pageContent = appendStrings(pageContent, "**Status**: ");

                        if(strcmp(verificationStatus->valuestring, "Completed") == 0){pageContent = appendStrings(pageContent, ":green_circle:");}
                        if(strcmp(verificationStatus->valuestring, "In progress") == 0){pageContent = appendStrings(pageContent, ":orange_circle:");}
                        if(strcmp(verificationStatus->valuestring, "Uncompleted") == 0){pageContent = appendStrings(pageContent, ":red_circle:");}

                        pageContent = appendStrings(pageContent, verificationStatus->valuestring);
                        pageContent = appendStrings(pageContent, "\n");

                    }


                }

            }
        }

        
        reqPage = addPageToList(&reqPage, TEST_REQ_PAGE_ID, id->valuestring, "", "", pageContent, "", "");

        fprintf(stderr, "\n\n\n%s\n\n\n", pageContent);

        break;
    }

    
    log_message(LOG_DEBUG, "Exiting function buildRequirementPageFromJSONRequirementList");

    return reqPage;

}

void appendMentionedIn(pageList** head){
    log_message(LOG_DEBUG, "Entering function appendMentionedIn");
    

    pageList* subjectPage = *head;

    pageList* linkTrackerPage = NULL;
    linkTrackerPage = addPageToList(&linkTrackerPage, LINK_TRACKER_PAGE_ID, "", "", "", "", "", "");
    fprintf(stderr,"linkTrackerPage id set\n");
    linkTrackerPage = getPage(&linkTrackerPage);

    pageList* IncomingLinks = findIncomingLinks(&IncomingLinks, linkTrackerPage->content, subjectPage->path);

    subjectPage->content = appendStrings(subjectPage->content, "\n# Mentiond in\n");
    while (IncomingLinks != NULL) {
        subjectPage->content = appendStrings(subjectPage->content, "- [");
        subjectPage->content = appendStrings(subjectPage->content, IncomingLinks->title);
        subjectPage->content = appendStrings(subjectPage->content, "](");
        subjectPage->content = appendStrings(subjectPage->content, IncomingLinks->path);
        subjectPage->content = appendStrings(subjectPage->content, ")\n");
        IncomingLinks = IncomingLinks->next;
    }

    subjectPage->content = appendStrings(subjectPage->content, "{.links-list}\n");

    subjectPage->content = replaceWord(subjectPage->content, "\n", "\\\\n");
    subjectPage->content = replaceWord(subjectPage->content, "\"", "\\\"");
    updatePageContentMutation(subjectPage);
    renderMutation(&subjectPage);

    
    log_message(LOG_DEBUG, "Exiting function appendMentionedIn");
    
    return;
}

char *createVcdPieChart(char *unverifiedPopulation, char *partiallyVerifiedPopulation, char *verifiedPopulation){
    log_message(LOG_DEBUG, "Entering function createVcdPieChart");
    

    char *pieChart = "```kroki\nvega\n\n{\n  \"$schema\": \"https://vega.github.io/schema/vega/v5.0.json\",\n  \"width\": 350,\n  \"height\": 350,\n  \"autosize\": \"pad\",\n  \"signals\": [\n    {\"name\": \"startAngle\", \"value\": 0},\n    {\"name\": \"endAngle\", \"value\": 6.29},\n    {\"name\": \"padAngle\", \"value\": 0},\n    {\"name\": \"sort\", \"value\": true},\n    {\"name\": \"strokeWidth\", \"value\": 2},\n    {\n      \"name\": \"selected\",\n      \"value\": \"\",\n      \"on\": [{\"events\": \"mouseover\", \"update\": \"datum\"}]\n    }\n  ],\n  \"data\": [\n    {\n      \"name\": \"table\",\n      \"values\": [\n        {\"continent\": \"Unverified\", \"population\": DefaultUnverifiedPopulation},\n        {\"continent\": \"Partially Verified\", \"population\": DefaultPartiallyVerifiedPopulation},\n        {\"continent\": \"Verified\", \"population\": DefaultVerifiedPopulation}\n      ],\n      \"transform\": [\n        {\n          \"type\": \"pie\",\n          \"field\": \"population\",\n          \"startAngle\": {\"signal\": \"startAngle\"},\n          \"endAngle\": {\"signal\": \"endAngle\"},\n          \"sort\": {\"signal\": \"sort\"}\n        }\n      ]\n    },\n    {\n      \"name\": \"fieldSum\",\n      \"source\": \"table\",\n      \"transform\": [\n        {\n          \"type\": \"aggregate\",\n          \"fields\": [\"population\"],\n          \"ops\": [\"sum\"],\n          \"as\": [\"sum\"]\n        }\n      ]\n    }\n  ],\n  \"legends\": [\n    {\n      \"fill\": \"color\",\n      \"title\": \"Legends\",\n      \"orient\": \"none\",\n      \"padding\": {\"value\": 10},\n      \"encode\": {\n        \"symbols\": {\"enter\": {\"fillOpacity\": {\"value\": 1}}},\n        \"legend\": {\n          \"update\": {\n            \"x\": {\n              \"signal\": \"(width / 2) + if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.1 * 0.8, if(width >= height, height, width) / 2 * 0.8)\",\n              \"offset\": 20\n            },\n            \"y\": {\"signal\": \"(height / 2)\", \"offset\": -50}\n          }\n        }\n      }\n    }\n  ],\n  \"scales\": [\n    {\"name\": \"color\", \"type\": \"ordinal\", \"range\": [\"#cf2608\", \"#ff9900\", \"#67b505\"]}\n  ],\n  \"marks\": [\n    {\n      \"type\": \"arc\",\n      \"from\": {\"data\": \"table\"},\n      \"encode\": {\n        \"enter\": {\n          \"fill\": {\"scale\": \"color\", \"field\": \"continent\"},\n          \"x\": {\"signal\": \"width / 2\"},\n          \"y\": {\"signal\": \"height / 2\"}\n        },\n        \"update\": {\n          \"startAngle\": {\"field\": \"startAngle\"},\n          \"endAngle\": {\"field\": \"endAngle\"},\n          \"cornerRadius\": {\"value\": 15},\n          \"padAngle\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, 0.015, 0.015)\"\n          },\n          \"innerRadius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 0.45, if(width >= height, height, width) / 2 * 0.5)\"\n          },\n          \"outerRadius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.05 * 0.8, if(width >= height, height, width) / 2 * 0.8)\"\n          },\n          \"opacity\": {\n            \"signal\": \"if(selected && selected.continent !== datum.continent, 1, 1)\"\n          },\n          \"stroke\": {\"signal\": \"scale('color', datum.continent)\"},\n          \"strokeWidth\": {\"signal\": \"strokeWidth\"},\n          \"fillOpacity\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, 0.8, 0.8)\"\n          }\n        }\n      }\n    },\n    {\n      \"type\": \"text\",\n      \"encode\": {\n        \"enter\": {\"fill\": {\"value\": \"#525252\"}, \"text\": {\"value\": \"\"}},\n        \"update\": {\n          \"opacity\": {\"value\": 1},\n          \"x\": {\"signal\": \"width / 2\"},\n          \"y\": {\"signal\": \"height / 2\"},\n          \"align\": {\"value\": \"center\"},\n          \"baseline\": {\"value\": \"middle\"},\n          \"fontSize\": {\"signal\": \"if(width >= height, height, width) * 0.05\"},\n          \"text\": {\"value\": \"Verification Status\"}\n        }\n      }\n    },\n    {\n      \"name\": \"mark_population\",\n      \"type\": \"text\",\n      \"from\": {\"data\": \"table\"},\n      \"encode\": {\n        \"enter\": {\n          \"text\": {\n            \"signal\": \"if(datum['endAngle'] - datum['startAngle'] < 0.3, '', format(datum['population'] / 1, '.0f'))\"\n          },\n          \"x\": {\"signal\": \"if(width >= height, height, width) / 2\"},\n          \"y\": {\"signal\": \"if(width >= height, height, width) / 2\"},\n          \"radius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.05 * 0.65, if(width >= height, height, width) / 2 * 0.65)\"\n          },\n          \"theta\": {\"signal\": \"(datum['startAngle'] + datum['endAngle'])/2\"},\n          \"fill\": {\"value\": \"#FFFFFF\"},\n          \"fontSize\": {\"value\": 12},\n          \"align\": {\"value\": \"center\"},\n          \"baseline\": {\"value\": \"middle\"}\n        }\n      }\n    }\n  ]\n}\n\n```";

    pieChart = replaceWord(pieChart, "DefaultUnverifiedPopulation", unverifiedPopulation);
    pieChart = replaceWord(pieChart, "DefaultPartiallyVerifiedPopulation", partiallyVerifiedPopulation);
    pieChart = replaceWord(pieChart, "DefaultVerifiedPopulation", verifiedPopulation);

    
    log_message(LOG_DEBUG, "Exiting function createVcdPieChart");

    return pieChart;

}

char *updateVcdStackedAreaChart(char *json_str, char *week, int verifiedValue, int partiallyVerifiedValue, int unverifiedValue) {
    log_message(LOG_DEBUG, "Entering function updateVcdStackedAreaChart");
    

    fprintf(stderr, "JSON string: %s\n", json_str);
    
    // Parse the input JSON string
    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL) {
        return NULL;
    }

    // Find the "values" array
    cJSON *data = cJSON_GetObjectItem(root, "data");
    if (data == NULL) {
        cJSON_Delete(root);
        return NULL;
    }

    cJSON *values = cJSON_GetObjectItem(data, "values");
    if (values == NULL || !cJSON_IsArray(values)) {
        cJSON_Delete(root);
        return NULL;
    }


    // Create new JSON objects for the three new items
    cJSON *item1 = cJSON_CreateObject();
    cJSON_AddStringToObject(item1, "week", week);
    cJSON_AddStringToObject(item1, "status", "Verified");
    cJSON_AddNumberToObject(item1, "value", verifiedValue);

    cJSON *item2 = cJSON_CreateObject();
    cJSON_AddStringToObject(item2, "week", week);
    cJSON_AddStringToObject(item2, "status", "Partially Verified");
    cJSON_AddNumberToObject(item2, "value", partiallyVerifiedValue);

    cJSON *item3 = cJSON_CreateObject();
    cJSON_AddStringToObject(item3, "week", week);
    cJSON_AddStringToObject(item3, "status", "Unverified");
    cJSON_AddNumberToObject(item3, "value", unverifiedValue);

    // Add the new items to the "values" array
    cJSON_AddItemToArray(values, item1);
    cJSON_AddItemToArray(values, item2);
    cJSON_AddItemToArray(values, item3);

    fprintf(stderr, "about to call cJSON_Print\n");

    // Convert the updated JSON structure back to a string
    char *updated_json_str = cJSON_Print(root);

    // Clean up
    cJSON_Delete(root);

    
    log_message(LOG_DEBUG, "Exiting function updateVcdStackedAreaChart");
    return updated_json_str;
}

/*
char *extractVerificationStatsForAReview(cJSON *requirementList){

    //VERIFICAITON
    int isVerification = 0;
    int verificationCount = 0;

    for(int reviewNumber = 1; reviewNumber <= MAXIMUM_NUMBER_OF_REVIEWS; reviewNumber++){
        for (int verificationNumber = 1; verificationNumber <= MAXIMUM_NUMBER_OF_VERIFICATIONS_PER_REVIEW; verificationNumber++){

            char JsonItemNameMethod[1024];
            snprintf(JsonItemNameMethod, sizeof(JsonItemNameMethod), "Review %d Verification %d Method", reviewNumber, verificationNumber);

            char JsonItemNameStatus[1024];
            snprintf(JsonItemNameStatus, sizeof(JsonItemNameStatus), "Review %d Verification %d Status", reviewNumber, verificationNumber);

            cJSON *verificationMethod = cJSON_GetObjectItemCaseSensitive(requirement, JsonItemNameMethod);
            cJSON *verificationStatus = cJSON_GetObjectItemCaseSensitive(requirement, JsonItemNameStatus);

            char *reviewName = REVIEW_1_NAME;

            if(reviewNumber = 1){reviewName = REVIEW_1_NAME;}
            if(reviewNumber = 2){reviewName = REVIEW_2_NAME;}
            if(reviewNumber = 3){reviewName = REVIEW_3_NAME;}
            if(reviewNumber = 4){reviewName = REVIEW_4_NAME;}

            if(strcmp(verificationMethod->valuestring, REQ_SHEET_EMPTY_VALUE) != 0){
                
                verificationCount++;

                if(isVerification == 0){
                    pageContent = appendStrings(pageContent, "\n# Verification");
                    isVerification = 1;
                }

                char temp_verificationNumber[100];
                snprintf(temp_verificationNumber, sizeof(temp_verificationNumber), "\n## Verification %d\n", verificationCount);
                pageContent = appendStrings(pageContent, temp_verificationNumber);

                pageContent = appendStrings(pageContent, "**Method**: ");
                pageContent = appendStrings(pageContent, verificationMethod->valuestring);
                pageContent = appendStrings(pageContent, "\n**Deadline**: ");
                pageContent = appendStrings(pageContent, reviewName);
                pageContent = appendStrings(pageContent, "\n");

                if(strcmp(verificationStatus->valuestring, REQ_SHEET_EMPTY_VALUE) != 0){
                    pageContent = appendStrings(pageContent, "**Status**: ");

                    if(strcmp(verificationStatus->valuestring, "Completed") == 0){pageContent = appendStrings(pageContent, ":green_circle:");}
                    if(strcmp(verificationStatus->valuestring, "In progress") == 0){pageContent = appendStrings(pageContent, ":orange_circle:");}
                    if(strcmp(verificationStatus->valuestring, "Uncompleted") == 0){pageContent = appendStrings(pageContent, ":red_circle:");}

                    pageContent = appendStrings(pageContent, verificationStatus->valuestring);
                    pageContent = appendStrings(pageContent, "\n");

                }


            }

        }
    }

}
*/

void freeWikiFlagList(wikiFlag** head) {
    log_message(LOG_DEBUG, "Entering function freeWikiFlagList");
    
    while (*head) {
        wikiFlag* temp = *head;
        *head = (*head)->next;
        
        // Debugging prints
        if (temp->pointerToBeginningOfSecondMarker){
            free(temp->pointerToBeginningOfSecondMarker);
        }
        if (temp->pointerToBeginningOfSecondMarker){
            free(temp->pointerToBeginningOfSecondMarker);
        }
        
        fprintf(stderr, "about to free temp\n");
        free(temp);
        fprintf(stderr, "freed temp\n");
    }

    
    log_message(LOG_DEBUG, "Exiting function freeWikiFlagList");
}