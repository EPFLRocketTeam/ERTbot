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
#include "../include/requirements.h"


#define MAX_ARGUMENTS 10
#define MAX_ARGUMENT_LENGTH 100


pageList* addPageToList(pageList** head,  char *id, char *title, char *path, char *description, char *content, char *updatedAt, char *createdAt, char *authorId) {
    log_message(LOG_DEBUG, "Entering function addPageToList");
    
    pageList* newNode = (pageList *)malloc(sizeof(pageList));
    if (!newNode) {
        log_message(LOG_ERROR, "Memory allocation error");
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

    newNode->authorId = malloc(strlen(authorId) + 1);
    strcpy(newNode->authorId, authorId);

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
                log_message(LOG_DEBUG, "Created directory: %s\n", curr_path);
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
    
    // Set the timezone to Eastern Time
    setenv("TZ", "GMT", 1);
    tzset();

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
        renderMutation(&current, false);
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

char* createList(char *list, pageList** sectionTitle, pageList* links){
    log_message(LOG_DEBUG, "Entering function createList");
    
    char *tempList = list;
    tempList = appendToString(tempList, "\\\\n\\\\n");
    tempList = appendToString(tempList, "## [");
    tempList = appendToString(tempList, (*sectionTitle)->title);
    tempList = appendToString(tempList, "](/");
    tempList = appendToString(tempList, (*sectionTitle)->path);
    tempList = appendToString(tempList, ")");
    //tempList = appendToString(tempList, "\\\\n");
    while (links != NULL) {
        tempList = appendToString(tempList, "\\\\n");
        tempList = appendToString(tempList, links->path);
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
    log_message(LOG_DEBUG, "path:%s\n", sectionTitle->path);

    tempList = appendToString(tempList, ")\\n" );

    
    link = links;
    while (link != NULL) {

        tempList = appendToString(tempList, link->path);
        tempList = appendToString(tempList, "\\n");
        link = link->next;
    }

    char *temp = createCombinedString(sectionTitle->path, ")");

    // Find and replace old list
    char *startPtr = strstr(list, temp);
    //startPtr += 1;
    if (startPtr == NULL) {
        log_message(LOG_DEBUG, "Section title: %s not found\n", sectionTitle->title);
        free(tempList);
        return list;
    }

    char *endPtr = strstr(startPtr, "\\n\\n");
    endPtr +=1;
    if (endPtr == NULL) {
        log_message(LOG_ERROR, "End of section not found");
        free(tempList);
        return list;
    }

    log_message(LOG_DEBUG, "About to update %s to:\n %s\n", sectionTitle->title, tempList);
    list = replaceParagraph(list, tempList, startPtr, endPtr);

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
    localGraph = appendToString(localGraph, "\n+");
    localGraph = appendToString(localGraph, " [[https://rocket-team.epfl.ch/en/");
    localGraph = appendToString(localGraph, (*tempPage)->path);
    localGraph = appendToString(localGraph, " ");
    localGraph = appendToString(localGraph, (*tempPage)->title);
    localGraph = appendToString(localGraph, "]]\n");

    while (currentIncomingLink != NULL) {
        localGraph = appendToString(localGraph, "\n--");
        localGraph = appendToString(localGraph, " [[https://rocket-team.epfl.ch/en/");
        localGraph = appendToString(localGraph, currentIncomingLink->path);
        localGraph = appendToString(localGraph, " ");
        localGraph = appendToString(localGraph, currentIncomingLink->title);
        localGraph = appendToString(localGraph, "]]\n");
        currentIncomingLink = currentIncomingLink->next;

    }

    currentOutgoingLink = currentOutgoingLink->next;

    while(currentOutgoingLink != NULL){
        localGraph = appendToString(localGraph, "\n++");
        localGraph = appendToString(localGraph, " [[https://rocket-team.epfl.ch/en/");
        localGraph = appendToString(localGraph, currentOutgoingLink->path);
        localGraph = appendToString(localGraph, " ");
        localGraph = appendToString(localGraph, getDocId(currentOutgoingLink->path));
        localGraph = appendToString(localGraph, "]]\n");
        currentOutgoingLink = currentOutgoingLink->next;
    }

    localGraph = appendToString(localGraph, "\n\n\n@endmindmap\n```\n\n\n");

    freePageList(&currentIncomingLink);
    freePageList(&currentOutgoingLink);
    
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
            log_message(LOG_DEBUG, "Freeing page struct variable id: %p", (void*)temp->id);
            free(temp->id);
        }
        if (temp->title){
            log_message(LOG_DEBUG, "Freeing page struct variable title: %p", (void*)temp->title);
            free(temp->title);
        }
        if (temp->path){
            log_message(LOG_DEBUG, "Freeing page struct variable path: %p", (void*)temp->path);
            free(temp->path);
        }
        if (temp->description){
            log_message(LOG_DEBUG, "Freeing page struct variable description: %p", (void*)temp->description);
            free(temp->description);
        }
        if (temp->content){
            log_message(LOG_DEBUG, "Freeing page struct variable content: %p", (void*)temp->content);
            free(temp->content);
        }
        if (temp->updatedAt){
            log_message(LOG_DEBUG, "Freeing page struct variable updatedAt: %p", (void*)temp->updatedAt);
            free(temp->updatedAt);
        }
        if (temp->createdAt){ 
            log_message(LOG_DEBUG, "Freeing page struct variable createdAt: %p", (void*)temp->createdAt);
            free(temp->createdAt);
        }
        if (temp->authorId){ 
            log_message(LOG_DEBUG, "Freeing page struct variable authorId: %p", (void*)temp->authorId);
            free(temp->authorId);
        }
        
        log_message(LOG_DEBUG, "about to free page struct");
        free(temp);
        log_message(LOG_DEBUG, "freed page struct");
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
            log_message(LOG_ERROR, "Memory allocation error");
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
            *links = addPageToList(links, "", title, link, "", "", "", "", "");
            log_message(LOG_DEBUG, "found link %s %s", link, title);
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
            log_message(LOG_ERROR, "Error: Missing startFlag2 after startFlag1");
            freePageList(&imageLinks);
            return imageLinks;
        }

        linkStart += strlen(startFlag2);
        char *linkEnd = strstr(linkStart, endFlag);
        if (!linkEnd) {
            log_message(LOG_ERROR, "Error: Missing endFlag after linkStart");
            freePageList(&imageLinks);
            return imageLinks;
        }

        // Allocate memory for the link substring
        int linkLen = linkEnd - linkStart;
        char *link = (char *)malloc((linkLen + 1) * sizeof(char));
        if (!link) {
            log_message(LOG_ERROR, "Memory allocation error");
            freePageList(&imageLinks);
            return imageLinks;
        }

        strncpy(link, linkStart, linkLen);
        link[linkLen] = '\0';
        imageLinks = addPageToList(&imageLinks, link, "", "", "", "", "", "", "");

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
        log_message(LOG_ERROR, "Memory allocation failed");
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
                log_message(LOG_DEBUG, "Found a reference in title: %s, path:%s\n", titleStart, pathStart);
                incomingLinks = addPageToList(&incomingLinks, "", titleStart, pathStart, "", "", "", "", "");
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
        log_message(LOG_ERROR, "Memory allocation failed");
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
                outgoingLinks = addPageToList(&outgoingLinks, "", "", link, "", "", "", "", "");
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

void appendMentionedIn(pageList** head){
    log_message(LOG_DEBUG, "Entering function appendMentionedIn");
    

    pageList* subjectPage = *head;

    pageList* linkTrackerPage = NULL;
    linkTrackerPage = addPageToList(&linkTrackerPage, LINK_TRACKER_PAGE_ID, "", "", "", "", "", "", "");
    log_message(LOG_DEBUG, "linkTrackerPage id set");
    linkTrackerPage = getPage(&linkTrackerPage);

    pageList* IncomingLinks = findIncomingLinks(&IncomingLinks, linkTrackerPage->content, subjectPage->path);

    subjectPage->content = appendToString(subjectPage->content, "\n# Mentiond in\n");
    while (IncomingLinks != NULL) {
        subjectPage->content = appendToString(subjectPage->content, "- [");
        subjectPage->content = appendToString(subjectPage->content, IncomingLinks->title);
        subjectPage->content = appendToString(subjectPage->content, "](");
        subjectPage->content = appendToString(subjectPage->content, IncomingLinks->path);
        subjectPage->content = appendToString(subjectPage->content, ")\n");
        IncomingLinks = IncomingLinks->next;
    }

    subjectPage->content = appendToString(subjectPage->content, "{.links-list}\n");

    subjectPage->content = replaceWord(subjectPage->content, "\n", "\\\\n");
    subjectPage->content = replaceWord(subjectPage->content, "\"", "\\\"");
    updatePageContentMutation(subjectPage);
    renderMutation(&subjectPage, false);
    
    log_message(LOG_DEBUG, "Exiting function appendMentionedIn");
    
    return;
}

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
        
        log_message(LOG_DEBUG, "about to free temp");
        free(temp);
        log_message(LOG_DEBUG, "freed temp");
    }

    
    log_message(LOG_DEBUG, "Exiting function freeWikiFlagList");
}

