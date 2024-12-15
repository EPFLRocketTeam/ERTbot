#ifndef ERTBOT_API_HELPERS_H
#define ERTBOT_API_HELPERS_H

#include <stddef.h>


extern char *GITHUB_API_TOKEN;
extern char *WIKI_API_TOKEN;
extern char *SLACK_API_TOKEN;
extern char *SHEET_API_TOKEN;
extern char *GOOGLE_CLIENT_ID;
extern char *GOOGLE_CLIENT_SECRET;
extern char *GOOGLE_REFRESH_TOKEN;

/**
 * @brief Initalises the API token by fetching the values from the server's system evironment variables
 */
void initializeApiTokenVariables();

/**
 * @brief Function used to store the callback data after executing a http request.
 *
 * @param[in] Pointer to the delivered data (which was stored by libcurl).
 * @param[in] size Size in bytes of each element of data.
 * @param[in] nmemb Number of elements in the data (size x nmemb = total size in bytes).
 * @param[in] clientp Pointer to memory chunk which stores the response data acceable by other funcitons.
 *                    clientp is set using CURLOPT_WRITEDATA.
 *
 * @return size_t The number of bytes successfully written to the memory buffer.
 *         If memory allocation fails, the function returns 0.
 *
 * @details This function dynamically resizes the memory buffer to accommodate the incoming data.
 *          The function is called by libcurl.
 *          The received data is appended to the buffer, and the buffer is null-terminated to ensure it
 *          remains a valid C-string.
 */
size_t writeCallback(const void *data, size_t size, size_t nmemb, void *clientp);

/**
 * @brief Parses a JSON string to extract the value associated with a given key.
 *
 * @param[in] json Pointer to the JSON string to be parsed.
 * @param[in] key Pointer to the key whose associated value is to be retrieved.
 *
 * @return char* Pointer to the extracted string value. The caller is responsible for freeing the allocated memory.
 *         If the key is not found or the JSON is malformed, the function returns NULL.
 *
 * @details This function searches for the specified key within the JSON string, locates the associated value,
 *          and returns a dynamically allocated copy of that value. Special handling is implemented for the
 *          key `"content"` to correctly parse the JSON structure.
 *
 * @warning Needs to be used differently depending on the format of the json value's format. if it is formated like this:
 *  "key": "value" add the colon to the key
 *  "key":"value" do not add the colon to the key
 */
char *jsonParserGetStringValue(const char *json, char *key);

char *jsonParserGetIntValue(const char *json, char *key);

void freeChunkResponse();

void resetChunkResponse();
#endif
