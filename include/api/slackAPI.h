#ifndef ERTBOT_SLACKAPI_H
#define ERTBOT_SLACKAPI_H

#include "ERTbot_common.h"


/**
 * @brief Sends a message to a specified Slack channel using the Slack API.
 * 
 * @param[in] message Pointer to the string containing the message to be sent.
 * 
 * @return int Returns 0 on success and 1 on failure.
 * 
 * @details This function sends a POST request to the Slack API to post a message to a specific channel.
 *          The message is formatted as a JSON payload and includes the channel ID and text of the message.
 *          The function sets up the necessary HTTP headers, including the authorization token, and sends the request 
 *          using libcurl. If the request fails or if the libcurl initialization fails, the function returns 1.
 *          On success, it returns 0.
 * 
 * @note The function assumes that the environment variable `SLACK_API_TOKEN` is set with the Slack API token, and that
 *       `SLACK_WIKI_TOOLBOX_CHANNEL` contains the channel ID. The maximum message length is defined by `MAX_MESSAGE_LENGTH`.
 */
int sendMessageToSlack(char *message);

/**
 * @brief Checks the last message sent in a specified Slack channel by fetching the conversation history.
 * 
 * @details This function sends a GET request to the Slack API to retrieve the most recent message from a specific 
 *          Slack channel. The request URL is constructed using the channel ID and a limit of 1 message. The response 
 *          is handled by the `writeCallback` function, which stores the result in a memory buffer. The function also 
 *          checks for any errors in the HTTP request and verifies the HTTP status code. Resources are cleaned up 
 *          after the request is completed.
 * 
 * @note The function assumes that the environment variables `SLACK_API_TOKEN` and `SLACK_WIKI_TOOLBOX_CHANNEL` 
 *       are set with the appropriate values. The function uses a global variable `chunk` of type `struct memory` 
 *       to store the response data. 
 */
void checkLastSlackMessage();

/**
 * @brief Retrieves the most recent message from a Slack channel and populates a `slackMessage` struct with its details.
 * 
 * @param[in] slackMsg A `slackMessage` struct that will be populated with the message details.
 * 
 * @return slackMessage The populated `slackMessage` struct containing the message text, sender ID, and timestamp.
 * 
 * @details This function calls `checkLastSlackMessage()` to retrieve the latest message from a Slack channel.
 *          It then parses the JSON response to extract the message text, sender ID, and timestamp using the 
 *          `jsonParserGetStringValue()` function. These values are assigned to the corresponding fields of the 
 *          `slackMessage` struct passed to the function. The response memory buffer is freed after parsing.
 * 
 * @note The function assumes that `chunk.response` contains the JSON response from Slack, and that the `slackMessage` 
 *       struct has fields for `message`, `sender`, and `timestamp` that are of type `char*`.
 */
slackMessage* getSlackMessage(slackMessage* slackMsg);

/**
 * @brief Invites a user to one or more Slack channels using the Slack API.
 * 
 * @param[in] channels A comma-separated list of Slack channel IDs to which the user will be invited.
 * @param[in] email The email address of the user to invite.
 * 
 * @return int Returns 0 on success and 1 on failure.
 * 
 * @details This function sends a POST request to the Slack API to invite a user to specified channels. The request
 *          payload is formatted as JSON and includes the channel IDs, user email, and team ID. The function handles
 *          setting the required HTTP headers, including the authorization token. If the request fails or if libcurl
 *          initialization fails, the function returns 1. On success, it returns 0.
 * 
 * @note The function assumes that the environment variable `SLACK_API_TOKEN` is set with the Slack API token, and that
 *       `SLACK_API_TOKEN` is valid. The maximum JSON payload length is defined by `2000` bytes.
 * 
 * @warning This function has not been tested yet and is not integrated into the program yet
 * 
 * @todo Test the addSlackMember function and integrate with the rest of the program
 */
int addSlackMember(char *channels, char *email);
#endif