#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <cjson/cJSON.h>
#include "sheetAPI.h"
#include "ERTbot_common.h"
#include "ERTbot_config"
#include "requirementsHelpers.h"
#include "stringHelpers.h"
#include "wikiAPI.h"
#include "pageListHelpers.h"


/**
 * @brief Creates a Vega chart configuration for a pie chart depicting verification statuses.
 *
 * This function generates a Vega chart specification for a pie chart that visualizes the distribution of three categories: unverified, partially verified, and verified populations. The chart is formatted using Vega's schema and is designed to be displayed with the `kroki` tool.
 *
 * @param unverifiedPopulation A string representing the population of unverified items. This value is inserted into the Vega chart configuration.
 * @param partiallyVerifiedPopulation A string representing the population of partially verified items. This value is inserted into the Vega chart configuration.
 * @param verifiedPopulation A string representing the population of verified items. This value is inserted into the Vega chart configuration.
 *
 * @return A dynamically allocated string containing the Vega chart specification. The returned string includes the provided population values substituted into the template.
 *
 * @details
 * - The function starts with a predefined Vega chart template in string format.
 * - It replaces placeholder values (`DefaultUnverifiedPopulation`, `DefaultPartiallyVerifiedPopulation`, `DefaultVerifiedPopulation`) in the template with the provided arguments.
 * - The final Vega chart specification is returned, ready to be used for rendering a pie chart.
 */
static char *createVcdPieChart(const int* verificationStatusCount);


static void countVerificationStatus(cJSON *requirementList, int* verificationStatusCount);


static char *buildVcdList(cJSON *requirementList, char* subSystem);

void updateVcdPage(command cmd){
    log_message(LOG_DEBUG, "Entering function updateVcdPage");

    char *sheetId;
    char *vcdPageId;

    if(strcmp(cmd.argument, "ST")==0){
        vcdPageId = "1186";
        sheetId = "ST!A3:AT300";
    }
    if(strcmp(cmd.argument, "PR")==0){
        vcdPageId = "1187";
        sheetId = "PR!A3:AT300";
    }
    if(strcmp(cmd.argument, "FD")==0){
        vcdPageId = "1182";
        sheetId = "FD!A3:AT300";
    }
    if(strcmp(cmd.argument, "RE")==0){
        vcdPageId = "1185";
        sheetId = "RE!A3:AT300";
    }
    if(strcmp(cmd.argument, "GS")==0){
        vcdPageId = "1183";
        sheetId = "GS!A3:AT300";
    }
    if(strcmp(cmd.argument, "AV")==0){
        vcdPageId = "1181";
        sheetId = "AV!A3:AT300";
    }
    if(strcmp(cmd.argument, "TE")==0){
        vcdPageId =
        sheetId = "TE!A3:AT300";
    }
    if(strcmp(cmd.argument, "PL")==0){
        vcdPageId = "1184";
        sheetId = "PL!A3:AT300";
    }
    if(strcmp(cmd.argument, "GE")==0){
        vcdPageId = "1180";
        sheetId = "GE!A3:AT300";
    }

    batchGetSheet("1i_PTwIqLuG9IUI73UaGuOvx8rVTDV1zIS7gmXNjMs1I", sheetId);
    log_message(LOG_DEBUG, "chunk.response: %s", chunk.response);
    cJSON *requirementList = parseArrayIntoJSONRequirementList(chunk.response);

    int verificationStatusCount[3];
    countVerificationStatus(requirementList, verificationStatusCount);
    char *pieChart = createVcdPieChart(verificationStatusCount);

    char *VCD = pieChart;
    char *listOfRequirements = buildVcdList(requirementList, cmd.argument);
    VCD = appendToString(VCD, listOfRequirements);

    free(listOfRequirements);

    pageList* vcdPage = NULL;
    vcdPage = addPageToList(&vcdPage, vcdPageId, NULL, NULL, NULL, VCD, NULL);

    vcdPage->content = replaceWord_Realloc(vcdPage->content, "\n", "\\\\n");
    vcdPage->content = replaceWord_Realloc(vcdPage->content, "\"", "\\\\\\\"");

    updatePageContentMutation(vcdPage);
    renderMutation(&vcdPage, false);
    freePageList(&vcdPage);

    cJSON_Delete(requirementList);


    free(VCD);

    log_message(LOG_DEBUG, "Exiting function updateVcdPage");
    return;
}

