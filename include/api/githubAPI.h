#ifndef ERTBOT_GITHUBAPI_H
#define ERTBOT_GITHUBAPI_H

#include <stdio.h>
#include <stdlib.h>


/**
 * @brief Fetches an image from a remote repository (on github) and saves it to a local path.
 * 
 * This function uses the libcurl library to download an image from a specified URL and save it to a local file. It handles HTTP authentication and sets appropriate headers for the request.
 * 
 * @param imagePath The path of the image in the remote repository.
 * @param pagePath The local path where the image will be saved. The image will be saved in the `LOCAL_FILE_PATH` directory concatenated with `pagePath` and `imagePath`.
 * 
 * @return int Returns 1 on successful download and 0 on failure.
 * 
 * @details
 * - Initializes libcurl and sets up the handle for the HTTP request.
 * - Constructs the local file path and ensures that any missing directories are created using `createMissingFolders`.
 * - Sets up the URL for the image and HTTP headers, including authentication using a GitHub Personal Access Token.
 * - Writes the downloaded image data to a local file using a custom `write_data` callback function.
 * - Cleans up resources including the libcurl handle, file pointer, and HTTP headers.
 */
int fetchImage(char* imagePath, char * pagePath);

// Callback function to write downloaded data to file
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);

#endif