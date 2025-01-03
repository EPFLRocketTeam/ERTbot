/**
 * @file wikiAPI.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief Contains all of the functions which are only used to interact with the wiki APIs.
 */

#include <stdbool.h>
#include <curl/curl.h>
#include <string.h>
#include "apiHelpers.h"
#include "ERTbot_common.h"
#include "stringHelpers.h"
#include "timeHelpers.h"
#include "pageListHelpers.h"



char *template_pages_singles_query = "{\"query\":\"{pages {single(id: DefaultID){id, path, title, content, description, updatedAt, createdAt, authorId}}}\"}";
char *template_list_pages_sortByPath_query = "{\"query\":\"{pages {list(orderBy: PATH){path, title, id, updatedAt}}}\"}";
char *template_list_pages_sortByTime_query = "{\"query\":\"{pages {list(orderBy: UPDATED, orderByDirection: DESC){path, title, id, updatedAt}}}\"}";
char *template_update_page_mutation = "{\"query\":\"mutation { pages { update(id: DefaultID, content: \\\"DefaultContent\\\", isPublished: true) { responseResult { succeeded, message } } } }\"}";
char *template_render_page_mutation = "{\"query\":\"mutation { pages { render(id: DefaultID) { responseResult { succeeded, message } } } }\"}";
char *template_create_user_mutation = "{\"query\":\"mutation { users { create(email: \\\"DefaultEmail\\\", name: \\\"DefaultName\\\", prodiverKey: DefaultProviderKey, groups: DefaultGroup, mustChangePassword: true, passwordRaw: \\\"DefaultPassword\\\") { responseResult { succeeded, message } } } }";
char *template_move_page_mutation = "{\"query\":\"mutation { pages { move(id: DefaultID, destinationPath: \\\"DefaultPath\\\", destinationLocale: \\\"en\\\") { responseResult { succeeded, message } } } }\"}";
char *template_create_page_mutation = "{\"query\":\"mutation { pages { create(content: \\\"DefaultContent\\\", description: \\\"\\\", editor: \\\"markdown\\\", isPublished: true, isPrivate: false, locale: \\\"en\\\", path: \\\"DefaultPath\\\", tags: [], title: \\\"DefaultTitle\\\") { responseResult { succeeded, message, errorCode } } } }\"}";
char *template_delete_page_mutation = "{\"query\":\"mutation { pages { delete(id: DefaultID) { responseResult { succeeded, message } } } } \"}";


/**
 * @brief Sends a GraphQL query to the Wiki API using a POST request.
 *
 * @param[in] query Pointer to a string containing the GraphQL query to be sent.
 *
 * @details This function initializes libcurl, sets up a POST request to the specified Wiki API endpoint with the provided
 *          GraphQL query. It includes necessary headers such as Content-Type and Authorization. The response is handled
 *          by the `writeCallback` function. After performing the request, the function checks for errors and the HTTP status
 *          code to ensure successful completion.
 *
 * @note Ensure that `WIKI_API_TOKEN` is set correctly and the `writeCallback` function is properly defined to handle the
 *       API response.
 */
void wikiApi(char *query){
    log_message(LOG_DEBUG, "Entering function wikiApi");
    log_message(LOG_DEBUG, "wikiApi query: %s", query);

    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    resetChunkResponse();

    if (curl) {
        // Set the API URL
        curl_easy_setopt(curl, CURLOPT_URL, "https://rocket-team.epfl.ch/graphql");
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
        // Set the HTTP method to POST
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        // Set the Content-Type header
        headers = curl_slist_append(headers, "Content-Type: application/json");
        // Set the GraphQL query as the request payload
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query);
        // Add Authorization header with the API key
        char auth_header[1024];
        snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s ", WIKI_API_TOKEN);
        headers = curl_slist_append(headers, auth_header);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        // Set the callback function to handle the response
        // Send all data to this function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
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
            log_message(LOG_ERROR, "wikiApi: HTTP request failed with status code %ld", http_code);
            log_message(LOG_ERROR, "chunk.resposnse: %s", chunk.response);
            exit(-1);
        }
        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    curl_global_cleanup();

    log_message(LOG_DEBUG, "Exiting function wikiApi");
}

