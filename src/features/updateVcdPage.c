#include <float.h>
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
#include "slackAPI.h"

#define VCD_TITLE_TEMPLATE "# Verification Statuses per Deadline\n"
#define DEADLINE_SUBSECTION_TITLE_TEMPLATE "\n## $Deadline Name$"
#define STATUS_SUBSUBSECTION_TITLE_TEMPLATE "\n### $Status Name$"
#define METHOD_SUBSUBSUBSECTION_TITLE_TEMPLATE "\n#### $Method Name$\n"

#define VCD_TABSET_TITLE_TEMPLATE "\n\n\n## $ID$\n"
#define VCD_ID_BLOCK_TEMPLATE "- [$ID$](/"
#define VCD_PAGE_DIRECTORY "$Requirement Pages Directory$"
#define VCD_PAGE_NAME "$ID$) **"
#define VCD_TITLE_BLOCK_TEMPLATE "$Title$**\n"

static char* buildVCD(const cJSON* verificationInformation, const cJSON* requirements, const cJSON* subsystem);

static int parseDeadlineBlock(const cJSON* verificationInformation, char** pageContent, const char* deadlineItemName, const cJSON* requirements, const cJSON* subsystem);

static int parseStatusBlock(char** pageContent, const cJSON* deadlineObject, const char* statusName, const cJSON* requirements, const cJSON* subsystem);

static int parseMethodBlock(char** pageContent, const cJSON* statusObject, const char* methodName, const cJSON* requirements, const cJSON* subsystem);

static int parseRequirementBlock(char** pageContent, const cJSON* requirement, const cJSON* subsystem);

static int getStatusCount(const cJSON* deadlineObject, const char* statusName);

static int countSizeOfArrayItem(const cJSON* statusObject, const char* arrayItemName);

static int appendVcdPieChart(char** pageContent, const cJSON* deadlineObject);

static cJSON* getDeadlineObject(cJSON* verificationInformation, const char* deadlineName);

static cJSON* addDeadlineObject(cJSON* verificationInformation, const char* deadlineName);

static cJSON* getMethodArray(cJSON* statusObject, const char* methodName);

static cJSON* getStatusObject(cJSON* deadlineObject, const char* statusName);

static bool verificationMethodAlreadyExists(const cJSON* deadlineObject, const char* verificationMethod);

static bool verificationDeadlineEmpty(const cJSON* requirement, const char* JsonItemNameDeadline);

static cJSON* parseVerificationInformation(const cJSON* requirements);

void updateVcdPage(command cmd){
    log_function_entry(__func__);

    updateCommandStatusMessage("fetching subsystem info");
    cJSON* subsystem = getSubsystemInfo(cmd.argument);
    const char *vcdPageId = cJSON_GetObjectItem(subsystem, "VCD Page ID")->valuestring;

    updateCommandStatusMessage("fetching requirements");
    cJSON *requirementList = getRequirements(subsystem);
    const cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");

    updateCommandStatusMessage("parsing requirement verification information");
    cJSON* verificationInformation = parseVerificationInformation(requirements);

    updateCommandStatusMessage("building VCD page content");
    char* pageContent = buildVCD(verificationInformation, requirements, subsystem);

    log_message(LOG_DEBUG, __func__, "pageContent after buildVCD:\n%s\n", pageContent);

    pageList* vcdPage = NULL;

    log_message(LOG_DEBUG, __func__, "vcdPageId%s", vcdPageId);

    vcdPage = addPageToList(&vcdPage, vcdPageId, NULL, NULL, NULL, pageContent, NULL);

    vcdPage->content = replaceWord_Realloc(vcdPage->content, "\n", "\\\\n");
    vcdPage->content = replaceWord_Realloc(vcdPage->content, "\"", "\\\\\\\"");

    updateCommandStatusMessage("updating VCD page content");
    updatePageContentMutation(vcdPage);
    renderMutation(&vcdPage, false);
    freePageList(&vcdPage);

    cJSON_Delete(requirementList);
    cJSON_Delete(subsystem);
    cJSON_Delete(verificationInformation);
    free(pageContent);
    log_function_exit(__func__);
    return;
}

