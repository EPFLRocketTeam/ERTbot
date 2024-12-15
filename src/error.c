#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ERTbot_common.h"

static ErrorState *errorListHead = NULL;

void addError(int code, const char *message, const char *functionName) {
    // Allocate memory for the new error
    ErrorState *newError = malloc(sizeof(ErrorState));
    if (newError == NULL) {
        fprintf(stderr, "Failed to allocate memory for error state.\n");
        return;
    }

    // Initialize the new error
    newError->errorCode = code;
    
    // Handle empty message and NULL functionName
    if (message == NULL || strlen(message) == 0) {
        strncpy(newError->errorMessage, "Unknown error", sizeof(newError->errorMessage) - 1);
    } else {
        strncpy(newError->errorMessage, message, sizeof(newError->errorMessage) - 1);
    }
    newError->errorMessage[sizeof(newError->errorMessage) - 1] = '\0'; // Ensure null-termination

    // Handle NULL function name
    if (functionName == NULL) {
        newError->functionName = "Unknown";
    } else {
        newError->functionName = functionName;
    }

    newError->next = NULL;

    // If the list is empty, set the new error as the head
    if (errorListHead == NULL) {
        errorListHead = newError;
    } else {
        // Traverse the list to find the last node
        ErrorState *current = errorListHead;
        while (current->next != NULL) {
            current = current->next;
        }
        // Append the new error at the end
        current->next = newError;
    }
}



ErrorState *getErrorList() {
    return errorListHead;
}

ErrorState *getLatestError() {
    ErrorState *current = getErrorList();

    if (current == NULL) {
        return NULL; // No errors in the list
    }

    // Traverse the linked list to find the last node
    while (current->next != NULL) {
        current = current->next;
    }

    return current; // Return the latest error
}

void clearErrorList() {
    ErrorState *current = errorListHead;
    while (current) {
        ErrorState *next = current->next;
        free(current);
        current = next;
    }
    errorListHead = NULL;
}

void removeLastError() {
    if (errorListHead == NULL) {
        // No errors in the list, nothing to remove
        return;
    }

    // If there's only one error in the list, remove it
    if (errorListHead->next == NULL) {
        free(errorListHead); // Free the only error in the list
        errorListHead = NULL; // Set the list head to NULL
        return;
    }

    // Traverse the list to find the second-to-last node
    ErrorState *current = errorListHead;
    while (current->next != NULL && current->next->next != NULL) {
        current = current->next;
    }

    // Remove the last node and free its memory
    free(current->next);
    current->next = NULL; // Set the second-to-last node's next to NULL
}
