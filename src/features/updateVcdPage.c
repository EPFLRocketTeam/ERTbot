#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <cjson/cJSON.h>
#include "sheetAPI.h"
#include "ERTbot_common.h"
#include "ERTbot_config.h"
#include "requirementsHelpers.h"
#include "stringHelpers.h"
#include "wikiAPI.h"
#include "pageListHelpers.h"

#define VCD_TITLE_TEMPLATE "# Verification Statuses per Deadline\n"
#define DEADLINE_SUBSECTION_TITLE_TEMPLATE "\n## $Deadline Name$"
#define STATUS_SUBSUBSECTION_TITLE_TEMPLATE "\n### $Status Name$"
#define METHOD_SUBSUBSUBSECTION_TITLE_TEMPLATE "\n#### $Method Name$"

#define VCD_TABSET_TITLE_TEMPLATE "\n\n\n## $ID$\n"
#define VCD_ID_BLOCK_TEMPLATE "- [$ID$](/"
#define VCD_PAGE_DIRECTORY "$Requirement Pages Directory$"
#define VCD_PAGE_NAME "$ID$) **"
#define VCD_TITLE_BLOCK_TEMPLATE "$Title$**\n"


static cJSON* parseVerificationInformation(cJSON* requirements);

static char* buildVCD(cJSON* verificationInformation, cJSON* requirements, cJSON* subsystem);

static int parseDeadlineBlock(cJSON* verificationInformation, char* pageContent, char* deadlineItemName, cJSON* requirements, cJSON* subsystem);

static int parseStatusBlock(char* pageContent, cJSON* deadlineObject, char* statusName, cJSON* requirements, cJSON* subsystem);

static int parseMethodBlock(char* pageContent, cJSON* statusObject, char* methodName, cJSON* requirements, cJSON* subsystem);

static int parseRequirementBlock(char* pageContent, cJSON* requirement, cJSON* subsystem);

static int getStatusCount(const cJSON* deadlineObject, const char* statusName);

static int appendVcdPieChart(char* pageContent, const cJSON* deadlineObject);

static cJSON* getDeadlineObject(cJSON* verificationInformation, char* deadlineName);

static cJSON* addDeadlineObject(cJSON* verificationInformation, char* deadlineName);

static cJSON* getMethodArray(cJSON* statusObject, char* methodName);

static cJSON* getStatusArray(cJSON* deadlineObject, char* statusName);

static bool verificationMethodAlreadyExists(const cJSON* deadlineObject, const char* verificationMethod);

static bool verificationDeadlineEmpty(const cJSON* requirement, const char* JsonItemNameDeadline);

static cJSON* parseVerificationInformation(cJSON* requirements);


void updateVcdPage(command cmd){
    log_message(LOG_DEBUG, "Entering function updateVcdPage");

    cJSON* subsystem = getSubsystemInfo(cmd.argument);
    const char *vcdPageId = cJSON_GetObjectItem(subsystem, "VCD Page ID")->valuestring;
    cJSON *requirementList = getRequirements(subsystem);
    cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");

    cJSON* verificationInformation = parseVerificationInformation(requirements);

    char* pageContent = buildVCD(verificationInformation, requirements, subsystem);

    pageList* vcdPage = NULL;
    vcdPage = addPageToList(&vcdPage, vcdPageId, NULL, NULL, NULL, pageContent, NULL);

    vcdPage->content = replaceWord_Realloc(vcdPage->content, "\n", "\\\\n");
    vcdPage->content = replaceWord_Realloc(vcdPage->content, "\"", "\\\\\\\"");

    updatePageContentMutation(vcdPage);
    renderMutation(&vcdPage, false);
    freePageList(&vcdPage);

    cJSON_Delete(requirementList);
    log_message(LOG_DEBUG, "Exiting function updateVcdPage");
    return;
}