static char* buildVCD(const cJSON* verificationInformation, const cJSON* requirements, const cJSON* subsystem){
    log_function_entry(__func__);

    int amountOfDifferentDeadlines = cJSON_GetArraySize(verificationInformation);
    char* pageContent = duplicate_Malloc(VCD_TITLE_TEMPLATE);

    for(int i = 1; i <= amountOfDifferentDeadlines; i++){

        char deadlineItemName[50];
        snprintf(deadlineItemName, sizeof(deadlineItemName), "Deadline %d", i);

        (void)parseDeadlineBlock(verificationInformation, &pageContent, deadlineItemName, requirements, subsystem);
    }

    log_function_exit(__func__);
    return pageContent;
}

static int parseDeadlineBlock(const cJSON* verificationInformation, char** pageContent, const char* deadlineItemName, const cJSON* requirements, const cJSON* subsystem){
    log_function_entry(__func__);
    if(!cJSON_HasObjectItem(verificationInformation, deadlineItemName)){
        return 0;
    }

    const cJSON* deadlineObject = cJSON_GetObjectItem(verificationInformation, deadlineItemName);
    (void)addSectionToPageContent(pageContent, DEADLINE_SUBSECTION_TITLE_TEMPLATE, deadlineObject, "Deadline Name");

    (void)appendVcdPieChart(pageContent, deadlineObject);

    (void)parseStatusBlock(pageContent, deadlineObject, "uncompleted", requirements, subsystem);

    (void)parseStatusBlock(pageContent, deadlineObject, "in progress", requirements, subsystem);

    (void)parseStatusBlock(pageContent, deadlineObject, "completed", requirements, subsystem);

    log_function_exit(__func__);
    return 1;
}

static int parseStatusBlock(char** pageContent, const cJSON* deadlineObject, const char* statusName, const cJSON* requirements, const cJSON* subsystem){
    log_function_entry(__func__);

    if(!cJSON_HasObjectItem(deadlineObject, statusName)){
        return 0;
    }

    const cJSON* statusObject = cJSON_GetObjectItem(deadlineObject, statusName);
    *pageContent = appendToString(*pageContent, STATUS_SUBSUBSECTION_TITLE_TEMPLATE);

    *pageContent = replaceWord_Realloc(*pageContent, "$Status Name$", statusName);

    (void)parseMethodBlock(pageContent, statusObject, "Test", requirements, subsystem);

    (void)parseMethodBlock(pageContent, statusObject, "Review Of Design", requirements, subsystem);

    (void)parseMethodBlock(pageContent, statusObject, "Inspection", requirements, subsystem);

    (void)parseMethodBlock(pageContent, statusObject, "Analysis", requirements, subsystem);


    log_function_exit(__func__);
    return 1;
}

static int parseMethodBlock(char** pageContent, const cJSON* statusObject, const char* methodName, const cJSON* requirements, const cJSON* subsystem){
    log_function_entry(__func__);

    if(!cJSON_HasObjectItem(statusObject, methodName)){
        return 0;
    }

    const cJSON* methodArray = cJSON_GetObjectItem(statusObject, methodName);
    *pageContent = appendToString(*pageContent, METHOD_SUBSUBSUBSECTION_TITLE_TEMPLATE);
    *pageContent = replaceWord_Realloc(*pageContent, "$Method Name$", methodName);

    int methodArraySize = cJSON_GetArraySize(methodArray);
    log_message(LOG_DEBUG, __func__, "methodArraySize: %d", methodArraySize);

    int requirementsArraySize = cJSON_GetArraySize(requirements);
    log_message(LOG_DEBUG, __func__, "requirementsArraySize: %d", requirementsArraySize);


    for(int j = 0; j < methodArraySize; j++){
        char* requirementId = cJSON_GetArrayItem(methodArray, j)->valuestring;
        log_message(LOG_DEBUG, __func__, "Looking for requirment ID %s", requirementId);

        for(int k = 0; k < requirementsArraySize; k++){
            const cJSON* requirement = cJSON_GetArrayItem(requirements, k);

            if(cJSON_HasObjectItem(requirement, "ID")
                && cJSON_IsString(cJSON_GetObjectItem(requirement, "ID"))
                && strcmp(cJSON_GetObjectItem(requirement, "ID")->valuestring, requirementId) == 0){

                    log_message(LOG_DEBUG, __func__, "Found Requirement!", requirementId);
                (void)parseRequirementBlock(pageContent, requirement, subsystem);
            }
        }
    }

    *pageContent = appendToString(*pageContent, "{.links-list}");

    log_function_exit(__func__);
    return 1;
}

