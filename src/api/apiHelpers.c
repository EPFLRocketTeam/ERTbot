/**
 * @file api.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief contains helper functions which are used by several api handling functions
 */

#include <stdlib.h>
#include <string.h>
#include "ERTbot_common.h"


char *GITHUB_API_TOKEN;
char *WIKI_API_TOKEN;
char *SLACK_API_TOKEN;
char *SHEET_API_TOKEN;
char *GOOGLE_CLIENT_ID;
char *GOOGLE_CLIENT_SECRET;
char *GOOGLE_REFRESH_TOKEN;

void initializeApiTokenVariables() {
    log_function_entry(__func__);

    GITHUB_API_TOKEN = getenv("GITHUB_API_TOKEN");
    WIKI_API_TOKEN = getenv("WIKI_API_TOKEN");
    SLACK_API_TOKEN = getenv("SLACK_API_TOKEN");
    GOOGLE_CLIENT_ID = getenv("GOOGLE_CLIENT_ID");
    GOOGLE_CLIENT_SECRET = getenv("GOOGLE_CLIENT_SECRET");
    GOOGLE_REFRESH_TOKEN = getenv("GOOGLE_REFRESH_TOKEN");


    log_function_exit(__func__);
    return;
}


size_t writeCallback(const void *data, size_t size, size_t nmemb, void *clientp) {
    log_function_entry(__func__);

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


    log_function_exit(__func__);
    return realsize;
}

char *jsonParserGetStringValue(const char *json, char *key) {
    log_function_entry(__func__);

    const char *start = strstr(json, key);
    if (start == NULL) {
        log_message(LOG_ERROR, __func__, "Error: Key '%s' not found in JSON", key);
        return NULL;
    }

    // Move to the value part of the key-value pair
    start += strlen(key) + 2;  // Skip key and the quote + colon

    const char *end;

    // Find the end of the string (closing quote)
    if(!strcmp(key, "\"content\"")){
        end = strstr(start, "\",\"description\"");
    }else{
        end = strchr(start, '"');
    }
    if (end == NULL) {
        log_message(LOG_ERROR, __func__, "Error: Malformed JSON");
        return NULL;
    }

    // Extract the string value
    size_t length = end - start;
    char *value = malloc(length + 1);
    strncpy(value, start, length);
    value[length] = '\0';

    log_message(LOG_DEBUG, __func__, "Value for key: %s is: %s", key, value);
    log_function_exit(__func__);
    return value;
}

char *jsonParserGetIntValue(const char *json, char *key) {
    log_function_entry(__func__);

    // Find the key in the JSON string
    const char *start = strstr(json, key);
    if (start == NULL) {
        log_message(LOG_ERROR, __func__, "Error: Key '%s' not found in JSON", key);
        return NULL;  // Return error code
    }

    // Move to the value part of the key-value pair
    start += strlen(key) + 1;  // Skip key and the colon (assumes no spaces, formatted JSON might require handling)

    // Find the end of the integer value (assume it ends at a comma or closing brace)
    const char *end = strpbrk(start, ",}");
    if (end == NULL) {
        log_message(LOG_ERROR, __func__, "Error: Malformed JSON");
        return NULL;  // Return error code
    }

    // Extract the integer value as a string
    size_t length = end - start;
    char *valueStr = malloc(length + 1);
    if (valueStr == NULL) {
        log_message(LOG_ERROR, __func__, "Error: Memory allocation failed");
        return NULL;  // Return error code
    }

    strncpy(valueStr, start, length);
    valueStr[length] = '\0';

    // Convert the extracted string to an integer

    log_function_exit(__func__);
    return valueStr;  // Return success
}

void freeChunkResponse(){
    if(chunk.response){
        free(chunk.response);

        chunk.response = NULL;
        chunk.size = 0;
    }
}

void resetChunkResponse(){
    if (chunk.response != NULL){
        free(chunk.response);
    }

    chunk.response = malloc(1);
    chunk.size = 0;
}