static char* buildVCD(cJSON* verificationInformation, cJSON* requirements, cJSON* subsystem){

    int amountOfDifferentDeadlines = cJSON_GetArraySize(verificationInformation);
    char* pageContent = duplicate_Malloc(VCD_TITLE_TEMPLATE);

    for(int i = 1; i <= amountOfDifferentDeadlines; i++){

        char deadlineItemName[50];
        snprintf(deadlineItemName, sizeof(deadlineItemName), "Deadline %d", i);

        (void)parseDeadlineBlock(verificationInformation, pageContent, deadlineItemName, requirements, subsystem);
    }

    return pageContent;
}

static int parseDeadlineBlock(cJSON* verificationInformation, char* pageContent, char* deadlineItemName, cJSON* requirements, cJSON* subsystem){
    if(!cJSON_HasObjectItem(verificationInformation, deadlineItemName)){
        return 0;
    }

    cJSON* deadlineObject = cJSON_GetObjectItem(verificationInformation, deadlineItemName);
    (void)addSectionToPageContent(&pageContent, DEADLINE_SUBSECTION_TITLE_TEMPLATE, deadlineObject, "Deadline Name");
    (void)appendVcdPieChart(pageContent, deadlineObject);
    (void)parseStatusBlock(pageContent, deadlineObject, "uncompleted", requirements, subsystem);
    (void)parseStatusBlock(pageContent, deadlineObject, "in progress", requirements, subsystem);
    (void)parseStatusBlock(pageContent, deadlineObject, "completed", requirements, subsystem);

    return 1;
}

static int parseStatusBlock(char* pageContent, cJSON* deadlineObject, char* statusName, cJSON* requirements, cJSON* subsystem){
    if(!cJSON_HasObjectItem(deadlineObject, statusName)){
        return 0;
    }

    cJSON* statusObject = cJSON_GetObjectItem(deadlineObject, statusName);
    pageContent = appendToString(pageContent, STATUS_SUBSUBSECTION_TITLE_TEMPLATE);
    pageContent = replaceWord_Realloc(pageContent, "$Status Name$", statusName);
    (void)parseMethodBlock(pageContent, statusObject, "Test", requirements, subsystem);
    (void)parseMethodBlock(pageContent, statusObject, "Review Of Design", requirements, subsystem);
    (void)parseMethodBlock(pageContent, statusObject, "Inspection", requirements, subsystem);
    (void)parseMethodBlock(pageContent, statusObject, "Analysis", requirements, subsystem);

    return 1;
}

static int parseMethodBlock(char* pageContent, cJSON* statusObject, char* methodName, cJSON* requirements, cJSON* subsystem){
    if(!cJSON_HasObjectItem(statusObject, methodName)){
        return 0;
    }

    cJSON* methodArray = cJSON_GetObjectItem(statusObject, methodName);
    pageContent = appendToString(pageContent, STATUS_SUBSUBSECTION_TITLE_TEMPLATE);
    pageContent = replaceWord_Realloc(pageContent, "$Method Name$", methodName);

    int methodArraySize = cJSON_GetArraySize(methodArray);
    int requirementsArraySize = cJSON_GetArraySize(requirements);


    for(int j = 0; j < methodArraySize; j++){
        char* requirementId = cJSON_GetArrayItem(methodArray, j)->valuestring;
        for(int k = 0; k < requirementsArraySize; k++){
            cJSON* requirement = cJSON_GetArrayItem(requirements, j);

            if(cJSON_HasObjectItem(requirement, "ID")
                && cJSON_IsString(cJSON_GetObjectItem(requirement, "ID"))
                && strcmp(cJSON_GetObjectItem(requirement, "ID")->valuestring, requirementId) == 0){

                (void)parseRequirementBlock(pageContent, requirement, subsystem);
            }
        }
    }

    pageContent = appendToString(pageContent, "{.links-list}");

    return 1;
}

static int parseRequirementBlock(char* pageContent, cJSON* requirement, cJSON* subsystem){
    (void)addSectionToPageContent(&pageContent, VCD_ID_BLOCK_TEMPLATE, requirement, "ID");
    (void)addSectionToPageContent(&pageContent, VCD_PAGE_DIRECTORY, subsystem, "Requirement Pages Directory");
    (void)addSectionToPageContent(&pageContent, VCD_PAGE_NAME, requirement, "ID");
    int hasTitle = addSectionToPageContent(&pageContent, VCD_TITLE_BLOCK_TEMPLATE, requirement, "Title");

    return hasTitle;
}