/**
 * @brief Constructs and sends a GraphQL query to retrieve the content of a page.
 *
 * @param[in] id Pointer to a string containing the ID of the page to retrieve.
 *
 * @details This function creates a query based on a template by replacing a placeholder with the provided page ID. It
 *          then sends the constructed query to the Wiki API using the `wikiApi` function. Memory for the query strings is
 *          dynamically allocated and freed after use.
 *
 * @note Ensure that `template_pages_singles_query` and `default_page.id` are correctly defined and that the `wikiApi`
 *       function is properly set up to handle the API request.
 */
static void getPageContentQuery(const char* id){
    log_message(LOG_DEBUG, "Entering function getPageContentQuery");

    char *modified_query = duplicate_Malloc(template_pages_singles_query); // Make a copy to modify
    modified_query = replaceWord_Realloc(modified_query, default_page.id, id);
    wikiApi(modified_query);
    free(modified_query);

    log_message(LOG_DEBUG, "Exiting function getPageContentQuery");
}

/**
 * @brief Constructs and sends a GraphQL query to retrieve a list of pages, sorted by the specified criteria.
 *
 * @param[in] sort Pointer to a string indicating the sorting criteria. Possible values are:
 *                 - "path" or "exact path" for sorting by path.
 *                 - "time" for sorting by time.
 *
 * @details This function selects and constructs a query based on the provided sorting criteria. It creates a query by
 *          duplicating a template query string and sends it to the Wiki API using the `wikiApi` function. Memory for the
 *          query string is dynamically allocated and freed after use.
 *
 * @note If the `sort` parameter does not match one of the expected values, an error message is printed.
 *
 * @warning Ensure that `template_list_pages_sortByPath_query` and `template_list_pages_sortByTime_query` are correctly
 *          defined, and that the `wikiApi` function is properly set up to handle the API request.
 */
static void getListQuery(const char *sort){
    log_message(LOG_DEBUG, "Entering function getListQuery");

    if(strcmp(sort, "path") == 0 || strcmp(sort, "exact path") == 0){
        char* temp_query = duplicate_Malloc(template_list_pages_sortByPath_query); // Make a copy to modify
        wikiApi(temp_query);
        free(temp_query);
    }
    else if(strcmp(sort, "time") == 0){
        char *temp_query = duplicate_Malloc(template_list_pages_sortByTime_query); // Make a copy to modify
        wikiApi(temp_query);
        free(temp_query);
    }
    else{
        log_message(LOG_ERROR, "Error: inappropriate sort type in getListQuery function call");
    }

    log_message(LOG_DEBUG, "Exiting function getListQuery");
}

pageList* getPage(pageList** head){
    log_message(LOG_DEBUG, "Entering function getPage");

    pageList* current = *head;
    getPageContentQuery(current->id);
    log_message(LOG_DEBUG, "%s" ,chunk.response);
    sscanf(chunk.response, "{ \"id\": %s", current->id);
    current->title = jsonParserGetStringValue(chunk.response, "\"title\"");
    current->path = jsonParserGetStringValue(chunk.response, "\"path\"");
    current->description = jsonParserGetStringValue(chunk.response, "\"description\"");
    current->content = jsonParserGetStringValue(chunk.response, "\"content\"");
    current->updatedAt = jsonParserGetStringValue(chunk.response, "\"updatedAt\"");
    current->createdAt = jsonParserGetStringValue(chunk.response, "\"createdAt\"");
    current->authorId = jsonParserGetIntValue(chunk.response, "\"authorId\"");
    log_message(LOG_DEBUG, "title: %s\n, path: %s\n, description: %s\n, content: %s\n, updatedAt: %s\n", current->title, current->path, current->description, current->content, current->updatedAt);

    freeChunkResponse();

    log_message(LOG_DEBUG, "Exiting function getPage");
    return current;
}

