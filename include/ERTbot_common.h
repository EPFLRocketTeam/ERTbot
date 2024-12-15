#ifndef ERTBOT_COMMON_H
#define ERTBOT_COMMON_H

#include <stdlib.h>
#include <time.h>


extern char *lastPageRefreshCheck;

/**
 * @struct memory
 * @brief Structure to store response data in memory.
 *
 * This structure is used to manage the storage of data retrieved during operations such as HTTP requests.
 * It holds the response data and the size of the allocated memory.
 *
 * @var memory::response
 * Pointer to a dynamically allocated block of memory where the response data is stored.
 *
 * @var memory::size
 * The size of the data stored in the `response` pointer.
 */
typedef struct memory {
    char *response;
    size_t size;
} memory;

/**
 * @var chunk
 * @brief An instance of the `memory` structure.
 *
 * The `chunk` variable is used to store the response data and its size when performing operations like HTTP requests.
 * It accumulates the data as the request progresses and can be accessed after the operation completes.
 */
extern memory chunk;

/**
 * @struct slackMessage
 * @brief Structure to store information about a Slack message.
 *
 * This structure holds details about a message sent via Slack, including the content of the message,
 * the sender's information, and the timestamp when the message was sent.
 *
 * @var slackMessage::message
 * Pointer to a string containing the content of the Slack message.
 *
 * @var slackMessage::sender
 * Pointer to a string containing the identifier or name of the sender of the message.
 *
 * @var slackMessage::timestamp
 * Pointer to a string representing the timestamp when the message was sent.
 */
typedef struct slackMessage {
  char *message;
  char *sender;
  char *timestamp;
}slackMessage;

extern slackMessage* commandStatusMessage;

/**
 * @struct pageList
 * @brief Structure to represent a linked list of pages, each containing metadata and content.
 *
 * This structure is used to store information about individual pages, including their unique identifiers,
 * titles, paths, descriptions, content, and timestamps for when they were created and last updated.
 *
 * @var pageList::id
 * Pointer to a string containing the unique identifier of the page.
 *
 * @var pageList::title
 * Pointer to a string containing the title of the page.
 *
 * @var pageList::path
 * Pointer to a string representing the path or location of the page.
 *
 * @var pageList::description
 * Pointer to a string providing a brief description of the page.
 *
 * @var pageList::content
 * Pointer to a string holding the actual content of the page.
 *
 * @var pageList::updatedAt
 * Pointer to a string representing the timestamp of the last update to the page.
 *
 * @var pageList::createdAt
 * Pointer to a string representing the timestamp when the page was created.
 *
 * @var pageList::next
 * Pointer to the next node in the linked list of pages.
 */
typedef struct pageList {
    char *id;
    char *title;
    char *path;
    char *description;
    char *content;
    char *updatedAt;
    char *createdAt;
    char *authorId;
    struct pageList *next;
}pageList;

/**
 * @var default_page
 * @brief Externally declared instance of the `pageList` structure representing the default page.
 */
extern pageList default_page;

/**
 * @struct command
 * @brief Represents a command with a function name and the argument.
 */
typedef struct command {
    char *function;
    char *argument;
    struct command *next;
}command;

extern command** headOfCommandQueue;

/**
 * @struct wikiFlag
 * @brief Represents a flag in the wiki system that contains a command, pointers, and a link to the next flag.
 *
 * @details
 * - `cmd`: A `command` structure representing the function and its associated arguments.
 * - `pointer_1`: A pointer to additional data or information related to the flag.
 * - `pointer_2`: Another pointer to additional data or information related to the flag.
 * - `next`: A pointer to the next `wikiFlag` in the list, enabling the creation of a linked list of flags.
 */
typedef struct wikiFlag {
    command cmd;
    char* pointerToEndOfFirstMarker;
    char* pointerToBeginningOfSecondMarker;
    struct wikiFlag* next;
}wikiFlag;

typedef struct ErrorState {
    int errorCode;                  // Error code
    char errorMessage[256];         // Error message
    const char *functionName;       // Name of the function where the error occurred
    struct ErrorState *next;        // Pointer to the next error (linked list)
} ErrorState;

void addError(int code, const char *message, const char *functionName);

ErrorState *getErrorList();

ErrorState *getLatestError();

void clearErrorList();

void removeLastError();

typedef enum {
    ERROR_UNKNOWN = 1000,
    ERROR_NULL_POINTER,
    ERROR_OUT_OF_MEMORY,
    ERROR_INVALID_ARG,
    ERROR_BUFFER_OVERFLOW,

    ERROR_FILE_NOT_FOUND = 2000,
    ERROR_FILE_READ,
    ERROR_FILE_WRITE,
    ERROR_FILE_PERMISSION,
    ERROR_FILE_CORRUPT
} ErrorCode;

#define LOG_DEBUG 0
#define LOG_INFO 1
#define LOG_ERROR 2

void log_message(int level, const char *func, const char *format, ...);

void log_function_entry(const char *func);
void log_function_exit(const char *func);

#endif