static int getStatusCount(const cJSON* deadlineObject, const char* statusName){
    const cJSON* statusObject = cJSON_GetObjectItem(deadlineObject, statusName);
    return cJSON_GetArraySize(statusObject);
}

static int appendVcdPieChart(char* pageContent, const cJSON* deadlineObject){
    log_message(LOG_DEBUG, "Entering function createVcdPieChart");

    char *pieChart = "\n```kroki\nvega\n\n{\n  \"$schema\": \"https://vega.github.io/schema/vega/v5.0.json\",\n  \"width\": 350,\n  \"height\": 350,\n  \"autosize\": \"pad\",\n  \"signals\": [\n    {\"name\": \"startAngle\", \"value\": 0},\n    {\"name\": \"endAngle\", \"value\": 6.29},\n    {\"name\": \"padAngle\", \"value\": 0},\n    {\"name\": \"sort\", \"value\": true},\n    {\"name\": \"strokeWidth\", \"value\": 2},\n    {\n      \"name\": \"selected\",\n      \"value\": \"\",\n      \"on\": [{\"events\": \"mouseover\", \"update\": \"datum\"}]\n    }\n  ],\n  \"data\": [\n    {\n      \"name\": \"table\",\n      \"values\": [\n        {\"continent\": \"Unverified\", \"population\": DefaultUnverifiedPopulation},\n        {\"continent\": \"Partially Verified\", \"population\": DefaultPartiallyVerifiedPopulation},\n        {\"continent\": \"Verified\", \"population\": DefaultVerifiedPopulation}\n      ],\n      \"transform\": [\n        {\n          \"type\": \"pie\",\n          \"field\": \"population\",\n          \"startAngle\": {\"signal\": \"startAngle\"},\n          \"endAngle\": {\"signal\": \"endAngle\"},\n          \"sort\": {\"signal\": \"sort\"}\n        }\n      ]\n    },\n    {\n      \"name\": \"fieldSum\",\n      \"source\": \"table\",\n      \"transform\": [\n        {\n          \"type\": \"aggregate\",\n          \"fields\": [\"population\"],\n          \"ops\": [\"sum\"],\n          \"as\": [\"sum\"]\n        }\n      ]\n    }\n  ],\n  \"legends\": [\n    {\n      \"fill\": \"color\",\n      \"title\": \"Legends\",\n      \"orient\": \"none\",\n      \"padding\": {\"value\": 10},\n      \"encode\": {\n        \"symbols\": {\"enter\": {\"fillOpacity\": {\"value\": 1}}},\n        \"legend\": {\n          \"update\": {\n            \"x\": {\n              \"signal\": \"(width / 2) + if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.1 * 0.8, if(width >= height, height, width) / 2 * 0.8)\",\n              \"offset\": 20\n            },\n            \"y\": {\"signal\": \"(height / 2)\", \"offset\": -50}\n          }\n        }\n      }\n    }\n  ],\n  \"scales\": [\n    {\"name\": \"color\", \"type\": \"ordinal\", \"range\": [\"#cf2608\", \"#ff9900\", \"#67b505\"]}\n  ],\n  \"marks\": [\n    {\n      \"type\": \"arc\",\n      \"from\": {\"data\": \"table\"},\n      \"encode\": {\n        \"enter\": {\n          \"fill\": {\"scale\": \"color\", \"field\": \"continent\"},\n          \"x\": {\"signal\": \"width / 2\"},\n          \"y\": {\"signal\": \"height / 2\"}\n        },\n        \"update\": {\n          \"startAngle\": {\"field\": \"startAngle\"},\n          \"endAngle\": {\"field\": \"endAngle\"},\n          \"cornerRadius\": {\"value\": 15},\n          \"padAngle\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, 0.015, 0.015)\"\n          },\n          \"innerRadius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 0.45, if(width >= height, height, width) / 2 * 0.5)\"\n          },\n          \"outerRadius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.05 * 0.8, if(width >= height, height, width) / 2 * 0.8)\"\n          },\n          \"opacity\": {\n            \"signal\": \"if(selected && selected.continent !== datum.continent, 1, 1)\"\n          },\n          \"stroke\": {\"signal\": \"scale('color', datum.continent)\"},\n          \"strokeWidth\": {\"signal\": \"strokeWidth\"},\n          \"fillOpacity\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, 0.8, 0.8)\"\n          }\n        }\n      }\n    },\n    {\n      \"type\": \"text\",\n      \"encode\": {\n        \"enter\": {\"fill\": {\"value\": \"#525252\"}, \"text\": {\"value\": \"\"}},\n        \"update\": {\n          \"opacity\": {\"value\": 1},\n          \"x\": {\"signal\": \"width / 2\"},\n          \"y\": {\"signal\": \"height / 2\"},\n          \"align\": {\"value\": \"center\"},\n          \"baseline\": {\"value\": \"middle\"},\n          \"fontSize\": {\"signal\": \"if(width >= height, height, width) * 0.05\"},\n          \"text\": {\"value\": \"Verification Status\"}\n        }\n      }\n    },\n    {\n      \"name\": \"mark_population\",\n      \"type\": \"text\",\n      \"from\": {\"data\": \"table\"},\n      \"encode\": {\n        \"enter\": {\n          \"text\": {\n            \"signal\": \"if(datum['endAngle'] - datum['startAngle'] < 0.3, '', format(datum['population'] / 1, '.0f'))\"\n          },\n          \"x\": {\"signal\": \"if(width >= height, height, width) / 2\"},\n          \"y\": {\"signal\": \"if(width >= height, height, width) / 2\"},\n          \"radius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.05 * 0.65, if(width >= height, height, width) / 2 * 0.65)\"\n          },\n          \"theta\": {\"signal\": \"(datum['startAngle'] + datum['endAngle'])/2\"},\n          \"fill\": {\"value\": \"#FFFFFF\"},\n          \"fontSize\": {\"value\": 12},\n          \"align\": {\"value\": \"center\"},\n          \"baseline\": {\"value\": \"middle\"}\n        }\n      }\n    }\n  ]\n}\n\n```\n## {.tabset}\n";

    char unverifiedPopulation[10];
    sprintf(unverifiedPopulation, "%d", getStatusCount(deadlineObject, "uncompleted"));

    char partiallyVerifiedPopulation[10];
    sprintf(partiallyVerifiedPopulation, "%d", getStatusCount(deadlineObject, "in progress"));

    char verifiedPopulation[10];
    sprintf(verifiedPopulation, "%d", getStatusCount(deadlineObject, "completed"));

    pageContent = appendToString(pageContent, pieChart);
    pageContent = replaceWord_Realloc(pageContent, "DefaultUnverifiedPopulation", unverifiedPopulation);
    pageContent = replaceWord_Realloc(pageContent, "DefaultPartiallyVerifiedPopulation", partiallyVerifiedPopulation);
    pageContent = replaceWord_Realloc(pageContent, "DefaultVerifiedPopulation", verifiedPopulation);

    log_message(LOG_DEBUG, "Exiting function createVcdPieChart");

    return 1;
}

