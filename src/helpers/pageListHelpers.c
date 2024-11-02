#include <string.h>
#include "ERTbot_common.h"

pageList* addPageToList(pageList** head, const char *id, const char *title, const char *path, const char *description, const char *content, const char *updatedAt, const char *createdAt, const char *authorId) {
    log_message(LOG_DEBUG, "Entering function addPageToList");

    pageList* newNode = (pageList *)malloc(sizeof(pageList));
    if (!newNode) {
        log_message(LOG_ERROR, "Memory allocation error");
        exit(1);
    }

    // Allocate memory and copy the path and id
    newNode->id = malloc(strlen(id) + 1);
    strcpy(newNode->id, id);

    newNode->title = malloc(strlen(title) + 1);
    strcpy(newNode->title, title);

    newNode->path = malloc(strlen(path) + 1);
    strcpy(newNode->path, path);

    newNode->description = malloc(strlen(description) + 1);
    strcpy(newNode->description, description);

    newNode->content = malloc(strlen(content) + 1);
    strcpy(newNode->content, content);

    newNode->updatedAt = malloc(strlen(updatedAt) + 1);
    strcpy(newNode->updatedAt, updatedAt);

    newNode->createdAt = malloc(strlen(createdAt) + 1);
    strcpy(newNode->createdAt, createdAt);

    newNode->authorId = malloc(strlen(authorId) + 1);
    strcpy(newNode->authorId, authorId);

    newNode->next = NULL;  // New node will be the last node

    // If the list is empty, make the new node the first node
    if (*head == NULL) {
        *head = newNode;
        return *head;
    }

    // Traverse the list to find the last node
    pageList* lastNode = *head;
    while (lastNode->next != NULL) {
        lastNode = lastNode->next;
    }

    // Link the new node after the last node
    lastNode->next = newNode;


    log_message(LOG_DEBUG, "Exiting function addPageToList");
    return *head;
}

void freePageList(pageList** head) {
    log_message(LOG_DEBUG, "Entering function freePageList");

    while (*head) {
        pageList* temp = *head;
        *head = (*head)->next;

        // Debugging prints
        if (temp->id){
            log_message(LOG_DEBUG, "Freeing page struct variable id: %p", (void*)temp->id);
            free(temp->id);
        }
        if (temp->title){
            log_message(LOG_DEBUG, "Freeing page struct variable title: %p", (void*)temp->title);
            free(temp->title);
        }
        if (temp->path){
            log_message(LOG_DEBUG, "Freeing page struct variable path: %p", (void*)temp->path);
            free(temp->path);
        }
        if (temp->description){
            log_message(LOG_DEBUG, "Freeing page struct variable description: %p", (void*)temp->description);
            free(temp->description);
        }
        if (temp->content){
            log_message(LOG_DEBUG, "Freeing page struct variable content: %p", (void*)temp->content);
            free(temp->content);
        }
        if (temp->updatedAt){
            log_message(LOG_DEBUG, "Freeing page struct variable updatedAt: %p", (void*)temp->updatedAt);
            free(temp->updatedAt);
        }
        if (temp->createdAt){
            log_message(LOG_DEBUG, "Freeing page struct variable createdAt: %p", (void*)temp->createdAt);
            free(temp->createdAt);
        }
        if (temp->authorId){
            log_message(LOG_DEBUG, "Freeing page struct variable authorId: %p", (void*)temp->authorId);
            free(temp->authorId);
        }

        log_message(LOG_DEBUG, "about to free page struct");
        free(temp);
        log_message(LOG_DEBUG, "freed page struct");
    }


    log_message(LOG_DEBUG, "Exiting function freePageList");
}
