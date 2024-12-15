#ifndef ERTBOT_COMMANDS_H
#define ERTBOT_COMMANDS_H

#include "ERTbot_common.h"


command** checkForCommand(command** commandQueue);

command** executeCommand(command** commandQueue);

/**
 * @brief Parses a command sentence into a `command` structure.
 *
 * This function takes a sentence and breaks it down into individual words, then stores these words into the fields of
 * a `command` structure. The sentence is expected to contain a function name followed by up to nine arguments.
 *
 * @param sentence The input string containing the command sentence to be parsed.
 * @param cmd A pointer to a `command` structure where the parsed command and arguments will be stored.
 *
 * @details The function creates a copy of the input sentence to avoid modifying the original string. It then tokenizes the
 *          sentence using spaces as delimiters and stores each token in the appropriate field of the `command` structure.
 *          It handles up to ten words (one function and nine arguments). If the sentence contains more than ten words,
 *          an error message is printed. All allocated strings in the `command` structure are dynamically allocated and
 *          should be freed by the caller when no longer needed.
 *
 * @note The `MAX_ARGUMENTS` constant should define the maximum number of arguments allowed (including the function name).
 *       The `command` structure should have fields for storing the function and up to nine arguments.
 */
void breakdownCommand(const char* sentence, command* cmd);

#endif