static cJSON* getDeadlineObject(cJSON* verificationInformation, char* deadlineName){
    int arraySize = cJSON_GetArraySize(verificationInformation);

    if(arraySize == 0){
        return addDeadlineObject(verificationInformation, deadlineName);
    }

    for(int i = 0; i < arraySize; i++){
        char JsonItemNameDeadline[50];
        snprintf(JsonItemNameDeadline, sizeof(JsonItemNameDeadline), "Deadline %d", i);

        cJSON* deadlineObject = cJSON_GetObjectItem(verificationInformation, JsonItemNameDeadline);

        if(strcmp(cJSON_GetObjectItem(deadlineObject, "Deadline Name")->valuestring, deadlineName) == 0){
            return deadlineObject;
        }
    }

    return addDeadlineObject(verificationInformation, deadlineName);
}

static cJSON* addDeadlineObject(cJSON* verificationInformation, char* deadlineName){
    int arraySize = cJSON_GetArraySize(verificationInformation);
    int deadlineNumber = arraySize + 1;

    char JsonItemNameDeadline[50];
    snprintf(JsonItemNameDeadline, sizeof(JsonItemNameDeadline), "Deadline %d", deadlineNumber);

    cJSON* deadlineNameItem = cJSON_CreateString(deadlineName);
    cJSON* deadlineObject = cJSON_CreateObject();

    cJSON_AddItemToObject(verificationInformation, JsonItemNameDeadline, deadlineObject);
    cJSON_AddItemToObject(deadlineObject, "Deadline Name", deadlineNameItem);

    return deadlineObject;
}