char* convert_timestamp_to_cest(char *timestamp) {
    struct tm tm;
    time_t raw_time;
    char *output_buffer = malloc(100); // Allocate memory for the output string

    if (output_buffer == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    // Initialize the struct tm to avoid any potential issues
    memset(&tm, 0, sizeof(struct tm));

    // Parse the timestamp into a struct tm (UTC time)
    if (strptime(timestamp, "%Y-%m-%dT%H:%M:%S", &tm) == NULL) {
        fprintf(stderr, "Failed to parse timestamp.\n");
        free(output_buffer);
        return NULL;
    }

    // Convert the struct tm to time_t (in UTC)
    raw_time = timegm(&tm);

    // Add 2 hours for CEST (Central European Summer Time)
    raw_time += 2 * 3600;

    // Convert back to struct tm in local time (CEST)
    struct tm *cest_time = localtime(&raw_time);

    // Format the CEST time into a readable string
    if (strftime(output_buffer, 100, "%A, %B %d at %H:%M:%S", cest_time) == 0) {
        fprintf(stderr, "Failed to format time.\n");
        free(output_buffer);
        return NULL;
    }

    return output_buffer; // Return the formatted string
}

char* buildPageDistributionPieChart(pageList* head){
    log_message(LOG_DEBUG, "Entering function buildPageDistributionPieChart");
    
    pageList* current = head;

    int competitionCount = 0;
    int icarusCount = 0;
    int hyperionCount = 0;
    int managementCount = 0;
    int spaceraceCount = 0;
    int otherCount = 0;

    while(current){

        if(strstr(current->path, "competition/")){
            competitionCount++;
        }
        else if(strstr(current->path, "icarus/")){
            icarusCount++;
        }
        else if(strstr(current->path, "hyperion/")){
            hyperionCount++;
        }
        else if(strstr(current->path, "management/")){
            managementCount++;
        }
        else if(strstr(current->path, "space_race/")){
            spaceraceCount++;
        }
        else{
            otherCount++;
        }
        current = current->next;
    }

    char *pieChart = "```kroki\nvega\n\n{\n  \"$schema\": \"https://vega.github.io/schema/vega/v5.0.json\",\n  \"width\": 350,\n  \"height\": 350,\n  \"autosize\": \"pad\",\n  \"signals\": [\n    {\"name\": \"startAngle\", \"value\": 0},\n    {\"name\": \"endAngle\", \"value\": 6.29},\n    {\"name\": \"padAngle\", \"value\": 0},\n    {\"name\": \"sort\", \"value\": true},\n    {\"name\": \"strokeWidth\", \"value\": 2},\n    {\n      \"name\": \"selected\",\n      \"value\": \"\",\n      \"on\": [{\"events\": \"mouseover\", \"update\": \"datum\"}]\n    }\n  ],\n  \"data\": [\n    {\n      \"name\": \"table\",\n      \"values\": [\n        {\"continent\": \"Competition\", \"population\": $competitionCount$},\n        {\"continent\": \"Hyperion\", \"population\": $hyperionCount$},\n        {\"continent\": \"Icarus\", \"population\": $icarusCount$},\n        {\"continent\": \"Space Race\", \"population\": $spaceraceCount$},\n        {\"continent\": \"Management\", \"population\": $managementCount$},\n        {\"continent\": \"Other\", \"population\": $otherCount$}\n      ],\n      \"transform\": [\n        {\n          \"type\": \"pie\",\n          \"field\": \"population\",\n          \"startAngle\": {\"signal\": \"startAngle\"},\n          \"endAngle\": {\"signal\": \"endAngle\"},\n          \"sort\": {\"signal\": \"sort\"}\n        }\n      ]\n    },\n    {\n      \"name\": \"fieldSum\",\n      \"source\": \"table\",\n      \"transform\": [\n        {\n          \"type\": \"aggregate\",\n          \"fields\": [\"population\"],\n          \"ops\": [\"sum\"],\n          \"as\": [\"sum\"]\n        }\n      ]\n    }\n  ],\n  \"legends\": [\n    {\n      \"fill\": \"color\",\n      \"title\": \"Legends\",\n      \"orient\": \"none\",\n      \"padding\": {\"value\": 10},\n      \"encode\": {\n        \"symbols\": {\"enter\": {\"fillOpacity\": {\"value\": 1}}},\n        \"legend\": {\n          \"update\": {\n            \"x\": {\n              \"signal\": \"(width / 2) + if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.1 * 0.8, if(width >= height, height, width) / 2 * 0.8)\",\n              \"offset\": 20\n            },\n            \"y\": {\"signal\": \"(height / 2)\", \"offset\": -50}\n          }\n        }\n      }\n    }\n  ],\n  \"scales\": [\n    {\"name\": \"color\", \"type\": \"ordinal\", \"range\": [\"#03071e\", \"#370617\", \"#9d0208\", \"#dc2f02\", \"#f48c06\", \"#ffba08\"]}\n  ],\n  \"marks\": [\n    {\n      \"type\": \"arc\",\n      \"from\": {\"data\": \"table\"},\n      \"encode\": {\n        \"enter\": {\n          \"fill\": {\"scale\": \"color\", \"field\": \"continent\"},\n          \"x\": {\"signal\": \"width / 2\"},\n          \"y\": {\"signal\": \"height / 2\"}\n        },\n        \"update\": {\n          \"startAngle\": {\"field\": \"startAngle\"},\n          \"endAngle\": {\"field\": \"endAngle\"},\n          \"cornerRadius\": {\"value\": 15},\n          \"padAngle\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, 0.015, 0.015)\"\n          },\n          \"innerRadius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 0.45, if(width >= height, height, width) / 2 * 0.5)\"\n          },\n          \"outerRadius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.05 * 0.8, if(width >= height, height, width) / 2 * 0.8)\"\n          },\n          \"opacity\": {\n            \"signal\": \"if(selected && selected.continent !== datum.continent, 1, 1)\"\n          },\n          \"stroke\": {\"signal\": \"scale('color', datum.continent)\"},\n          \"strokeWidth\": {\"signal\": \"strokeWidth\"},\n          \"fillOpacity\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, 0.8, 0.8)\"\n          }\n        }\n      }\n    },\n    {\n      \"type\": \"text\",\n      \"encode\": {\n        \"enter\": {\"fill\": {\"value\": \"#525252\"}, \"text\": {\"value\": \"\"}},\n        \"update\": {\n          \"opacity\": {\"value\": 1},\n          \"x\": {\"signal\": \"width / 2\"},\n          \"y\": {\"signal\": \"height / 2\"},\n          \"align\": {\"value\": \"center\"},\n          \"baseline\": {\"value\": \"middle\"},\n          \"fontSize\": {\"signal\": \"if(width >= height, height, width) * 0.05\"},\n          \"text\": {\"value\": \"Page Distribution\"}\n        }\n      }\n    },\n    {\n      \"name\": \"mark_population\",\n      \"type\": \"text\",\n      \"from\": {\"data\": \"table\"},\n      \"encode\": {\n        \"enter\": {\n          \"text\": {\n            \"signal\": \"if(datum['endAngle'] - datum['startAngle'] < 0.3, '', format(datum['population'] / 1, '.0f'))\"\n          },\n          \"x\": {\"signal\": \"if(width >= height, height, width) / 2\"},\n          \"y\": {\"signal\": \"if(width >= height, height, width) / 2\"},\n          \"radius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.05 * 0.65, if(width >= height, height, width) / 2 * 0.65)\"\n          },\n          \"theta\": {\"signal\": \"(datum['startAngle'] + datum['endAngle'])/2\"},\n          \"fill\": {\"value\": \"#FFFFFF\"},\n          \"fontSize\": {\"value\": 12},\n          \"align\": {\"value\": \"center\"},\n          \"baseline\": {\"value\": \"middle\"}\n        }\n      }\n    }\n  ]\n}\n```";

    char strcompetitionCount[10];
    sprintf(strcompetitionCount, "%d", competitionCount);

    char stricarusCount[10];
    sprintf(stricarusCount, "%d", icarusCount);

    char strhyperionCount[10];
    sprintf(strhyperionCount, "%d", hyperionCount);

    char strmanagementCount[10];
    sprintf(strmanagementCount, "%d", managementCount);

    char strspaceraceCount[10];
    sprintf(strspaceraceCount, "%d", spaceraceCount);

    char strotherCount[10];
    sprintf(strotherCount, "%d", otherCount);

    pieChart = replaceWord(pieChart, "$competitionCount$", strcompetitionCount);
    pieChart = replaceWord(pieChart, "$icarusCount$", stricarusCount);
    pieChart = replaceWord(pieChart, "$hyperionCount$", strhyperionCount);
    pieChart = replaceWord(pieChart, "$managementCount$", strmanagementCount);
    pieChart = replaceWord(pieChart, "$spaceraceCount$", strspaceraceCount);
    pieChart = replaceWord(pieChart, "$otherCount$", strotherCount);

    log_message(LOG_DEBUG, "Exiting function buildPageDistributionPieChart");
    return pieChart;
}




