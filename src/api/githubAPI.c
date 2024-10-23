/**
 * @file githubAPI.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief Contains all of the functions which are only used to interact with the Github APIs.
 * 
 * @todo reactivate a github api key, test the functions on the ERTbot_server
 */

#include <curl/curl.h>
#include <string.h>
#include "ERTbot_common.h"
#include "ERTbot_config.h"
#include "apiHelpers.h"
#include "stringHelpers.h"


/**
 * @brief Creates any missing directories in the specified path.
 * 
 * @param[in] path The full path of directories to create. The function will create directories as needed, including any intermediate
 *                 directories that do not exist.
 * 
 * @details This function processes the given path, creating directories one by one. It starts from the root of the path and works its
 *          way down, creating each directory in turn if it does not already exist. The path is tokenized by '/' to handle each directory
 *          level. It uses the `mkdir` function to create directories and checks for existing directories with `stat`. If an error occurs 
 *          while creating a directory, the function prints an error message and stops.
 * 
 * @note The function assumes that the path is a valid string and that the application has the necessary permissions to create directories 
 *       in the specified location. The `mkdir` function is used with mode `0755`, which sets the directory permissions to readable and 
 *       executable by everyone and writable by the owner.
 */
static void createMissingFolders(char *path);


int fetchImage(char* imagePath, char * pagePath){
    log_message(LOG_DEBUG, "Entering function fetchImage");
    
    CURL *curl;
        CURLcode res;
        FILE *fp;
        struct curl_slist *headers = NULL;

        curl_global_init(CURL_GLOBAL_DEFAULT);

        // Initialize curl handle
        curl = curl_easy_init();
        if (curl) {

            char localImagePath[600];
            snprintf(localImagePath, sizeof(localImagePath), "%s%s%s", LOCAL_FILE_PATH, pagePath, imagePath);
            createMissingFolders(localImagePath);
            fp = fopen(localImagePath, "wb");
            if (fp == NULL) {
                log_message(LOG_ERROR, "Error opening file.");
                return 0;
            }

            char imageURL[300];
            snprintf(imageURL, sizeof(imageURL), "%s%s", REPO_URL, imagePath);


            // Set URL to download from
            curl_easy_setopt(curl, CURLOPT_URL, imageURL);

            headers = curl_slist_append(headers, "Accept: application/vnd.github.raw");

            // Set GitHub Personal Access Token for authentication
            char auth_header[300];
            snprintf(auth_header, sizeof(auth_header), "Authorization: token %s", GITHUB_API_TOKEN);
            headers = curl_slist_append(headers, auth_header);

            headers = curl_slist_append(headers, "User-Agent: " USER_AGENT);

            headers = curl_slist_append(headers, "X-GitHub-Api-Version: 2022-11-28");



            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // Set callback function to write data to file
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

            // Perform the request
            res = curl_easy_perform(curl);

            // Clean up
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            fclose(fp);

            if (res != CURLE_OK) {
                log_message(LOG_ERROR, "curl_easy_perform() failed: %s", curl_easy_strerror(res));
                return 0;
            }

                log_message(LOG_INFO, "%s downloaded successfully!", imagePath);

        } else {
            log_message(LOG_ERROR, "Failed to initialize CURL.");
            return 0;
        }

        curl_global_cleanup();

    
    log_message(LOG_DEBUG, "Exiting function fetchImage");
    return 1;
}

// Callback function to write downloaded data to file
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

static void createMissingFolders(char *path){
    log_message(LOG_DEBUG, "Entering function createMissingFolders");
    
    char *dup_path = strdup(path);  // Duplicate the path to modify
    dup_path = removeLastFolder(dup_path);
    char *token = strtok(dup_path, "/"); // Tokenize by "/"
    char curr_path[1024] = "";  // Current path being processed
    strcat(curr_path, "/");  // Starting with root directory

    // Iterate through each token
    while (token != NULL) {
        strcat(curr_path, token);  // Append the current token to the path
        strcat(curr_path, "/");    // Add a '/' for the next token

        // Create directory if it doesn't exist
        struct stat st = {0};
        if (stat(curr_path, &st) == -1) {
            if (mkdir(curr_path, 0755) != 0) {
                log_message(LOG_ERROR, "Failed to create directory: %s\n", curr_path);
                free(dup_path);
                return;
            } else {
                log_message(LOG_DEBUG, "Created directory: %s\n", curr_path);
            }
        }

        token = strtok(NULL, "/");  // Get the next token
    }

    
    log_message(LOG_DEBUG, "Exiting function createMissingFolders");
    free(dup_path);
}