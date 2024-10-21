/**
 * @file api.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief contains helper functions which are used by several api handling functions
 * 
 * @todo revamp jsonParserGetStringValue to uses cJSON
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


char *GITHUB_API_TOKEN;
char *WIKI_API_TOKEN;
char *SLACK_API_TOKEN;
char *SHEET_API_TOKEN;
char *GOOGLE_CLIENT_ID;
char *GOOGLE_CLIENT_SECRET;
char *GOOGLE_REFRESH_TOKEN;

void initializeApiTokenVariables() {
    log_message(LOG_DEBUG, "Entering function initializeApiTokenVariables");
    
    GITHUB_API_TOKEN = getenv("GITHUB_API_TOKEN");
    WIKI_API_TOKEN = getenv("WIKI_API_TOKEN");
    SLACK_API_TOKEN = getenv("SLACK_API_TOKEN");
    SHEET_API_TOKEN = getenv("SHEET_API_TOKEN");
    GOOGLE_CLIENT_ID = getenv("GOOGLE_CLIENT_ID");
    GOOGLE_CLIENT_SECRET = getenv("GOOGLE_CLIENT_SECRET");
    GOOGLE_REFRESH_TOKEN = getenv("GOOGLE_REFRESH_TOKEN");

    
    log_message(LOG_DEBUG, "Exiting function initializeApiTokenVAriables");
    return;
}

size_t writeCallback(void *data, size_t size, size_t nmemb, void *clientp) {
    log_message(LOG_DEBUG, "Entering function writeCallback");
    
    size_t realsize = size * nmemb; // set the size of the chunk of memory
    struct memory *mem = (struct memory *)clientp; //points to a struct which is the buffer to store data in

    char *ptr = realloc(mem->response, mem->size + realsize + 1);
    if(!ptr){
        return 0;
    }

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    
    log_message(LOG_DEBUG, "Exiting function writeCallback");
    return realsize;
}

char *jsonParserGetStringValue(char *json, char *key) {
    log_message(LOG_DEBUG, "Entering function jsonParserGetStringValue");
    
    char *start = strstr(json, key);
    if (start == NULL) {
        log_message(LOG_ERROR, "Error: Key '%s' not found in JSON", key);
        return NULL;
    }

    // Move to the value part of the key-value pair
    start += strlen(key) + 2;  // Skip key and the quote + colon

    char *end;
    
    // Find the end of the string (closing quote)
    if(!strcmp(key, "\"content\"")){
        end = strstr(start, "\",\"description\"");
    }else{
        end = strchr(start, '"');
    }
    if (end == NULL) {
        log_message(LOG_ERROR, "Error: Malformed JSON");
        return NULL;
    }

    // Extract the string value
    size_t length = end - start;
    char *value = malloc(length + 1);
    strncpy(value, start, length);
    value[length] = '\0';

    log_message(LOG_DEBUG, "Value for key: %s is: %s", key, value);
    log_message(LOG_DEBUG, "Exiting function jsonParserGetStringValue");
    return value;
}

char *jsonParserGetIntValue(char *json, char *key) {
    log_message(LOG_DEBUG, "Entering function jsonParserGetIntValue");
    
    // Find the key in the JSON string
    char *start = strstr(json, key);
    if (start == NULL) {
        log_message(LOG_ERROR, "Error: Key '%s' not found in JSON", key);
        return NULL;  // Return error code
    }

    // Move to the value part of the key-value pair
    start += strlen(key) + 1;  // Skip key and the colon (assumes no spaces, formatted JSON might require handling)

    // Find the end of the integer value (assume it ends at a comma or closing brace)
    char *end = strpbrk(start, ",}");
    if (end == NULL) {
        log_message(LOG_ERROR, "Error: Malformed JSON");
        return NULL;  // Return error code
    }

    // Extract the integer value as a string
    size_t length = end - start;
    char *valueStr = malloc(length + 1);
    if (valueStr == NULL) {
        log_message(LOG_ERROR, "Error: Memory allocation failed");
        return NULL;  // Return error code
    }

    strncpy(valueStr, start, length);
    valueStr[length] = '\0';

    // Convert the extracted string to an integer

    log_message(LOG_DEBUG, "Exiting function jsonParserGetIntValue");
    return valueStr;  // Return success
}