static int parseRequirementBlock(char** pageContent, const cJSON* requirement, const cJSON* subsystem){
    log_function_entry(__func__);
    (void)addSectionToPageContent(pageContent, VCD_ID_BLOCK_TEMPLATE, requirement, "ID");
    (void)addSectionToPageContent(pageContent, VCD_PAGE_DIRECTORY, subsystem, "Requirement Pages Directory");
    (void)addSectionToPageContent(pageContent, VCD_PAGE_NAME, requirement, "ID");
    int hasTitle = addSectionToPageContent(pageContent, VCD_TITLE_BLOCK_TEMPLATE, requirement, "Title");

    log_function_exit(__func__);
    return hasTitle;
}

static int getStatusCount(const cJSON* deadlineObject, const char* statusName){
    log_function_entry(__func__);

    const cJSON* statusObject = cJSON_GetObjectItem(deadlineObject, statusName);

    int count = 0;

    count += countSizeOfArrayItem(statusObject, "Test");
    count += countSizeOfArrayItem(statusObject, "Inspection");
    count += countSizeOfArrayItem(statusObject, "Review Of Design");
    count += countSizeOfArrayItem(statusObject, "Analysis");

    log_function_exit(__func__);
    return count;
}

static int countSizeOfArrayItem(const cJSON* statusObject, const char* arrayItemName){

    int count = 0;

    if(cJSON_HasObjectItem(statusObject, arrayItemName)
        && cJSON_IsArray(cJSON_GetObjectItem(statusObject, arrayItemName))){
        count = count + cJSON_GetArraySize(cJSON_GetObjectItem(statusObject, arrayItemName));
    }

    return count;
}

