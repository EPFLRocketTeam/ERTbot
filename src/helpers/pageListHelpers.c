#include <string.h>
#include "ERTbot_common.h"
#include "stringHelpers.h"

pageList* addPageToList(pageList** head, const char *id, const char *title, const char *path, const char *description, const char *content, const char *updatedAt) {
    log_function_entry(__func__);

    pageList* newNode = (pageList *)malloc(sizeof(pageList));
    if (!newNode) {
        log_message(LOG_ERROR, __func__, "Memory allocation error");
        exit(1);
    }

    newNode->id = NULL;
    newNode->title = NULL;
    newNode->path = NULL;
    newNode->description = NULL;
    newNode->content = NULL;
    newNode->updatedAt = NULL;
    newNode->createdAt = NULL;
    newNode->authorId = NULL;

    allocateAndCopy(&newNode->id, id, "newNode->id");
    allocateAndCopy(&newNode->title, title, "newNode->title");
    allocateAndCopy(&newNode->path, path, "newNode->path");
    allocateAndCopy(&newNode->description, description, "newNode->description");
    allocateAndCopy(&newNode->content, content, "newNode->content");
    allocateAndCopy(&newNode->updatedAt, updatedAt, "newNode->updatedAt");

    newNode->next = NULL; 

    if (*head == NULL) {
        *head = newNode;
        return *head;
    }

    pageList* lastNode = *head;
    while (lastNode->next != NULL) {
        lastNode = lastNode->next;
    }

    lastNode->next = newNode;

    log_function_exit(__func__);
    return *head;
}

void freePageList(pageList** head) {
    log_function_entry(__func__);

    while (*head) {
        pageList* temp = *head;
        *head = (*head)->next;

        if (temp->id && temp->id != NULL){
            log_message(LOG_DEBUG, __func__, "Freeing page struct variable id: %p", (void*)temp->id);
            free(temp->id);
        }
        if (temp->title && temp->title != NULL){
            log_message(LOG_DEBUG, __func__, "Freeing page struct variable title: %p", (void*)temp->title);
            free(temp->title);
        }
        if (temp->path && temp->path != NULL){
            log_message(LOG_DEBUG, __func__, "Freeing page struct variable path: %p", (void*)temp->path);
            free(temp->path);
        }
        if (temp->description && temp->description != NULL){
            log_message(LOG_DEBUG, __func__, "Freeing page struct variable description: %p", (void*)temp->description);
            free(temp->description);
        }
        if (temp->content && temp->content != NULL){
            log_message(LOG_DEBUG, __func__, "Freeing page struct variable content: %p", (void*)temp->content);
            free(temp->content);
        }
        if (temp->updatedAt && temp->updatedAt != NULL){
            log_message(LOG_DEBUG, __func__, "Freeing page struct variable updatedAt: %p", (void*)temp->updatedAt);
            free(temp->updatedAt);
        }
        if (temp->createdAt && temp->createdAt != NULL){
            log_message(LOG_DEBUG, __func__, "Freeing page struct variable createdAt: %p", (void*)temp->createdAt);
            free(temp->createdAt);
        }
        if (temp->authorId && temp->authorId != NULL){
            log_message(LOG_DEBUG, __func__, "Freeing page struct variable authorId: %p", (void*)temp->authorId);
            free(temp->authorId);
        }

        log_message(LOG_DEBUG, __func__, "about to free page struct");
        free(temp);
        log_message(LOG_DEBUG, __func__, "freed page struct");
    }


    log_function_exit(__func__);
}



