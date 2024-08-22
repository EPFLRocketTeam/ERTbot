/**
 * @file slackAPI.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief 
 * 
 * @details Contains all of the functions which are only used to interact with the Slack APIs.
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


#define MAX_MESSAGE_LENGTH 100000

int sendMessageToSlack( char *message) {
    log_message(LOG_DEBUG, "Entering function sendMessageToSlack");
    
    CURL *curl;
    CURLcode res;
    struct curl_slist *headerlist = NULL;
    static  char *url = "https://slack.com/api/chat.postMessage";

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(curl) {
        // Construct JSON payload for the message
        char json[MAX_MESSAGE_LENGTH];
        snprintf(json, sizeof(json), "{\"channel\":\"%s\",\"text\":\"%s\"}", SLACK_WIKI_TOOLBOX_CHANNEL, message);

        // Set the URL for Slack message posting
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the POST data (JSON payload)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);

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
            log_message(LOG_ERROR, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }
    }
    else {
        log_message(LOG_ERROR, "Failed to initialize libcurl");
        return 1;
    }

    curl_global_cleanup();
    
    log_message(LOG_DEBUG, "Exiting function sendMessageToSlack");
    return 0;
}

void checkLastSlackMessage() {
    log_message(LOG_DEBUG, "Entering function checkLastSlackMessage");
    
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    static  char buf[] = "Expect:";

    chunk.response = malloc(1);  /* grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */


    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL for Slack API conversation history
        char url[256];
        snprintf(url, sizeof(url), "https://slack.com/api/conversations.history?channel=%s&limit=1", SLACK_WIKI_TOOLBOX_CHANNEL);
        curl_easy_setopt(curl, CURLOPT_URL, url);

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
            log_message(LOG_ERROR, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Check the HTTP status code
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code != 200) {
            log_message(LOG_ERROR, "checkLastSlackMessage: HTTP request failed with status code %ld\n", http_code);
        }

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
    
    log_message(LOG_DEBUG, "Exiting function checkLastSlackMessage");
}

slackMessage* getSlackMessage(slackMessage* slackMsg) {
    log_message(LOG_DEBUG, "Entering function getSlackMessage");
    
    checkLastSlackMessage();

    slackMsg->message = jsonParserGetStringValue(chunk.response, "\"text\"");
    slackMsg->sender = jsonParserGetStringValue(chunk.response, "\"user\"");
    slackMsg->timestamp = jsonParserGetStringValue(chunk.response, "\"ts\"");

    free(chunk.response);

    
    log_message(LOG_DEBUG, "Exiting function getSlackMessage");

    return slackMsg;
}

int addSlackMember(char *channels, char *email) {
    log_message(LOG_DEBUG, "Entering function addSlackMember");
    
    CURL *curl;
    CURLcode res;
    struct curl_slist *headerlist = NULL;
    static  char *url = "https://slack.com/api/admin.users.invite";
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) {
        // Construct JSON payload for the message
        char json[2000];
        snprintf(json, sizeof(json), "{\"channel_ids\":\"%s\",\"email\":\"%s\",\"team_id\":\"T9NJWLTU6\"}", channels, email);
        // Set the URL for Slack message posting
        curl_easy_setopt(curl, CURLOPT_URL, url);
        // Set the POST data (JSON payload)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
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
            log_message(LOG_ERROR, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }
    }
    else {
        log_message(LOG_ERROR, "Failed to initialize libcurl");
        return 1;
    }
    curl_global_cleanup();

    
    log_message(LOG_DEBUG, "Exiting function addSlackMember");
    return 0;
}