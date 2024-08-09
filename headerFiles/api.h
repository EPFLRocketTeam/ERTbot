#ifndef API_H
#define API_H

//Callback function to handle the HTTP response, alocates memory for chunk.response
size_t writeCallback(void *data, size_t size, size_t nmemb, void *clientp);

//Function to extract a string value from a JSON object*/
char *jsonParserGetStringValue(char *json, char *key);

#endif