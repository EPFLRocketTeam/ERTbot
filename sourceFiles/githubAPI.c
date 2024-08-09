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


//Receives imagePath and pagePath get the image from github using it's API, save the image to (LOCAL_FILE_PATH/pagePath/imagePath)
int fetchImage(char* imagePath, char * pagePath){
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
                printf("Error opening file.\n");
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
                printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                return 0;
            }

                printf("%s downloaded successfully!\n", imagePath);

        } else {
            printf("Failed to initialize CURL.\n");
            return 0;
        }

        curl_global_cleanup();

    return 1;
}

// Callback function to write downloaded data to file
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}