static char *buildVcdList(cJSON *requirementList, char* subSystem){
    log_message(LOG_DEBUG, "Entering function buildVcdList");

    // Get the requirements array from the requirementList object
    cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");
    if (!cJSON_IsArray(requirements)) {
        log_message(LOG_ERROR, "Error: requirements is not a JSON array");
    }

    const char *VCD_header = "\n# Verification Status for next Review\n\n# {.tabset}";

    const char *UVROD_header = "\n### Review Of Design\n";
    const char *IPVROD_header = "\n### Review Of Design\n";
    const char *VROD_header  = "\n### Review Of Design\n";

    const char *UVTE_header = "\n### Test\n";
    const char *IPVTE_header = "\n### Test\n";
    const char *VTE_header  = "\n### Test\n";

    const char *UVIN_header = "\n### Inspection\n";
    const char *IPVIN_header = "\n### Inspection\n";
    const char *VIN_header  = "\n### Inspection\n";

    const char *UVAN_header = "\n### Analysis\n";
    const char *IPVAN_header = "\n### Analysis\n";
    const char *VAN_header  = "\n### Analysis\n";


    char *VCD = (char *)malloc(strlen(VCD_header) + 1);
    char *UVROD = (char *)malloc(strlen(UVROD_header) + 1);
    char *IPVROD = (char *)malloc(strlen(IPVROD_header) + 1);
    char *VROD = (char *)malloc(strlen(VROD_header) + 1);
    char *UVTE = (char *)malloc(strlen(UVTE_header) + 1);
    char *IPVTE = (char *)malloc(strlen(IPVTE_header) + 1);
    char *VTE = (char *)malloc(strlen(VTE_header) + 1);
    char *UVIN = (char *)malloc(strlen(UVIN_header) + 1);
    char *IPVIN = (char *)malloc(strlen(IPVIN_header) + 1);
    char *VIN = (char *)malloc(strlen(VIN_header) + 1);
    char *UVAN = (char *)malloc(strlen(UVAN_header) + 1);
    char *IPVAN = (char *)malloc(strlen(IPVAN_header) + 1);
    char *VAN = (char *)malloc(strlen(VAN_header) + 1);


    strcpy(VCD, VCD_header);
    strcpy(UVROD, UVROD_header);
    strcpy(IPVROD, IPVROD_header);
    strcpy(VROD, VROD_header);
    strcpy(UVTE, UVTE_header);
    strcpy(IPVTE, IPVTE_header);
    strcpy(VTE, VTE_header);
    strcpy(UVIN, UVIN_header);
    strcpy(IPVIN, IPVIN_header);
    strcpy(VIN, VIN_header);
    strcpy(UVAN, UVAN_header);
    strcpy(IPVAN, IPVAN_header);
    strcpy(VAN, VAN_header);



    // Iterate over each requirement object in the requirements array
    int num_reqs = cJSON_GetArraySize(requirements);
    for (int i = 0; i < num_reqs; i++) {
        cJSON *requirement = cJSON_GetArrayItem(requirements, i);
        if (!cJSON_IsObject(requirement)) {
            log_message(LOG_ERROR, "Error: requirement is not a JSON object");
            continue;
        }

        // Get and print each item of the requirement object
        cJSON *id = cJSON_GetObjectItemCaseSensitive(requirement, "ID");
        cJSON *title = cJSON_GetObjectItemCaseSensitive(requirement, "Title");
        cJSON *assignee = cJSON_GetObjectItemCaseSensitive(requirement, "Assignee");


        if(strlen(id->valuestring) < 2){
            log_message(LOG_DEBUG, "ID is smaller than one, breaking");
            break;
        }

        if(title == NULL|| strstr(id->valuestring, "2024_") == NULL){
            log_message(LOG_DEBUG, "Found a new group: %s", id->valuestring);
            continue;
        }

        cJSON *R1V1S = cJSON_GetObjectItemCaseSensitive(requirement, "Review 1 Verification 1 Status");
        cJSON *R1V2S = cJSON_GetObjectItemCaseSensitive(requirement, "Review 1 Verification 2 Status");
        cJSON *R1V3S = cJSON_GetObjectItemCaseSensitive(requirement, "Review 1 Verification 3 Status");

        cJSON *R1V1M = cJSON_GetObjectItemCaseSensitive(requirement, "Review 1 Verification 1 Method");
        cJSON *R1V2M = cJSON_GetObjectItemCaseSensitive(requirement, "Review 1 Verification 2 Method");
        cJSON *R1V3M = cJSON_GetObjectItemCaseSensitive(requirement, "Review 1 Verification 3 Method");

        char **targetList = NULL;


        for(int i = 0; i < 3; i++){
            cJSON *tempStatus = NULL;
            cJSON *tempMethod = NULL;

            switch (i)
            {
            case 0:
                tempStatus = R1V1S;
                tempMethod = R1V1M;

                log_message(LOG_DEBUG, "tempStatus string values: %s", tempStatus->valuestring);
                log_message(LOG_DEBUG, "tempMethod string values: %s", tempMethod->valuestring);
                break;

            case 1:
                tempStatus = R1V2S;
                tempMethod = R1V2M;
                log_message(LOG_DEBUG, "tempStatus string values: %s", tempStatus->valuestring);
                log_message(LOG_DEBUG, "tempMethod string values: %s", tempMethod->valuestring);
                break;

            case 2:
                tempStatus = R1V3S;
                tempMethod = R1V3M;
                log_message(LOG_DEBUG, "tempStatus string values: %s", tempStatus->valuestring);
                log_message(LOG_DEBUG, "tempMethod string values: %s", tempMethod->valuestring);
                break;

            default:
                break;
            }

            if(strcmp(tempMethod->valuestring, "N/A") == 0 || strcmp(tempStatus->valuestring, "N/A") == 0){
                continue;
            }

            if(strcmp(tempMethod->valuestring, "Review of Design") == 0 && strcmp(tempStatus->valuestring, "Completed") == 0){
                targetList = &VROD;
            }

            if(strcmp(tempMethod->valuestring, "Review of Design") == 0 && strcmp(tempStatus->valuestring, "In progress") == 0){
                targetList = &IPVROD;
            }

            if(strcmp(tempMethod->valuestring, "Review of Design") == 0 && strcmp(tempStatus->valuestring, "Uncompleted") == 0){
                targetList = &UVROD;
            }

            if(strcmp(tempMethod->valuestring, "Test") == 0 && strcmp(tempStatus->valuestring, "Completed") == 0){
                targetList = &VTE;
            }

            if(strcmp(tempMethod->valuestring, "Test") == 0 && strcmp(tempStatus->valuestring, "In progress") == 0){
                targetList = &IPVTE;
            }

            if(strcmp(tempMethod->valuestring, "Test") == 0 && strcmp(tempStatus->valuestring, "Uncompleted") == 0){
                targetList = &UVTE;
            }

            if(strcmp(tempMethod->valuestring, "Analysis") == 0 && strcmp(tempStatus->valuestring, "Completed") == 0){
                targetList = &VAN;
            }

            if(strcmp(tempMethod->valuestring, "Analysis") == 0 && strcmp(tempStatus->valuestring, "In progress") == 0){
                targetList = &IPVAN;
            }

            if(strcmp(tempMethod->valuestring, "Analysis") == 0 && strcmp(tempStatus->valuestring, "Uncompleted") == 0){
                targetList = &UVAN;
            }

            if(strcmp(tempMethod->valuestring, "Inspection") == 0 && strcmp(tempStatus->valuestring, "Completed") == 0){
                targetList = &VIN;
            }

            if(strcmp(tempMethod->valuestring, "Inspection") == 0 && strcmp(tempStatus->valuestring, "In progress") == 0){
                targetList = &IPVIN;
            }

            if(strcmp(tempMethod->valuestring, "Inspection") == 0 && strcmp(tempStatus->valuestring, "Uncompleted") == 0){
                targetList = &UVIN;
            }

            if (cJSON_IsString(id) && id->valuestring) {
                log_message(LOG_DEBUG, "ID: %s", id->valuestring);
                *targetList = appendToString(*targetList, "- [");
                *targetList = appendToString(*targetList, id->valuestring);
                *targetList = appendToString(*targetList, "](/");
                *targetList = appendToString(*targetList, "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_");
                *targetList = appendToString(*targetList, subSystem);
                *targetList = appendToString(*targetList, "_DRL/");
                *targetList = appendToString(*targetList, id->valuestring);
                *targetList = appendToString(*targetList, ") **");
            }

            if (cJSON_IsString(title) && title->valuestring) {
                log_message(LOG_DEBUG, "title: %s", title->valuestring);
                *targetList = appendToString(*targetList, title->valuestring);
                *targetList = appendToString(*targetList, "**\n");
            }

            if (cJSON_IsString(assignee) && assignee->valuestring) {
                log_message(LOG_DEBUG, "assignee: %s", assignee->valuestring);
                *targetList = appendToString(*targetList, "**Assignee**: ");
                *targetList = appendToString(*targetList, assignee->valuestring);
                *targetList = appendToString(*targetList, "\n");
            }

            log_message(LOG_DEBUG, "targetList string value: %s", targetList);

        }
    }

    UVROD =  appendToString(UVROD, "\n{.links-list}");
    IPVROD =  appendToString(IPVROD, "\n{.links-list}");
    VROD  =  appendToString(VROD, "\n{.links-list}");

    UVTE =  appendToString(UVTE, "\n{.links-list}");
    IPVTE =  appendToString(IPVTE, "\n{.links-list}");
    VTE  =  appendToString(VTE, "\n{.links-list}");

    UVIN =  appendToString(UVIN, "\n{.links-list}");
    IPVIN =  appendToString(IPVIN, "\n{.links-list}");
    VIN  =  appendToString(VIN, "\n{.links-list}");

    UVAN =  appendToString(UVAN, "\n{.links-list}");
    IPVAN =  appendToString(IPVAN, "\n{.links-list}");
    VAN  =  appendToString(VAN, "\n{.links-list}");

    VCD = appendToString(VCD, "\n## Unverified Requirements\n");
    VCD = appendToString(VCD, UVROD);
    VCD = appendToString(VCD, UVTE);
    VCD = appendToString(VCD, UVIN);
    VCD = appendToString(VCD, UVAN);

    VCD = appendToString(VCD, "\n## Verification In Progress\n");
    VCD = appendToString(VCD, IPVROD);
    VCD = appendToString(VCD, IPVTE);
    VCD = appendToString(VCD, IPVIN);
    VCD = appendToString(VCD, IPVAN);

    VCD = appendToString(VCD, "\n## Verified Requirements\n");
    VCD = appendToString(VCD, VROD);
    VCD = appendToString(VCD, VTE);
    VCD = appendToString(VCD, VIN);
    VCD = appendToString(VCD, VAN);


    if(UVROD){
        free(UVROD);
    }
    if(IPVROD){
        free(IPVROD);
    }
    if(VROD){
        free(VROD);
    }
    if(UVTE){
        free(UVTE);
    }
    if(IPVTE){
        free(IPVTE);
    }
    if(VTE){
        free(VTE);
    }
    if(UVIN){
        free(UVIN);
    }
    if(IPVIN){
        free(IPVIN);
    }
    if(VIN){
        free(VIN);
    }
    if(UVAN){
        free(UVAN);
    }
    if(IPVAN){
        free(IPVAN);
    }
    if(VAN){
        free(VAN);
    }

    log_message(LOG_DEBUG, "Exiting function buildVcdList");
    return VCD;
}

