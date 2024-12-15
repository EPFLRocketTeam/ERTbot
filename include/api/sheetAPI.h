#ifndef ERTBOT_SHEETAPI_H
#define ERTBOT_SHEETAPI_H

/**
 * @brief Makes an API request to a specified URL using the provided query and request type.
 *
 * @param[in] query Pointer to the query string to be sent in the request body.
 * @param[in] url Pointer to the URL to which the request is made.
 * @param[in] requestType Pointer to the request type (e.g., "PUT", "POST").
 *
 * @details This function initializes a cURL session to perform an HTTP request to a given URL.
 *          It sets the necessary HTTP headers, including an authorization token, and sends the
 *          query data as the request body. The response from the server is handled by the
 *          `writeCallback` function, which stores the response in a memory buffer. The cURL
 *          session and associated resources are cleaned up after the request is completed.
 *
 * @note The function assumes that a global variable `chunk` of type `struct memory` is defined and
 *       used to store the server's response. The authorization token `SHEET_API_TOKEN` must be
 *       defined as a macro or global variable elsewhere in the program.
 */
void sheetApi(char *query, char *url, char *requestType);

/**
 * @brief Performs a batch update on a Google Sheet by sending a POST request with specified parameters.
 *
 * @param[in] sheetId Pointer to the string containing the ID of the Google Sheet to be updated.
 * @param[in] range Pointer to the string specifying the range of cells to update.
 * @param[in] values Pointer to the string containing the values to be inserted into the specified range.
 *
 * @details This function constructs a URL and query string for a batch update request to the Google Sheets API.
 *          The URL and query template strings are modified to include the provided `sheetId`, `range`, and `values`.
 *          The modified URL and query are then passed to the `sheetAPI` function, which handles the API request.
 *          After the request is completed, the dynamically allocated memory for the modified URL and query strings
 *          is freed.
 *
 * @note The function assumes the existence of global template strings `template_batch_update_url` and
 *       `template_batch_update_query`, as well as the utility function `replaceWord` for string substitution.
 */
void batchUpdateSheet(const char *sheetId, const char *range, const char *values);

/**
 * @brief Retrieves data from a specified range in a Google Sheet by sending a GET request.
 *
 * @param[in] sheetId Pointer to the string containing the ID of the Google Sheet to be accessed.
 * @param[in] range Pointer to the string specifying the range of cells to retrieve.
 *
 * @details This function constructs a URL for a GET request to the Google Sheets API by modifying a template URL
 *          to include the provided `sheetId` and `range`. The constructed URL is then passed to the `sheetAPI`
 *          function, which handles the API request. Since this is a GET request, the query string is left empty.
 *          After the request is completed, the dynamically allocated memory for the modified URL is freed.
 *
 * @note The function assumes the existence of a global template string `template_batch_get_url`, as well as the
 *       utility function `replaceWord` for string substitution.
 */
void batchGetSheet(const char *sheetId, const char *range);

/**
 * @brief Refreshes the OAuth token by sending a POST request to the Google OAuth 2.0 server.
 */
void refreshOAuthToken();

#endif
