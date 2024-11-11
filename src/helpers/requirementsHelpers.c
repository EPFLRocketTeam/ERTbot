#include <string.h>
#include <cjson/cJSON.h>
#include "ERTbot_config.h"
#include "ERTbot_common.h"
#include "sheetAPI.h"
#include "apiHelpers.h"
#include "stringHelpers.h"

static cJSON* parseSheet(const cJSON* values_array);

cJSON* parseArrayIntoJSONRequirementList(const char *input_str) {
    log_message(LOG_DEBUG, "Entering function parseArrayIntoJSONRequirementList");

    // Parse the input string as JSON
    cJSON *input_json = cJSON_Parse(input_str);
    if (!input_json) {
        log_message(LOG_ERROR, "Error parsing input string as JSON object");
        return NULL;
    }

    // Extract the "values" array from the JSON object
    const cJSON *values_array = cJSON_GetObjectItemCaseSensitive(input_json, "values");
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
    cJSON *requirements = parseSheet(values_array);
    if (!requirements) {
        log_message(LOG_ERROR, "Error creating JSON array");
        cJSON_Delete(json);
        cJSON_Delete(input_json);
        cJSON_Delete(requirements);
        return NULL;
    }

    cJSON_AddItemToObject(json, "requirements", requirements);

    log_message(LOG_DEBUG, "Exiting function parseArrayIntoJSONRequirementList");

    freeChunkResponse();

    cJSON_Delete(input_json);

    return json;
}

cJSON* getSubsystemInfo(const char* acronym){
    log_message(LOG_DEBUG, "Entering function getSubsystemInfo");

    batchGetSheet("1iB1yl2Nre95kD1g6TFDYvvLe0g5QzghtAHdnxNTD4tg", "INFO!A2:H30");

    cJSON *input_json = cJSON_Parse(chunk.response);
    if (!input_json) {
        log_message(LOG_ERROR, "Error parsing input string as JSON object");
        return NULL;
    }

    // Extract the "values" array from the JSON object
    const cJSON *values_array = cJSON_GetObjectItemCaseSensitive(input_json, "values");
    if (!cJSON_IsArray(values_array)) {
        log_message(LOG_ERROR, "Error: values is not a JSON array");
        cJSON_Delete(input_json);
        return NULL;
    }

    // Create a JSON array to hold the requirement objects
    cJSON *subsystemsInfo = parseSheet(values_array);

    int numberOfSubsystems = cJSON_GetArraySize(subsystemsInfo);

    for(int i = 0; i< numberOfSubsystems; i++){
        const cJSON *subsystem = cJSON_GetArrayItem(subsystemsInfo, i);

        if(strcmp(cJSON_GetObjectItem(subsystem, "Acronym")->valuestring, acronym) == 0){
            cJSON* result = cJSON_DetachItemFromArray(subsystemsInfo, i);
            cJSON_Delete(input_json);
            cJSON_Delete(subsystemsInfo);

            log_message(LOG_DEBUG, "Exiting function getSubsystemInfo");
            return result;
        }
    }

    log_message(LOG_ERROR, "Subsystem was not found");
    exit(1);
}

static cJSON* parseSheet(const cJSON* values_array){
    log_message(LOG_DEBUG, "Entering function parseSheet");

    int numberOfRows = cJSON_GetArraySize(values_array);
    const cJSON *headerRow = cJSON_GetArrayItem(values_array, 0);
    int numberOfColumnsInHeader = cJSON_GetArraySize(headerRow);

    cJSON *parsedSheet = cJSON_CreateArray();

    for(int i = 1; i<numberOfRows; i++){
        const cJSON *row = cJSON_GetArrayItem(values_array, i);

        int numberOfColumnsInRow = cJSON_GetArraySize(row);
        cJSON* parsedSheetRow = cJSON_CreateObject();

        if (numberOfColumnsInHeader < numberOfColumnsInRow){
            log_message(LOG_ERROR, "parseSheet: You have are missing header values in your sheet");
            return NULL;
        }

        for(int j = 0; j < numberOfColumnsInRow; j++){
            const cJSON *headerItem = cJSON_GetArrayItem(headerRow, j);
            if(strcmp(headerItem->valuestring,"")==0){
                log_message(LOG_ERROR, "parseSheet: One of your header values is empty");
                cJSON_Delete(parsedSheetRow);
                return NULL;
            }

            const cJSON *cellItem = cJSON_GetArrayItem(row, j);
            cJSON_AddStringToObject(parsedSheetRow, headerItem->valuestring, cellItem->valuestring);
        }

        cJSON_AddItemToArray(parsedSheet, parsedSheetRow);
    }


    log_message(LOG_DEBUG, "Exiting function parseSheet");
    return parsedSheet;
}

cJSON* getRequirements(const cJSON* subsystem){
    log_message(LOG_DEBUG, "Entering function getRequirements");

    const char *sheetId = cJSON_GetObjectItem(subsystem, "Req_DB Sheet Acronym and Range")->valuestring;
    const char *reqDbId = cJSON_GetObjectItem(subsystem, "Req_DB Spreadsheet ID")->valuestring;

    batchGetSheet(reqDbId, sheetId);

    log_message(LOG_DEBUG, "Exiting function getRequirements");
    return parseArrayIntoJSONRequirementList(chunk.response);
}

char* addDollarSigns(const char* characteristic){
    char* wordToReplace = duplicate_Malloc("$word$");
    wordToReplace = replaceWord_Realloc(wordToReplace, "word", characteristic);
    return wordToReplace;
}

int addSectionToPageContent(char** pageContent, const char* template, const cJSON* object, const char* item){
    log_message(LOG_DEBUG, "Entering function addSectionToPageContent");

    if(!cJSON_HasObjectItem(object, item)){
        log_message(LOG_ERROR, "addSectionToPageContent: characteristic does not exist");
        return 0;
    }

    const cJSON* jsonCharacteristic = cJSON_GetObjectItem(object, item);

    if(!cJSON_IsString(jsonCharacteristic) || strcmp(jsonCharacteristic->valuestring, "") == 0 || strcmp(jsonCharacteristic->valuestring, "N/A") == 0 || strcmp(jsonCharacteristic->valuestring, "TBD")==0){
        log_message(LOG_ERROR, "addSectionToPageContent: Characteristic has no value");
        return 0;
    }
    
    char *newSection = duplicate_Malloc(template);
    char *wordToReplace = addDollarSigns(item);

    newSection = replaceWord_Realloc(newSection, wordToReplace, jsonCharacteristic->valuestring);

    *pageContent = appendToString(*pageContent, newSection);

    free(wordToReplace);
    free(newSection);


    log_message(LOG_DEBUG, "Exiting function addSectionToPageContent");
    return 1;
}