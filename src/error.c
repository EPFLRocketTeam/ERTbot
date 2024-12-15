#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ERTbot_common.h"

static ErrorState *errorListHead = NULL;

void addError(int code, const char *message, const char *functionName) {
    ErrorState *newError = malloc(sizeof(ErrorState));
    if (newError == NULL) {
        fprintf(stderr, "Failed to allocate memory for error state.\n");
        return;
    }

    newError->errorCode = code;
    
    if (message == NULL || strlen(message) == 0) {
        strncpy(newError->errorMessage, "Unknown error", sizeof(newError->errorMessage) - 1);
    } else {
        strncpy(newError->errorMessage, message, sizeof(newError->errorMessage) - 1);
    }
    newError->errorMessage[sizeof(newError->errorMessage) - 1] = '\0';

    if (functionName == NULL) {
        newError->functionName = "Unknown";
    } else {
        newError->functionName = functionName;
    }

    newError->next = NULL;

    if (errorListHead == NULL) {
        errorListHead = newError;
    } else {
        ErrorState *current = errorListHead;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newError;
    }
}



ErrorState *getErrorList() {
    return errorListHead;
}

ErrorState *getLatestError() {
    ErrorState *current = getErrorList();

    if (current == NULL) {
        return NULL;
    }

    while (current->next != NULL) {
        current = current->next;
    }

    return current;
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
        return;
    }

    if (errorListHead->next == NULL) {
        free(errorListHead);
        errorListHead = NULL;
        return;
    }

    ErrorState *current = errorListHead;
    while (current->next != NULL && current->next->next != NULL) {
        current = current->next;
    }

    free(current->next);
    current->next = NULL;
}
