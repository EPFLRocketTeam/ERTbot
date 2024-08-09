#ifndef GITHUBAPI_H
#define GITHUBAPI_H

//Receives imagePath and pagePath get the image from github using it's API, save the image to (LOCAL_FILE_PATH/pagePath/imagePath)
int fetchImage(char* imagePath, char * pagePath);

// Callback function to write downloaded data to file
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);

#endif