static int appendVcdPieChart(char** pageContent, const cJSON* deadlineObject){
    log_function_entry(__func__);

    const char *pieChart = "\n```kroki\nvega\n\n{\n  \"$schema\": \"https://vega.github.io/schema/vega/v5.0.json\",\n  \"width\": 350,\n  \"height\": 350,\n  \"autosize\": \"pad\",\n  \"signals\": [\n    {\"name\": \"startAngle\", \"value\": 0},\n    {\"name\": \"endAngle\", \"value\": 6.29},\n    {\"name\": \"padAngle\", \"value\": 0},\n    {\"name\": \"sort\", \"value\": true},\n    {\"name\": \"strokeWidth\", \"value\": 2},\n    {\n      \"name\": \"selected\",\n      \"value\": \"\",\n      \"on\": [{\"events\": \"mouseover\", \"update\": \"datum\"}]\n    }\n  ],\n  \"data\": [\n    {\n      \"name\": \"table\",\n      \"values\": [\n        {\"continent\": \"Unverified\", \"population\": DefaultUnverifiedPopulation},\n        {\"continent\": \"Partially Verified\", \"population\": DefaultPartiallyVerifiedPopulation},\n        {\"continent\": \"Verified\", \"population\": DefaultVerifiedPopulation}\n      ],\n      \"transform\": [\n        {\n          \"type\": \"pie\",\n          \"field\": \"population\",\n          \"startAngle\": {\"signal\": \"startAngle\"},\n          \"endAngle\": {\"signal\": \"endAngle\"},\n          \"sort\": {\"signal\": \"sort\"}\n        }\n      ]\n    },\n    {\n      \"name\": \"fieldSum\",\n      \"source\": \"table\",\n      \"transform\": [\n        {\n          \"type\": \"aggregate\",\n          \"fields\": [\"population\"],\n          \"ops\": [\"sum\"],\n          \"as\": [\"sum\"]\n        }\n      ]\n    }\n  ],\n  \"legends\": [\n    {\n      \"fill\": \"color\",\n      \"title\": \"Legends\",\n      \"orient\": \"none\",\n      \"padding\": {\"value\": 10},\n      \"encode\": {\n        \"symbols\": {\"enter\": {\"fillOpacity\": {\"value\": 1}}},\n        \"legend\": {\n          \"update\": {\n            \"x\": {\n              \"signal\": \"(width / 2) + if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.1 * 0.8, if(width >= height, height, width) / 2 * 0.8)\",\n              \"offset\": 20\n            },\n            \"y\": {\"signal\": \"(height / 2)\", \"offset\": -50}\n          }\n        }\n      }\n    }\n  ],\n  \"scales\": [\n    {\"name\": \"color\", \"type\": \"ordinal\", \"range\": [\"#cf2608\", \"#ff9900\", \"#67b505\"]}\n  ],\n  \"marks\": [\n    {\n      \"type\": \"arc\",\n      \"from\": {\"data\": \"table\"},\n      \"encode\": {\n        \"enter\": {\n          \"fill\": {\"scale\": \"color\", \"field\": \"continent\"},\n          \"x\": {\"signal\": \"width / 2\"},\n          \"y\": {\"signal\": \"height / 2\"}\n        },\n        \"update\": {\n          \"startAngle\": {\"field\": \"startAngle\"},\n          \"endAngle\": {\"field\": \"endAngle\"},\n          \"cornerRadius\": {\"value\": 15},\n          \"padAngle\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, 0.015, 0.015)\"\n          },\n          \"innerRadius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 0.45, if(width >= height, height, width) / 2 * 0.5)\"\n          },\n          \"outerRadius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.05 * 0.8, if(width >= height, height, width) / 2 * 0.8)\"\n          },\n          \"opacity\": {\n            \"signal\": \"if(selected && selected.continent !== datum.continent, 1, 1)\"\n          },\n          \"stroke\": {\"signal\": \"scale('color', datum.continent)\"},\n          \"strokeWidth\": {\"signal\": \"strokeWidth\"},\n          \"fillOpacity\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, 0.8, 0.8)\"\n          }\n        }\n      }\n    },\n    {\n      \"type\": \"text\",\n      \"encode\": {\n        \"enter\": {\"fill\": {\"value\": \"#525252\"}, \"text\": {\"value\": \"\"}},\n        \"update\": {\n          \"opacity\": {\"value\": 1},\n          \"x\": {\"signal\": \"width / 2\"},\n          \"y\": {\"signal\": \"height / 2\"},\n          \"align\": {\"value\": \"center\"},\n          \"baseline\": {\"value\": \"middle\"},\n          \"fontSize\": {\"signal\": \"if(width >= height, height, width) * 0.05\"},\n          \"text\": {\"value\": \"Verification Status\"}\n        }\n      }\n    },\n    {\n      \"name\": \"mark_population\",\n      \"type\": \"text\",\n      \"from\": {\"data\": \"table\"},\n      \"encode\": {\n        \"enter\": {\n          \"text\": {\n            \"signal\": \"if(datum['endAngle'] - datum['startAngle'] < 0.3, '', format(datum['population'] / 1, '.0f'))\"\n          },\n          \"x\": {\"signal\": \"if(width >= height, height, width) / 2\"},\n          \"y\": {\"signal\": \"if(width >= height, height, width) / 2\"},\n          \"radius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.05 * 0.65, if(width >= height, height, width) / 2 * 0.65)\"\n          },\n          \"theta\": {\"signal\": \"(datum['startAngle'] + datum['endAngle'])/2\"},\n          \"fill\": {\"value\": \"#FFFFFF\"},\n          \"fontSize\": {\"value\": 12},\n          \"align\": {\"value\": \"center\"},\n          \"baseline\": {\"value\": \"middle\"}\n        }\n      }\n    }\n  ]\n}\n\n```\n## {.tabset}\n";

    char unverifiedPopulation[10];
    snprintf(unverifiedPopulation,  sizeof(unverifiedPopulation),"%d", getStatusCount(deadlineObject, "uncompleted"));

    char partiallyVerifiedPopulation[10];
    snprintf(partiallyVerifiedPopulation, sizeof(partiallyVerifiedPopulation), "%d", getStatusCount(deadlineObject, "in progress"));

    char verifiedPopulation[10];
    snprintf(verifiedPopulation, sizeof(verifiedPopulation), "%d", getStatusCount(deadlineObject, "completed"));

    *pageContent = appendToString(*pageContent, pieChart);
    *pageContent = replaceWord_Realloc(*pageContent, "DefaultUnverifiedPopulation", unverifiedPopulation);
    *pageContent = replaceWord_Realloc(*pageContent, "DefaultPartiallyVerifiedPopulation", partiallyVerifiedPopulation);
    *pageContent = replaceWord_Realloc(*pageContent, "DefaultVerifiedPopulation", verifiedPopulation);

    log_function_exit(__func__);

    return 1;
}

