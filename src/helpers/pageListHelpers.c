#include <string.h>
#include "ERTbot_common.h"

static size_t strlcpy(char *dst, const char *src, size_t dstsize);

pageList* addPageToList(pageList** head, const char *id, const char *title, const char *path, const char *description, const char *content, const char *updatedAt, const char *createdAt, const char *authorId) {
    log_message(LOG_DEBUG, "Entering function addPageToList");

    pageList* newNode = (pageList *)malloc(sizeof(pageList));
    if (!newNode) {
        log_message(LOG_ERROR, "Memory allocation error");
        exit(1);
    }

    // Allocate memory and copy the path and id
    newNode->id = NULL;
    newNode->title = NULL;
    newNode->path = NULL;
    newNode->description = NULL;
    newNode->content = NULL;
    newNode->updatedAt = NULL;
    newNode->createdAt = NULL;
    newNode->authorId = NULL;


    if(id!=NULL){
        size_t len = strlen(id) + 1;
        newNode->id = malloc(len);
        if(newNode->id){
            strlcpy(newNode->id, id, len);
        }
        else{
            log_message(LOG_ERROR, "Memory allocation failed for newNode->id");
        }
    }

    if(title!=NULL){
        size_t len = strlen(title) + 1;
        newNode->title = malloc(len);
        if(newNode->title){
            strlcpy(newNode->title, title, len);
        }
        else{
            log_message(LOG_ERROR, "Memory allocation failed for newNode->title");
        }
    }

    if(path!=NULL){
        size_t len = strlen(path) + 1;
        newNode->path = malloc(len);
        if(newNode->path){
            strlcpy(newNode->path, path, len);
        }
        else{
            log_message(LOG_ERROR, "Memory allocation failed for newNode->path");
        }
    }

    if(description!=NULL){
        size_t len = strlen(description) + 1;
        newNode->description = malloc(len);
        if(newNode->description){
            strlcpy(newNode->description, description, len);
        }
        else{
            log_message(LOG_ERROR, "Memory allocation failed for newNode->description");
        }
    }

    if(content!=NULL){
        size_t len = strlen(content) + 1;
        newNode->content = malloc(len);
        if(newNode->content){
            strlcpy(newNode->content, content, len);
        }
        else{
            log_message(LOG_ERROR, "Memory allocation failed for newNode->content");
        }
    }

    if(updatedAt!=NULL){
        size_t len = strlen(updatedAt) + 1;
        newNode->updatedAt = malloc(len);
        if(newNode->updatedAt){
            strlcpy(newNode->updatedAt, updatedAt, len);
        }
        else{
            log_message(LOG_ERROR, "Memory allocation failed for newNode->updatedAt");
        }
    }

    if(createdAt!=NULL){
        size_t len = strlen(createdAt) + 1;
        newNode->createdAt = malloc(len);
        if(newNode->createdAt){
            strlcpy(newNode->createdAt, createdAt, len);
        }
        else{
            log_message(LOG_ERROR, "Memory allocation failed for newNode->createdAt");
        }
    }

    if(authorId!=NULL){
        size_t len = strlen(authorId) + 1;
        newNode->authorId = malloc(len);
        if(newNode->authorId){
            strlcpy(newNode->authorId, authorId, len);
        }
        else{
            log_message(LOG_ERROR, "Memory allocation failed for newNode->authorId");
        }
    }

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

        if (temp->id && temp->id != NULL){
            log_message(LOG_DEBUG, "Freeing page struct variable id: %p", (void*)temp->id);
            free(temp->id);
        }
        if (temp->title && temp->title != NULL){
            log_message(LOG_DEBUG, "Freeing page struct variable title: %p", (void*)temp->title);
            free(temp->title);
        }
        if (temp->path && temp->path != NULL){
            log_message(LOG_DEBUG, "Freeing page struct variable path: %p", (void*)temp->path);
            free(temp->path);
        }
        if (temp->description && temp->description != NULL){
            log_message(LOG_DEBUG, "Freeing page struct variable description: %p", (void*)temp->description);
            free(temp->description);
        }
        if (temp->content && temp->content != NULL){
            log_message(LOG_DEBUG, "Freeing page struct variable content: %p", (void*)temp->content);
            free(temp->content);
        }
        if (temp->updatedAt && temp->updatedAt != NULL){
            log_message(LOG_DEBUG, "Freeing page struct variable updatedAt: %p", (void*)temp->updatedAt);
            free(temp->updatedAt);
        }
        if (temp->createdAt && temp->createdAt != NULL){
            log_message(LOG_DEBUG, "Freeing page struct variable createdAt: %p", (void*)temp->createdAt);
            free(temp->createdAt);
        }
        if (temp->authorId && temp->authorId != NULL){
            log_message(LOG_DEBUG, "Freeing page struct variable authorId: %p", (void*)temp->authorId);
            free(temp->authorId);
        }

        log_message(LOG_DEBUG, "about to free page struct");
        free(temp);
        log_message(LOG_DEBUG, "freed page struct");
    }


    log_message(LOG_DEBUG, "Exiting function freePageList");
}

static size_t strlcpy(char *dst, const char *src, size_t dstsize) {
    size_t src_len = 0;
    while (src[src_len] != '\0') src_len++;

    if (dstsize > 0) {
        size_t copy_len = (src_len >= dstsize) ? dstsize - 1 : src_len;
        for (size_t i = 0; i < copy_len; i++) {
            dst[i] = src[i];
        }
        dst[copy_len] = '\0';
    }

    return src_len;
}