/**
 * @file sheetAPI.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief Contains all of the functions which are only used to interact with the Google Sheets APIs.
 *
 */

#include <curl/curl.h>
#include <string.h>
#include "ERTbot_common.h"
#include "apiHelpers.h"
#include "stringHelpers.h"



char *template_batch_update_url = "https://sheets.googleapis.com/v4/spreadsheets/DefaultSheetID/values:batchUpdate";
char *template_batch_get_url = "https://sheets.googleapis.com/v4/spreadsheets/DefaultSheetID/values/DefaultRange";
char *template_batch_update_query = "{\"valueInputOption\": \"USER_ENTERED\",\"data\": [{\"range\": \"DefaultRange\",\"majorDimension\": \"ROWS\",\"values\": DefaultValues}],\"includeValuesInResponse\": true,\"responseValueRenderOption\": \"FORMATTED_VALUE\",\"responseDateTimeRenderOption\": \"SERIAL_NUMBER\"}";

//char *query = "{\"valueInputOption\": \"USER_ENTERED\",\"data\": [{\"range\": \"Sheet1!A1:C4\",\"majorDimension\": \"ROWS\",\"values\": [[\"Item\", \"Cost\", \"Review\"],[\"Coffee\", 2.50, 5]]}],\"includeValuesInResponse\": true,\"responseValueRenderOption\": \"FORMATTED_VALUE\",\"responseDateTimeRenderOption\": \"SERIAL_NUMBER\"}";

void sheetAPI(char *query, char *url, char *requestType) {
    log_message(LOG_DEBUG, "Entering function sheetAPI");

    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    resetChunkResponse();

    if (curl) {
        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
        // Set the HTTP headers
        headers = curl_slist_append(headers, "Content-Type: application/json");
        char auth_header[1024];
        snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s ", SHEET_API_TOKEN);
        headers = curl_slist_append(headers, auth_header);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        // Set the request type to PUT
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, requestType);
        // Set the query for the request
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query);
        // Set the callback function to handle the response
        // Send all data to this function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        // Perform the request
        res = curl_easy_perform(curl);
        // Check for errors
        if (res != CURLE_OK) {
            log_message(LOG_ERROR, "sheetAPI: curl_easy_perform() failed: %s", curl_easy_strerror(res));
        }
        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();

    log_message(LOG_DEBUG, "Exiting function sheetAPI");
}

void refreshOAuthToken() {
    log_message(LOG_DEBUG, "Entering function refreshOAuthToken");

    CURL *curl;
    CURLcode res;

    if (GOOGLE_CLIENT_ID == NULL || GOOGLE_CLIENT_SECRET == NULL || GOOGLE_REFRESH_TOKEN == NULL) {
        log_message(LOG_ERROR, "Environment variables not set.");
        return;
    }

    // Set up the data for the POST request
    char postfields[1024];
    snprintf(postfields, sizeof(postfields), "client_id=%s&client_secret=%s&refresh_token=%s&grant_type=refresh_token", GOOGLE_CLIENT_ID, GOOGLE_CLIENT_SECRET, GOOGLE_REFRESH_TOKEN);

    resetChunkResponse();

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(curl) {
        // Set the URL for the token request
        curl_easy_setopt(curl, CURLOPT_URL, "https://oauth2.googleapis.com/token");
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
        // Specify that we want to send a POST request
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Set the POST fields
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields);

        // Pass the callback function to handle the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);

        // Pass the memory structure to the callback function
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        // Perform the request and get the response code
        res = curl_easy_perform(curl);

        // Check for errors
        if(res != CURLE_OK) {
            log_message(LOG_ERROR, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            //Yes the key value has an extra : compared to when the same function is called in wikiAPI functions
            //see the note on the jsonParserGetStringValue function
            SHEET_API_TOKEN = jsonParserGetStringValue(chunk.response, "\"access_token\":");
        }

        // Clean up
        curl_easy_cleanup(curl);
        freeChunkResponse();
    }

    curl_global_cleanup();

    log_message(LOG_DEBUG, "Exiting function refreshOAuthToken");
    return;
}

void batchUpdateSheet(const char *sheetId, const char *range, const char *values){
    log_message(LOG_DEBUG, "Entering function batchUpdateSheet");

    char *requestType = "POST";
    char *modified_url = strdup(template_batch_update_url); // Make a copy to modify
    modified_url = replaceWord_Realloc(modified_url, "DefaultSheetID", sheetId);
    char *modified_query = strdup(template_batch_update_query); // Make a copy to modify
    modified_query = replaceWord_Realloc(modified_query, "DefaultRange", range);
    modified_query = replaceWord_Realloc(modified_query, "DefaultValues", values);


    sheetAPI(modified_query, modified_url, requestType);

    free(modified_query);
    free(modified_url);

    log_message(LOG_DEBUG, "Exiting function batchUpdateSheet");
}

void batchGetSheet(const char *sheetId, const char *range){
    log_message(LOG_DEBUG, "Entering function batchGetSheet");

    refreshOAuthToken();

    char *requestType = "GET";
    char *query = "";
    char *modified_url = strdup(template_batch_get_url); // Make a copy to modify
    modified_url = replaceWord_Realloc(modified_url, "DefaultSheetID", sheetId);
    modified_url = replaceWord_Realloc(modified_url, "DefaultRange", range);

    sheetAPI(query, modified_url, requestType);

    free(modified_url);

    log_message(LOG_DEBUG, "Exiting function batchGetSheet");
}