static cJSON* getDeadlineObject(cJSON* verificationInformation, const char* deadlineName){
    log_function_entry(__func__);
    int arraySize = cJSON_GetArraySize(verificationInformation);

    if(arraySize == 0){
        log_function_exit(__func__);
        return addDeadlineObject(verificationInformation, deadlineName);
    }

    for(int i = 1; i <= arraySize; i++){
        char JsonItemNameDeadline[50];
        snprintf(JsonItemNameDeadline, sizeof(JsonItemNameDeadline), "Deadline %d", i);

        cJSON* deadlineObject = cJSON_GetObjectItem(verificationInformation, JsonItemNameDeadline);

        if(strcmp(cJSON_GetObjectItem(deadlineObject, "Deadline Name")->valuestring, deadlineName) == 0){
            log_function_exit(__func__);
            return deadlineObject;
        }
    }

    log_function_exit(__func__);
    return addDeadlineObject(verificationInformation, deadlineName);
}

static cJSON* addDeadlineObject(cJSON* verificationInformation, const char* deadlineName){
    log_function_entry(__func__);
    int arraySize = cJSON_GetArraySize(verificationInformation);
    int deadlineNumber = arraySize + 1;

    char JsonItemNameDeadline[50];
    snprintf(JsonItemNameDeadline, sizeof(JsonItemNameDeadline), "Deadline %d", deadlineNumber);

    cJSON* deadlineNameItem = cJSON_CreateString(deadlineName);
    cJSON* deadlineObject = cJSON_CreateObject();

    cJSON_AddItemToObject(verificationInformation, JsonItemNameDeadline, deadlineObject);
    cJSON_AddItemToObject(deadlineObject, "Deadline Name", deadlineNameItem);

    log_function_exit(__func__);
    return deadlineObject;
}

static cJSON* getMethodArray(cJSON* statusObject, const char* methodName){
    log_function_entry(__func__);

    if(cJSON_HasObjectItem(statusObject, methodName)){
        return cJSON_GetObjectItem(statusObject, methodName);
    }

    cJSON* methodeArray = cJSON_CreateArray();
    cJSON_AddItemToObject(statusObject, methodName, methodeArray);

    log_function_exit(__func__);
   return cJSON_GetObjectItem(statusObject, methodName);
}

static cJSON* getStatusObject(cJSON* deadlineObject, const char* statusName){
    log_function_entry(__func__);

    if(cJSON_HasObjectItem(deadlineObject, statusName)){

        log_function_exit(__func__);
        return cJSON_GetObjectItem(deadlineObject, statusName);
    }

    cJSON* statusObject = cJSON_CreateObject();
    cJSON_AddItemToObject(deadlineObject, statusName, statusObject);

    log_function_exit(__func__);
   return cJSON_GetObjectItem(deadlineObject, statusName);
}

static bool verificationMethodAlreadyExists(const cJSON* deadlineObject, const char* verificationMethod){
    return cJSON_HasObjectItem(deadlineObject, verificationMethod);
}

static bool verificationDeadlineEmpty(const cJSON* requirement, const char* JsonItemNameDeadline){
    log_function_entry(__func__);
    if(!cJSON_HasObjectItem(requirement, JsonItemNameDeadline)
        || strcmp(cJSON_GetObjectItem(requirement, JsonItemNameDeadline)->valuestring, "N/A") == 0
        || strcmp(cJSON_GetObjectItem(requirement, JsonItemNameDeadline)->valuestring, "") == 0){

        log_function_exit(__func__);
        return true;
    }


    log_function_exit(__func__);
    return false;
}

static cJSON* parseVerificationInformation(const cJSON* requirements){
    log_function_entry(__func__);

    int num_reqs = cJSON_GetArraySize(requirements);

    cJSON* verificationInformation = cJSON_CreateObject();

    for (int i = 0; i < num_reqs; i++) {
        const cJSON *requirement = cJSON_GetArrayItem(requirements, i);

        if (!cJSON_IsObject(requirement)) {
            log_message(LOG_ERROR, __func__, "Error: requirement is not a JSON object");
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

            const char* deadlineName = cJSON_GetObjectItem(requirement, JsonItemNameDeadline)->valuestring;
            const char* methodName = cJSON_GetObjectItem(requirement, JsonItemNameMethod)->valuestring;
            const char* statusName = cJSON_GetObjectItem(requirement, JsonItemNameStatus)->valuestring;

            cJSON* deadlineObject = getDeadlineObject(verificationInformation, deadlineName);

            cJSON* statusObject = getStatusObject(deadlineObject, statusName);

            cJSON* methodArray = getMethodArray(statusObject, methodName);

            cJSON* stringItem = cJSON_CreateString(cJSON_GetObjectItem(requirement, "ID")->valuestring);


            cJSON_AddItemToArray(methodArray, stringItem);
        }

    }

    log_function_exit(__func__);
    return verificationInformation;
}
