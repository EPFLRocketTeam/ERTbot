/**
 * @file slackAPI.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief
 *
 * @details Contains all of the functions which are only used to interact with the Slack APIs.
 */

#include <curl/curl.h>
#include <ERTbot_config.h>
#include "apiHelpers.h"
#include "ERTbot_common.h"
#include "stringHelpers.h"

slackMessage* commandStatusMessage;

#define MAX_MESSAGE_LENGTH 100000

static int slackPostApi(char* url, char* postFields){
    log_function_entry(__func__);

    CURL *curl;
    CURLcode res;
    struct curl_slist *headerlist = NULL;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    resetChunkResponse();

    if(curl) {
        // Set the URL for Slack message posting
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

        // Set the POST data (JSON payload)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields);

        // Set the write function to ignore the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        // Add headers
        headerlist = curl_slist_append(headerlist, "Content-Type: application/json");
        headerlist = curl_slist_append(headerlist, "charset: utf-8");

        char auth_header[128];
        snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", SLACK_API_TOKEN);
        headerlist = curl_slist_append(headerlist, auth_header);

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

        // Perform the request
        res = curl_easy_perform(curl);

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headerlist);

        if(res != CURLE_OK) {
            log_message(LOG_ERROR, __func__, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }
    }
    else {
        log_message(LOG_ERROR, __func__, "Failed to initialize libcurl");
        return 1;
    }

    curl_global_cleanup();

    log_function_exit(__func__);
    return 0;
}

int sendMessageToSlackAPI(char *message){
    char postFields[MAX_MESSAGE_LENGTH];
    snprintf(postFields, sizeof(postFields), "{\"channel\":\"%s\",\"text\":\"%s\"}", SLACK_WIKI_TOOLBOX_CHANNEL, message);
    char *url = "https://slack.com/api/chat.postMessage";

    return slackPostApi(url, postFields);
}

int updateSlackMessage(slackMessage* slackMessage) {
#ifndef TESTING
    char postFields[MAX_MESSAGE_LENGTH];
    snprintf(postFields, sizeof(postFields), "{\"channel\":\"%s\",\"ts\":\"%s\",\"text\":\"%s\"}", SLACK_WIKI_TOOLBOX_CHANNEL, slackMessage->timestamp, slackMessage->message);
    char *url = "https://slack.com/api/chat.update";
    int returnValue = slackPostApi(url, postFields);
    freeChunkResponse();
    return returnValue;
#endif
}

int sendMessageToSlack(char *message) {
    log_function_entry(__func__);

    int returnValue = sendMessageToSlackAPI(message);

    freeChunkResponse();

    return returnValue;
}

void checkLastSlackMessage() {
    log_function_entry(__func__);

    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    static  char buf[] = "Expect:";

    resetChunkResponse();

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL for Slack API conversation history
        char url[256];
        snprintf(url, sizeof(url), "https://slack.com/api/conversations.history?channel=%s&limit=1", SLACK_WIKI_TOOLBOX_CHANNEL);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

        // Add headers
        headers = curl_slist_append(headers, buf);
        char auth_header[128];
        snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", SLACK_API_TOKEN);
        headers = curl_slist_append(headers, auth_header);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the write callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        // Perform the HTTP GET request
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

        // Perform the HTTP request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            log_message(LOG_ERROR, __func__, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Check the HTTP status code
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code != 200) {
            log_message(LOG_ERROR, __func__, "checkLastSlackMessage: HTTP request failed with status code %ld", http_code);
            log_message(LOG_ERROR, __func__, "chunk.resposnse: %s", chunk.response);
        }

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();

    log_function_exit(__func__);
}

slackMessage* getSlackMessage(slackMessage* slackMsg) {
    log_function_entry(__func__);

    checkLastSlackMessage();

    slackMsg->message = jsonParserGetStringValue(chunk.response, "\"text\"");
    slackMsg->sender = jsonParserGetStringValue(chunk.response, "\"user\"");
    slackMsg->timestamp = jsonParserGetStringValue(chunk.response, "\"ts\"");

    freeChunkResponse();

    log_function_exit(__func__);

    return slackMsg;
}

