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


char *GITHUB_API_TOKEN;
char *WIKI_API_TOKEN;
char *SLACK_API_TOKEN;
char *SHEET_API_TOKEN;
char *GOOGLE_CLIENT_ID;
char *GOOGLE_CLIENT_SECRET;
char *GOOGLE_REFRESH_TOKEN;

void initializeApiTokenVariables() {
    GITHUB_API_TOKEN = getenv("GITHUB_API_TOKEN");
    WIKI_API_TOKEN = getenv("WIKI_API_TOKEN");
    SLACK_API_TOKEN = getenv("SLACK_API_TOKEN");
    SHEET_API_TOKEN = getenv("SHEET_API_TOKEN");
    GOOGLE_CLIENT_ID = getenv("GOOGLE_CLIENT_ID");
    GOOGLE_CLIENT_SECRET = getenv("GOOGLE_CLIENT_SECRET");
    GOOGLE_REFRESH_TOKEN = getenv("GOOGLE_REFRESH_TOKEN");

    return;
}

//Callback function to handle the HTTP response, alocates memory for chunk.response
size_t writeCallback(void *data, size_t size, size_t nmemb, void *clientp) {
    // size us allways 1
    // nmemb is the size of the chunk of memory getting passed to writeCallback
    // data points to the delivered data 
    size_t realsize = size * nmemb; // set the size of the chunk of memory
    struct memory *mem = (struct memory *)clientp; //points to a struct which is the buffer to store data in

    char *ptr = realloc(mem->response, mem->size + realsize + 1);
    if(!ptr)
        return 0;  /* out of memory! */

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

//Function to extract a string value from a JSON object*/
char *jsonParserGetStringValue(char *json, char *key) {
    char *start = strstr(json, key);
    if (start == NULL) {
        fprintf(stderr, "Error: Key '%s' not found in JSON\n", key);
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
        fprintf(stderr, "Error: Malformed JSON\n");
        return NULL;
    }

    // Extract the string value
    size_t length = end - start;
    char *value = malloc(length + 1);
    strncpy(value, start, length);
    value[length] = '\0';

    return value;
}