static int getStatusCount(const cJSON* deadlineObject, const char* statusName){
    const cJSON* statusObject = cJSON_GetObjectItem(deadlineObject, statusName);
    return cJSON_GetArraySize(statusObject);
}

static char *createVcdPieChart(const cJSON* deadlineObject){
    log_message(LOG_DEBUG, "Entering function createVcdPieChart");

    char *pieChart = "```kroki\nvega\n\n{\n  \"$schema\": \"https://vega.github.io/schema/vega/v5.0.json\",\n  \"width\": 350,\n  \"height\": 350,\n  \"autosize\": \"pad\",\n  \"signals\": [\n    {\"name\": \"startAngle\", \"value\": 0},\n    {\"name\": \"endAngle\", \"value\": 6.29},\n    {\"name\": \"padAngle\", \"value\": 0},\n    {\"name\": \"sort\", \"value\": true},\n    {\"name\": \"strokeWidth\", \"value\": 2},\n    {\n      \"name\": \"selected\",\n      \"value\": \"\",\n      \"on\": [{\"events\": \"mouseover\", \"update\": \"datum\"}]\n    }\n  ],\n  \"data\": [\n    {\n      \"name\": \"table\",\n      \"values\": [\n        {\"continent\": \"Unverified\", \"population\": DefaultUnverifiedPopulation},\n        {\"continent\": \"Partially Verified\", \"population\": DefaultPartiallyVerifiedPopulation},\n        {\"continent\": \"Verified\", \"population\": DefaultVerifiedPopulation}\n      ],\n      \"transform\": [\n        {\n          \"type\": \"pie\",\n          \"field\": \"population\",\n          \"startAngle\": {\"signal\": \"startAngle\"},\n          \"endAngle\": {\"signal\": \"endAngle\"},\n          \"sort\": {\"signal\": \"sort\"}\n        }\n      ]\n    },\n    {\n      \"name\": \"fieldSum\",\n      \"source\": \"table\",\n      \"transform\": [\n        {\n          \"type\": \"aggregate\",\n          \"fields\": [\"population\"],\n          \"ops\": [\"sum\"],\n          \"as\": [\"sum\"]\n        }\n      ]\n    }\n  ],\n  \"legends\": [\n    {\n      \"fill\": \"color\",\n      \"title\": \"Legends\",\n      \"orient\": \"none\",\n      \"padding\": {\"value\": 10},\n      \"encode\": {\n        \"symbols\": {\"enter\": {\"fillOpacity\": {\"value\": 1}}},\n        \"legend\": {\n          \"update\": {\n            \"x\": {\n              \"signal\": \"(width / 2) + if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.1 * 0.8, if(width >= height, height, width) / 2 * 0.8)\",\n              \"offset\": 20\n            },\n            \"y\": {\"signal\": \"(height / 2)\", \"offset\": -50}\n          }\n        }\n      }\n    }\n  ],\n  \"scales\": [\n    {\"name\": \"color\", \"type\": \"ordinal\", \"range\": [\"#cf2608\", \"#ff9900\", \"#67b505\"]}\n  ],\n  \"marks\": [\n    {\n      \"type\": \"arc\",\n      \"from\": {\"data\": \"table\"},\n      \"encode\": {\n        \"enter\": {\n          \"fill\": {\"scale\": \"color\", \"field\": \"continent\"},\n          \"x\": {\"signal\": \"width / 2\"},\n          \"y\": {\"signal\": \"height / 2\"}\n        },\n        \"update\": {\n          \"startAngle\": {\"field\": \"startAngle\"},\n          \"endAngle\": {\"field\": \"endAngle\"},\n          \"cornerRadius\": {\"value\": 15},\n          \"padAngle\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, 0.015, 0.015)\"\n          },\n          \"innerRadius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 0.45, if(width >= height, height, width) / 2 * 0.5)\"\n          },\n          \"outerRadius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.05 * 0.8, if(width >= height, height, width) / 2 * 0.8)\"\n          },\n          \"opacity\": {\n            \"signal\": \"if(selected && selected.continent !== datum.continent, 1, 1)\"\n          },\n          \"stroke\": {\"signal\": \"scale('color', datum.continent)\"},\n          \"strokeWidth\": {\"signal\": \"strokeWidth\"},\n          \"fillOpacity\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, 0.8, 0.8)\"\n          }\n        }\n      }\n    },\n    {\n      \"type\": \"text\",\n      \"encode\": {\n        \"enter\": {\"fill\": {\"value\": \"#525252\"}, \"text\": {\"value\": \"\"}},\n        \"update\": {\n          \"opacity\": {\"value\": 1},\n          \"x\": {\"signal\": \"width / 2\"},\n          \"y\": {\"signal\": \"height / 2\"},\n          \"align\": {\"value\": \"center\"},\n          \"baseline\": {\"value\": \"middle\"},\n          \"fontSize\": {\"signal\": \"if(width >= height, height, width) * 0.05\"},\n          \"text\": {\"value\": \"Verification Status\"}\n        }\n      }\n    },\n    {\n      \"name\": \"mark_population\",\n      \"type\": \"text\",\n      \"from\": {\"data\": \"table\"},\n      \"encode\": {\n        \"enter\": {\n          \"text\": {\n            \"signal\": \"if(datum['endAngle'] - datum['startAngle'] < 0.3, '', format(datum['population'] / 1, '.0f'))\"\n          },\n          \"x\": {\"signal\": \"if(width >= height, height, width) / 2\"},\n          \"y\": {\"signal\": \"if(width >= height, height, width) / 2\"},\n          \"radius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.05 * 0.65, if(width >= height, height, width) / 2 * 0.65)\"\n          },\n          \"theta\": {\"signal\": \"(datum['startAngle'] + datum['endAngle'])/2\"},\n          \"fill\": {\"value\": \"#FFFFFF\"},\n          \"fontSize\": {\"value\": 12},\n          \"align\": {\"value\": \"center\"},\n          \"baseline\": {\"value\": \"middle\"}\n        }\n      }\n    }\n  ]\n}\n\n```";

    char unverifiedPopulation[10];
    sprintf(unverifiedPopulation, "%d", getStatusCount(deadlineObject, "uncompleted"));

    char partiallyVerifiedPopulation[10];
    sprintf(partiallyVerifiedPopulation, "%d", getStatusCount(deadlineObject, "in progress"));

    char verifiedPopulation[10];
    sprintf(verifiedPopulation, "%d", getStatusCount(deadlineObject, "completed"));

    pieChart = replaceWord_Malloc(pieChart, "DefaultUnverifiedPopulation", unverifiedPopulation);
    pieChart = replaceWord_Realloc(pieChart, "DefaultPartiallyVerifiedPopulation", partiallyVerifiedPopulation);
    pieChart = replaceWord_Realloc(pieChart, "DefaultVerifiedPopulation", verifiedPopulation);

    log_message(LOG_DEBUG, "Exiting function createVcdPieChart");

    return pieChart;
}