slackMessage* sendUpdatedableSlackMessage(slackMessage* slackMsg) {
    log_function_entry(__func__);

    sendMessageToSlackAPI(slackMsg->message);

    log_message(LOG_DEBUG, __func__, "chunk.response: %s", chunk.response);

    if(slackMsg->timestamp){
        free(slackMsg->timestamp);
        slackMsg->timestamp = NULL;
    }

    slackMsg->timestamp = jsonParserGetStringValue(chunk.response, "\"ts\"");

    freeChunkResponse();

    log_function_exit(__func__);

    return slackMsg;
}

void sendLoadingBar(const int currentValue, const int totalValue){
    log_function_entry(__func__);
    

#ifndef TESTING


    if (totalValue <= 0 || currentValue < 0 || currentValue > totalValue) {
        fprintf(stderr, "Invalid values for currentValue or totalValue\n");
        return;
    }

    const int barWidth = 20; // Width of the loading bar in characters
    int progress = (int)((double)currentValue / totalValue * barWidth);

    // Create the loading bar string
    char loadingBar[barWidth + 3]; // 20 bars + 2 brackets + null terminator
    loadingBar[0] = '[';
    for (int i = 1; i <= barWidth; i++) {
        loadingBar[i] = (i <= progress) ? '#' : '-';
    }
    loadingBar[barWidth + 1] = ']';
    loadingBar[barWidth + 2] = '\0';

    // Calculate the percentage
    int percentage = (int)((double)currentValue / totalValue * 100);

    // Create the final message string
    char newMessage[100];
    snprintf(newMessage, sizeof(newMessage), "%s %d%% (%d/%d)", loadingBar, percentage, currentValue, totalValue);

    commandStatusMessage->message = newMessage;

    if (currentValue % (totalValue / 10) == 0) {
        updateSlackMessage(commandStatusMessage);
    }

#endif

    
    log_function_exit(__func__);
    return;
}

void updateCommandStatusMessage(char *newStatusMessage){
#ifndef TESTING
    commandStatusMessage->message = newStatusMessage;
    updateSlackMessage(commandStatusMessage);
#endif
}

void initialiseSlackCommandStatusMessage(){

    log_function_entry(__func__);
    
    commandStatusMessage = (slackMessage*)malloc(sizeof(slackMessage));

    commandStatusMessage->message = NULL;
    commandStatusMessage->timestamp = NULL;
    commandStatusMessage->sender = NULL;

    
    log_function_exit(__func__);

    return ;
}

void freeSlackCommandStatusMessageVariables(){
    log_function_entry(__func__);
    
    
    if(commandStatusMessage->message){
        free(commandStatusMessage->message);
        commandStatusMessage->message = NULL;
    }

    if(commandStatusMessage->timestamp){
        free(commandStatusMessage->timestamp);
        commandStatusMessage->timestamp = NULL;
    }

    if(commandStatusMessage->sender){
        free(commandStatusMessage->sender);
        commandStatusMessage->sender = NULL;
    }
    
    
    log_function_exit(__func__);
    return;
}

void sendStartingStatusMessage(const char *commandName){
    log_function_entry(__func__);

    commandStatusMessage->message = duplicate_Malloc("Starting ");
    commandStatusMessage->message = appendToString(commandStatusMessage->message, commandName);

    sendUpdatedableSlackMessage(commandStatusMessage);
    
    free(commandStatusMessage->message);
    commandStatusMessage->message = NULL;

    
    log_function_exit(__func__);

    return;
}

void sendCompletedStatusMessage(const char *commandName){
    log_function_entry(__func__);
    

    commandStatusMessage->message = duplicate_Malloc("Finished ");
    commandStatusMessage->message = appendToString(commandStatusMessage->message, commandName);

    updateSlackMessage(commandStatusMessage);
    
    freeSlackCommandStatusMessageVariables();

    
    log_function_exit(__func__);
    return;
}