// Function to filter and parse the JSON string into a Node linked list
static pageList* parseJSON(pageList** head, const char* jsonString, const char* filterType, const char* filterCondition) {
    log_message(LOG_DEBUG, "Entering function parseJSON");

    if (strstr(filterCondition, "\\") != NULL) {
        filterCondition = replaceWord_Malloc(filterCondition, "\\", "");
    }

    const char* start = strstr(jsonString, "\"list\":");
    if (start == NULL) {
        log_message(LOG_ERROR, "Invalid JSON format.");
        return *head;
    }

    // Start parsing from "list"
    const char* current = start;
    const char* pathKey = "\"path\":\"";
    size_t lengthPathKey = strlen(pathKey);
    const char* titleKey = "\"title\":\"";
    size_t lengthTitleKey = strlen(titleKey);
    const char* idKey = "\"id\":";
    size_t lengthIdKey = strlen(idKey);
    const char* updatedAtKey = "\"updatedAt\":";
    size_t lengthUpdatedAtKey = strlen(updatedAtKey);
    while (1) {
        // Find "path"
        const char* pathStart = strstr(current, pathKey);
        if (pathStart == NULL) break;
        pathStart += lengthPathKey;
        const char* pathValueEnd = strchr(pathStart, '\"');
        if (pathValueEnd == NULL) break;
        size_t pathLength = pathValueEnd - pathStart;
        char* path = (char*)malloc(pathLength + 1);
        strncpy(path, pathStart, pathLength);
        path[pathLength] = '\0';

        // Find "title"
        const char* titleStart = strstr(current, titleKey);
        if (titleStart == NULL) {
            free(path);
            break;
        }
        titleStart += lengthTitleKey;
        const char* titleValueEnd = strchr(titleStart, '\"');
        if (titleValueEnd == NULL) {
            free(path);
            break;
        }
        size_t titleLength = titleValueEnd - titleStart;
        char* title = (char*)malloc(titleLength + 1);
        strncpy(title, titleStart, titleLength);
        title[titleLength] = '\0';

        // Find "id"
        const char* idStart = strstr(current, idKey);
        if (idStart == NULL) {
            free(path);
            free(title);
            break;
        }
        idStart += lengthIdKey;
        const char* idValueEnd = strchr(idStart, ',');
        if (idValueEnd == NULL) {
            free(path);
            free(title);
            break;
        }
        size_t idLength = idValueEnd - idStart;
        char* id = (char*)malloc(idLength + 1);
        strncpy(id, idStart, idLength);
        id[idLength] = '\0';

        // Find "updatedAt"
        const char* updatedAtStart = strstr(current, updatedAtKey);
        if (updatedAtStart == NULL) {
            free(path);
            free(title);
            free(id);
            break;
        }
        updatedAtStart += lengthUpdatedAtKey + 1;
        const char* updatedAtValueEnd = strchr(updatedAtStart, '\"');
        if (updatedAtValueEnd == NULL) {
            free(path);
            free(title);
            free(id);
            break;
        }
        size_t updatedAtLength = updatedAtValueEnd - updatedAtStart;
        char* updatedAt = (char*)malloc(updatedAtLength + 1);
        strncpy(updatedAt, updatedAtStart, updatedAtLength);
        updatedAt[updatedAtLength] = '\0';

        // Add page to list based on the filter condition
        if (strcmp(filterType, "path") == 0 && (strstr(path, filterCondition) != NULL || strcmp(filterCondition, "none") == 0)) {
            log_message(LOG_DEBUG, "Page found after filtering by path:\n path: %s,\n title: %s,\n id: %s,\n updatedAt: %s\n", path, title, id, updatedAt);
            *head = addPageToList(head, id, title, path, NULL, NULL, updatedAt);
            log_message(LOG_DEBUG, "Page added to list");
        }

        if (strcmp(filterType, "time") == 0 && (strcmp(filterCondition, "none") == 0 || compareTimes(filterCondition, updatedAt) == -1)) {
            log_message(LOG_DEBUG, "Page found after filtering by time:\n path: %s,\n title: %s,\n id: %s,\n updatedAt: %s\n", path, title, id, updatedAt);
            *head = addPageToList(head, id, title, path, NULL, NULL, updatedAt);
            log_message(LOG_DEBUG, "Page added to list");
        }

        if (strcmp(filterType, "exact path") == 0 && strcmp(path, filterCondition) == 0) {
            log_message(LOG_DEBUG, "Page found after filtering by exact path:\n path: %s,\n title: %s,\n id: %s,\n updatedAt: %s\n", path, title, id, updatedAt);
            *head = addPageToList(head, id, title, path, NULL, NULL, updatedAt);
            log_message(LOG_DEBUG, "Page added to list");
            free(path);
            free(title);
            free(id);
            free(updatedAt);
            break;
        }

        if (strcmp(filterType, "time") == 0 && strcmp(filterCondition, "none") != 0 && compareTimes(filterCondition, updatedAt) == 1) {
            log_message(LOG_DEBUG, "Last page found");
            free(id);
            free(title);
            free(path);
            free(updatedAt);
            break;
        }

        free(id);
        free(title);
        free(path);
        free(updatedAt);

        // Move to next page in received JSON
        current = strchr(pathValueEnd, '}');
        if (current == NULL) break;
        current++;
    }

    log_message(LOG_DEBUG, "Finished searching for pages");

    log_message(LOG_DEBUG, "Exiting function parseJSON");
    return *head;
}

