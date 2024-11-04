#include <string.h>
#include <cjson/cJSON.h>
#include "ERTbot_config.h"
#include "ERTbot_common.h"
#include "sheetAPI.h"
#include "apiHelpers.h"

static cJSON* parseSheet(const cJSON* values_array, cJSON* parsedSheet);

cJSON* parseArrayIntoJSONRequirementList(char *input_str) {
    log_message(LOG_DEBUG, "Entering function parseArrayIntoJSONRequirementList");

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
    requirements = parseSheet(values_array, requirements);
    if (!requirements) {
        log_message(LOG_ERROR, "Error creating JSON array");
        cJSON_Delete(json);
        cJSON_Delete(input_json);
        return NULL;
    }

    cJSON_AddItemToObject(json, "requirements", requirements);

    log_message(LOG_DEBUG, "Exiting function parseArrayIntoJSONRequirementList");

    if(chunk.response){
        free(chunk.response);
    }

    cJSON_Delete(input_json);

    return json;
}

cJSON* getSubsystemInfo(char* acronym){
    log_message(LOG_DEBUG, "Entering function getSubsystemInfo");

    batchGetSheet("1iB1yl2Nre95kD1g6TFDYvvLe0g5QzghtAHdnxNTD4tg", "INFO!A2:G20");

    log_message(LOG_DEBUG, "chunk.response: %s", chunk.response);

    cJSON *input_json = cJSON_Parse(chunk.response);
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
    cJSON *subsystemsInfo = cJSON_CreateArray();
    subsystemsInfo = parseSheet(values_array, subsystemsInfo);

    int numberOfSubsystens = cJSON_GetArraySize(subsystemsInfo);

    for(int i = 0; i< numberOfSubsystens; i++){
        cJSON *subsystem = cJSON_GetArrayItem(subsystemsInfo, i);

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

static cJSON* parseSheet(const cJSON* values_array, cJSON* parsedSheet){
    log_message(LOG_DEBUG, "Entering function parseSheet");

    int numberOfRows = cJSON_GetArraySize(values_array);
    cJSON *headerRow = cJSON_GetArrayItem(values_array, 0);
    int numberOfColumnsInHeader = cJSON_GetArraySize(headerRow);


    for(int i = 1; i<numberOfRows; i++){
        cJSON *row = cJSON_GetArrayItem(values_array, i);

        int numberOfColumnsInRow = cJSON_GetArraySize(row);
        cJSON* parsedSheetRow = cJSON_CreateObject();

        for(int j = 0; j < numberOfColumnsInRow; j++){
            cJSON *headerItem = cJSON_GetArrayItem(headerRow, j);
            cJSON *cellItem = cJSON_GetArrayItem(row, j);
            cJSON_AddStringToObject(parsedSheetRow, headerItem->valuestring, cellItem->valuestring);
        }

        cJSON_AddItemToArray(parsedSheet, parsedSheetRow);
    }

    log_message(LOG_DEBUG, "Exiting function parseSheet");
    return parsedSheet;
}
