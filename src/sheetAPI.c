/**
 * @file sheetAPI.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief Contains all of the functions which are only used to interact with the Google Sheets APIs.
 *
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


char *template_batch_update_url = "https://sheets.googleapis.com/v4/spreadsheets/DefaultSheetID/values:batchUpdate";
char *template_batch_get_url = "https://sheets.googleapis.com/v4/spreadsheets/DefaultSheetID/values/DefaultRange";
char *template_batch_update_query = "{\"valueInputOption\": \"USER_ENTERED\",\"data\": [{\"range\": \"DefaultRange\",\"majorDimension\": \"ROWS\",\"values\": DefaultValues}],\"includeValuesInResponse\": true,\"responseValueRenderOption\": \"FORMATTED_VALUE\",\"responseDateTimeRenderOption\": \"SERIAL_NUMBER\"}";

//char *query = "{\"valueInputOption\": \"USER_ENTERED\",\"data\": [{\"range\": \"Sheet1!A1:C4\",\"majorDimension\": \"ROWS\",\"values\": [[\"Item\", \"Cost\", \"Review\"],[\"Coffee\", 2.50, 5]]}],\"includeValuesInResponse\": true,\"responseValueRenderOption\": \"FORMATTED_VALUE\",\"responseDateTimeRenderOption\": \"SERIAL_NUMBER\"}";

void sheetAPI(char *query, char *url, char *requestType) {
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    chunk.response = malloc(1);  /* grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    if (curl) {
        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, url);
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
            fprintf(stderr, "sheetAPI: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
}

void batchUpdateSheet(char *sheetId, char *range, char *values){
    char *requestType = "POST";
    char *temp_url = strdup(template_batch_update_url); // Make a copy to modify
    char *modified_url = replaceWord(temp_url, "DefaultSheetID", sheetId);
    char *temp_query = strdup(template_batch_update_query); // Make a copy to modify
    char *modified_query = replaceWord(temp_query, "DefaultRange", range);
    modified_query = replaceWord(modified_query, "DefaultValues", values);
    
    //fprintf(stderr, "modified_query: %s\n", modified_query);
    //fprintf(stderr, "sheetAPI\n");
    sheetAPI(modified_query, modified_url, requestType);

    //fprintf(stderr, "chunk.response: %s\n", chunk.response);

    free(modified_query);
    free(modified_url);
}

void batchGetSheet(char *sheetId, char *range){
    char *requestType = "GET";
    char *query = "";
    char *temp_url = strdup(template_batch_get_url); // Make a copy to modify
    char *modified_url = replaceWord(temp_url, "DefaultSheetID", sheetId);
    modified_url = replaceWord(modified_url, "DefaultRange", range);

    sheetAPI(query, modified_url, requestType);


    free(temp_url);
    free(modified_url);
}

void refreshOAuthToken() {
    CURL *curl;
    CURLcode res;

    if (GOOGLE_CLIENT_ID == NULL || GOOGLE_CLIENT_SECRET == NULL || GOOGLE_REFRESH_TOKEN == NULL) {
        fprintf(stderr, "Environment variables not set.\n");
        return;
    }

    // Set up the data for the POST request
    char postfields[1024];
    snprintf(postfields, sizeof(postfields), "client_id=%s&client_secret=%s&refresh_token=%s&grant_type=refresh_token", GOOGLE_CLIENT_ID, GOOGLE_CLIENT_SECRET, GOOGLE_REFRESH_TOKEN);

    chunk.response = malloc(1);  /* grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(curl) {
        // Set the URL for the token request
        curl_easy_setopt(curl, CURLOPT_URL, "https://oauth2.googleapis.com/token");

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
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Output the response
            fprintf(stderr, "Response: %s\n", chunk.response);

            // Here you would parse the JSON response to extract the access token.
            // The response would look something like this:
            // {
            //   "access_token": "ya29.a0AfH6SM...",
            //   "expires_in": 3599,
            //   "scope": "https://www.googleapis.com/auth/spreadsheets",
            //   "token_type": "Bearer"
            // }
            //
            // You can use a JSON parser to extract the "access_token" from this response.
        }

        // Clean up
        curl_easy_cleanup(curl);
        free(chunk.response);
    }

    curl_global_cleanup();

    return;
}