void updatePageContentMutation(pageList* head){
    log_message(LOG_DEBUG, "Entering function updatePageContentMutation");

    char *temp_query = template_update_page_mutation;
    log_message(LOG_DEBUG, "About to update page (id: %s) to content: %s", head->id, head->content);
    temp_query = replaceWord_Malloc(temp_query, default_page.id, head->id);
    temp_query = replaceWord_Realloc(temp_query, default_page.content, head->content);

    log_message(LOG_DEBUG, "About to update send query: %s\n", temp_query);
    wikiApi(temp_query);

    freeChunkResponse();

    free(temp_query);

    log_message(LOG_DEBUG, "Exiting function updatePageContentMutation");
}

void renderMutation(pageList** head, bool renderEntireList){
    log_message(LOG_DEBUG, "Entering function renderMutation");

#ifndef TESTING

    pageList* current = *head;
    while (current)  {
        char *temp_query = template_render_page_mutation;
        temp_query = replaceWord_Malloc(temp_query, default_page.id, current->id);
        wikiApi(temp_query);
        free(temp_query);

        if(!renderEntireList){
            break;
        }

        current = current->next;
    }

    freeChunkResponse();

#endif

    log_message(LOG_DEBUG, "Exiting function renderMutation");
}

void movePageMutation(pageList** head){
    log_message(LOG_DEBUG, "Entering function movePageMutation");

    pageList* current = *head;
    while (current)  {
        char *temp_query = template_move_page_mutation;
        temp_query = replaceWord_Malloc(temp_query, default_page.id, current->id);
        temp_query = replaceWord_Realloc(temp_query, default_page.path, current->path);
        wikiApi(temp_query);
        free(temp_query);
        current = current->next;
    }

    freeChunkResponse();

    log_message(LOG_DEBUG, "Exiting function movePageMutation");
}

pageList* populatePageList(pageList** head, const char *filterType, const char *filterCondition){
    log_message(LOG_DEBUG, "Entering function populatePageList");

    pageList* temp = *head;
    getListQuery(filterType);
    temp = parseJSON(&temp, chunk.response, filterType, filterCondition);

    freeChunkResponse();

    log_message(LOG_DEBUG, "Exiting function populatePageList");
    return temp;
}

void createPageMutation(const char* path, const char* content, const char* title){
    log_message(LOG_DEBUG, "Entering function createPageMutation");

    char *temp_query = duplicate_Malloc(template_create_page_mutation);
    temp_query = replaceWord_Realloc(temp_query, default_page.path, path);
    temp_query = replaceWord_Realloc(temp_query, default_page.content, content);
    temp_query = replaceWord_Realloc(temp_query, default_page.title, title);
    log_message(LOG_DEBUG, "About to call the create page query %s", temp_query);

    wikiApi(temp_query);

    free(temp_query);

    log_message(LOG_DEBUG, "chunk.response after calling for a page creation: %s", chunk.response);

    log_message(LOG_DEBUG, "Exiting function createPageMutation");
}

char *fetchAndModifyPageContent(const char* pageId, const char* newPageContent, char* outputString){
    log_message(LOG_DEBUG, "Entering function fetchAndModifyPageContent");

    pageList* page = NULL;
    page = addPageToList(&page, pageId, NULL, NULL, NULL, NULL, NULL);
    page = getPage(&page);

    size_t len;
    len = strlen(page->content) + 1;

    outputString = malloc(len);
    if (outputString != NULL) {
        strlcpy(outputString, page->content, len);
    }


    size_t len2;
    len2 = strlen(newPageContent) + 1;
    page->content = realloc(page->content, len2);
    strlcpy(page->content, newPageContent, len2);

    updatePageContentMutation(page);

    freePageList(&page);

    log_message(LOG_DEBUG, "Exiting function fetchAndModifyPageContent");

    return outputString;
}

void deletePageMutation(const char* id){
    log_message(LOG_DEBUG, "Entering function deletePageMutation");

    char *temp_query = template_delete_page_mutation;
    temp_query = replaceWord_Malloc(temp_query, default_page.id, id);
    wikiApi(temp_query);
    free(temp_query);

    log_message(LOG_DEBUG, "Exiting function deletePageMutation");
}