static cJSON getDeadlineObject(cJSON* verificationInformation, char* deadlineName){
    int arraySize = cJSON_GetArraySize(verificationInformation);

    if(arraySize == 0){
        return addDeadlineObject(verificationInformation, deadlineName);
    }

    for(int i = 0; i < arraySize; i++){
        char JsonItemNameDeadline[50];
        snprintf(JsonItemNameDeadline, sizeof(JsonItemNameDeadline), "Deadline %d", i);

        cJSON* deadlineObject = cJSON_GetObjectItem(verificationInformation, JsonItemNameDeadline);
        
        if(strcmp((deadlineObject, "Deadline Name")->valuestring, deadlineName) == 0){
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

    cJSON* deadlineName = cJSON_CreateString(deadlineName);
    cJSON* deadlineObject = cJSON_CreateObject();

    cJSON_AddItemToObject(verificationInformation, JsonItemNameDeadline, deadlineObject);
    cJSON_AddItemToObject(deadlineObject, "Deadline Name", deadlineName);

    return;
}

static cJSON* getMethodArray(cJSON* statusObject, char* methodName){

    if(cJSON_HasObjectItem(statusObject, methodName);){
        return cJSON_GetObjectItem(statusObject, methodName);
    }

    cJSON* methodeArray = cJSON_CreateArray();
    cJSON_AddItemToObject(statusObject, methodName, methodeArray);

   return cJSON_GetObjectItem(statusObject, methodName);
}

static cJSON* getStatusArray(cJSON* deadlineObject, char* statusName){

    if(cJSON_HasObjectItem(deadlineObject, statusName);){
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
    if(!cJSON_HasObjectItem(requirement, JsonItemNameDeadline) || strcmp(cJSON_GetObjectItem(requirement, JsonItemNameDeadline)->valuestring, "N/A") == 0 
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
            cJSON* stringItem = cJSON_CreateString(cJSON_GetObjectItem(requirement, "ID")->valuestring)

            cJSON_AddItemToArray(methodArray, stringItem);
        }
    }

    return verificationInformation;

}