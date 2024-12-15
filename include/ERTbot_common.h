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
 *
 * @details
 * The `memory` structure is typically used in scenarios where data needs to be accumulated in memory,
 * such as when making HTTP requests and storing the response for further processing.
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
 *
 * @details
 * The `slackMessage` structure is useful for storing and processing messages retrieved from Slack
 * in applications that interact with the Slack API or manage Slack communications.
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
 *
 * @details
 * The `pageList` structure allows for the creation of a linked list where each node contains all the necessary
 * information about a page. This can be used in applications that need to manage or display a collection of pages,
 * such as a content management system or documentation tool.
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
 *
 * This variable is an instance of the `pageList` structure and is declared externally.
 * It is intended to represent a default page with pre-defined or fallback values in the context of the application.
 *
 * @details
 * The `default_page` instance can be used as a template or fallback when no other page is available or
 * when initializing the system with a base set of page properties.
 * The actual definition of `default_page` is expected to be in another source file, making it accessible across multiple files.
 */
extern pageList default_page;

/**
 * @struct command
 * @brief Represents a command with a function name and up to nine arguments.
 *
 * The `command` structure is used to encapsulate a function name along with a variable number of arguments (up to nine).
 * This structure can be utilized to represent a command that is to be executed, with the arguments specifying the
 * parameters needed by the function.
 *
 * @details
 * - `function`: A string representing the name of the function to be executed.
 * - `argument` to `argument_9`: Strings representing the arguments that are passed to the function.
 *   These fields are optional and may be left NULL if not used.
 * - `next`: A pointer to the next command in a sequence, allowing commands to be chained together.
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
 * The `wikiFlag` structure is used to manage flags within the wiki system. Each flag contains a command to be executed,
 * two additional pointers for auxiliary data, and a link to the next flag in the sequence.
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


typedef struct PeriodicCommand {
    struct command* command;      // Command to be executed
    int period;         // Period in seconds
    time_t next_time;   // Next time to execute the command
    struct PeriodicCommand* next;  // Next periodic command in the list
} PeriodicCommand;

extern PeriodicCommand** headOfPeriodicCommands;

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

void log_message(int level, const char *format, ...);

#endif