static cJSON* getMethodArray(cJSON* statusObject, char* methodName){

    if(cJSON_HasObjectItem(statusObject, methodName)){
        return cJSON_GetObjectItem(statusObject, methodName);
    }

    cJSON* methodeArray = cJSON_CreateArray();
    cJSON_AddItemToObject(statusObject, methodName, methodeArray);

   return cJSON_GetObjectItem(statusObject, methodName);
}

static cJSON* getStatusArray(cJSON* deadlineObject, char* statusName){

    if(cJSON_HasObjectItem(deadlineObject, statusName)){
        return cJSON_GetObjectItem(deadlineObject, statusName);
    }

    cJSON* statusArray = cJSON_CreateArray();
    cJSON_AddItemToObject(deadlineObject, statusName, statusArray);

   return cJSON_GetObjectItem(deadlineObject, statusName);
}

static bool verificationMethodAlreadyExists(const cJSON* deadlineObject, const char* verificationMethod){
    return cJSON_HasObjectItem(deadlineObject, verificationMethod);
}


static bool verificationDeadlineEmpty(const cJSON* requirement, const char* JsonItemNameDeadline){
    if(!cJSON_HasObjectItem(requirement, JsonItemNameDeadline)
        || strcmp(cJSON_GetObjectItem(requirement, JsonItemNameDeadline)->valuestring, "N/A") == 0
        || strcmp(cJSON_GetObjectItem(requirement, JsonItemNameDeadline)->valuestring, "") == 0){
        return true;
    }

    return false;
}


static cJSON* parseVerificationInformation(cJSON* requirements){
    int num_reqs = cJSON_GetArraySize(requirements);

    cJSON* verificationInformation = cJSON_CreateObject();

    for (int i = 0; i < num_reqs; i++) {
        const cJSON *requirement = cJSON_GetArrayItem(requirements, i);

        if (!cJSON_IsObject(requirement)) {
            log_message(LOG_ERROR, "Error: requirement is not a JSON object");
            continue;
        }

        for (int verificationNumber = 1; verificationNumber <= MAXIMUM_NUMBER_OF_VERIFICATIONS; verificationNumber++){

            char JsonItemNameDeadline[50];
            snprintf(JsonItemNameDeadline, sizeof(JsonItemNameDeadline), "Verification Deadline %d", verificationNumber);

            char JsonItemNameMethod[50];
            snprintf(JsonItemNameMethod, sizeof(JsonItemNameMethod), "Verification Method %d", verificationNumber);

            char JsonItemNameStatus[50];
            snprintf(JsonItemNameStatus, sizeof(JsonItemNameStatus), "Verification Status %d", verificationNumber);

            if(verificationDeadlineEmpty(requirement, JsonItemNameDeadline)){
                continue;
            }

            char* deadlineName = cJSON_GetObjectItem(requirement, JsonItemNameDeadline)->valuestring;
            char* methodName = cJSON_GetObjectItem(requirement, JsonItemNameMethod)->valuestring;
            char* statusName = cJSON_GetObjectItem(requirement, JsonItemNameStatus)->valuestring;

            cJSON* deadlineObject = getDeadlineObject(verificationInformation, deadlineName);
            cJSON* statusArray = getStatusArray(deadlineObject, statusName);
            cJSON* methodArray = getMethodArray(statusArray, methodName);
            cJSON* stringItem = cJSON_CreateString(cJSON_GetObjectItem(requirement, "ID")->valuestring);

            cJSON_AddItemToArray(methodArray, stringItem);
        }
    }

    return verificationInformation;
}
