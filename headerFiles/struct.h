#ifndef STRUCT_H
#define STRUCT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <cjson/cJSON.h>

//Used to store the information received when performing a http request (performing an API)
typedef struct memory {
    char *response;
    size_t size;
}memory;
extern memory chunk;

//Store information about a slack message
typedef struct slackMessage {
  char *message;
  char *sender;
  char *timestamp;
}slackMessage;

//Linked list for information about wiki pages
typedef struct pageList {
    char *id;
    char *title;
    char *path;
    char *description;
    char *content;
    char *updatedAt;
    char *createdAt;
    struct pageList *next;
}pageList;
extern pageList default_page;

typedef struct command {
    char *function;
    char *argument_1;
    char *argument_2;
    char *argument_3;
    char *argument_4;
    char *argument_5;
    char *argument_6;
    char *argument_7;
    char *argument_8;
    char *argument_9;
}command;

typedef struct wikiFlag {
    command cmd;
    char* pointer_1;
    char* pointer_2;
    struct wikiFlag *next;
}wikiFlag;



#endif