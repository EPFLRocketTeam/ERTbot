#include "../include/struct.h"
#include "../include/api.h"
#include "../include/config.h"
#include "../include/features.h"
#include "../include/githubAPI.h"
#include "../include/helperFunctions.h"
#include "../include/markdownToPDF.h"
#include "../include/slackAPI.h"
#include "../include/stringTools.h"
#include "../include/wikiAPI.h"
#include "../include/sheetAPI.h"
#include "../include/command.h"
#include "../include/log.h"
#include "../include/requirements.h"

char *template_DRL = "# General Design Requirements List\n\n\n# table {.tabset}\n\n";
char *template_REQ = "";


char *updateVcdStackedAreaChart(char *json_str, char *week, int verifiedValue, int partiallyVerifiedValue, int unverifiedValue) {
    log_message(LOG_DEBUG, "Entering function updateVcdStackedAreaChart");
    

    log_message(LOG_DEBUG, "JSON string: %s", json_str);
    
    // Parse the input JSON string
    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL) {
        return NULL;
    }

    // Find the "values" array
    cJSON *data = cJSON_GetObjectItem(root, "data");
    if (data == NULL) {
        cJSON_Delete(root);
        return NULL;
    }

    cJSON *values = cJSON_GetObjectItem(data, "values");
    if (values == NULL || !cJSON_IsArray(values)) {
        cJSON_Delete(root);
        return NULL;
    }


    // Create new JSON objects for the three new items
    cJSON *item1 = cJSON_CreateObject();
    cJSON_AddStringToObject(item1, "week", week);
    cJSON_AddStringToObject(item1, "status", "Verified");
    cJSON_AddNumberToObject(item1, "value", verifiedValue);

    cJSON *item2 = cJSON_CreateObject();
    cJSON_AddStringToObject(item2, "week", week);
    cJSON_AddStringToObject(item2, "status", "Partially Verified");
    cJSON_AddNumberToObject(item2, "value", partiallyVerifiedValue);

    cJSON *item3 = cJSON_CreateObject();
    cJSON_AddStringToObject(item3, "week", week);
    cJSON_AddStringToObject(item3, "status", "Unverified");
    cJSON_AddNumberToObject(item3, "value", unverifiedValue);

    // Add the new items to the "values" array
    cJSON_AddItemToArray(values, item1);
    cJSON_AddItemToArray(values, item2);
    cJSON_AddItemToArray(values, item3);

    log_message(LOG_DEBUG, "about to call cJSON_Print");

    // Convert the updated JSON structure back to a string
    char *updated_json_str = cJSON_Print(root);

    // Clean up
    cJSON_Delete(root);

    
    log_message(LOG_DEBUG, "Exiting function updateVcdStackedAreaChart");
    return updated_json_str;
}

char *createVcdPieChart(int* verificationStatusCount){
    log_message(LOG_DEBUG, "Entering function createVcdPieChart");
    

    char *pieChart = "```kroki\nvega\n\n{\n  \"$schema\": \"https://vega.github.io/schema/vega/v5.0.json\",\n  \"width\": 350,\n  \"height\": 350,\n  \"autosize\": \"pad\",\n  \"signals\": [\n    {\"name\": \"startAngle\", \"value\": 0},\n    {\"name\": \"endAngle\", \"value\": 6.29},\n    {\"name\": \"padAngle\", \"value\": 0},\n    {\"name\": \"sort\", \"value\": true},\n    {\"name\": \"strokeWidth\", \"value\": 2},\n    {\n      \"name\": \"selected\",\n      \"value\": \"\",\n      \"on\": [{\"events\": \"mouseover\", \"update\": \"datum\"}]\n    }\n  ],\n  \"data\": [\n    {\n      \"name\": \"table\",\n      \"values\": [\n        {\"continent\": \"Unverified\", \"population\": DefaultUnverifiedPopulation},\n        {\"continent\": \"Partially Verified\", \"population\": DefaultPartiallyVerifiedPopulation},\n        {\"continent\": \"Verified\", \"population\": DefaultVerifiedPopulation}\n      ],\n      \"transform\": [\n        {\n          \"type\": \"pie\",\n          \"field\": \"population\",\n          \"startAngle\": {\"signal\": \"startAngle\"},\n          \"endAngle\": {\"signal\": \"endAngle\"},\n          \"sort\": {\"signal\": \"sort\"}\n        }\n      ]\n    },\n    {\n      \"name\": \"fieldSum\",\n      \"source\": \"table\",\n      \"transform\": [\n        {\n          \"type\": \"aggregate\",\n          \"fields\": [\"population\"],\n          \"ops\": [\"sum\"],\n          \"as\": [\"sum\"]\n        }\n      ]\n    }\n  ],\n  \"legends\": [\n    {\n      \"fill\": \"color\",\n      \"title\": \"Legends\",\n      \"orient\": \"none\",\n      \"padding\": {\"value\": 10},\n      \"encode\": {\n        \"symbols\": {\"enter\": {\"fillOpacity\": {\"value\": 1}}},\n        \"legend\": {\n          \"update\": {\n            \"x\": {\n              \"signal\": \"(width / 2) + if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.1 * 0.8, if(width >= height, height, width) / 2 * 0.8)\",\n              \"offset\": 20\n            },\n            \"y\": {\"signal\": \"(height / 2)\", \"offset\": -50}\n          }\n        }\n      }\n    }\n  ],\n  \"scales\": [\n    {\"name\": \"color\", \"type\": \"ordinal\", \"range\": [\"#cf2608\", \"#ff9900\", \"#67b505\"]}\n  ],\n  \"marks\": [\n    {\n      \"type\": \"arc\",\n      \"from\": {\"data\": \"table\"},\n      \"encode\": {\n        \"enter\": {\n          \"fill\": {\"scale\": \"color\", \"field\": \"continent\"},\n          \"x\": {\"signal\": \"width / 2\"},\n          \"y\": {\"signal\": \"height / 2\"}\n        },\n        \"update\": {\n          \"startAngle\": {\"field\": \"startAngle\"},\n          \"endAngle\": {\"field\": \"endAngle\"},\n          \"cornerRadius\": {\"value\": 15},\n          \"padAngle\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, 0.015, 0.015)\"\n          },\n          \"innerRadius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 0.45, if(width >= height, height, width) / 2 * 0.5)\"\n          },\n          \"outerRadius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.05 * 0.8, if(width >= height, height, width) / 2 * 0.8)\"\n          },\n          \"opacity\": {\n            \"signal\": \"if(selected && selected.continent !== datum.continent, 1, 1)\"\n          },\n          \"stroke\": {\"signal\": \"scale('color', datum.continent)\"},\n          \"strokeWidth\": {\"signal\": \"strokeWidth\"},\n          \"fillOpacity\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, 0.8, 0.8)\"\n          }\n        }\n      }\n    },\n    {\n      \"type\": \"text\",\n      \"encode\": {\n        \"enter\": {\"fill\": {\"value\": \"#525252\"}, \"text\": {\"value\": \"\"}},\n        \"update\": {\n          \"opacity\": {\"value\": 1},\n          \"x\": {\"signal\": \"width / 2\"},\n          \"y\": {\"signal\": \"height / 2\"},\n          \"align\": {\"value\": \"center\"},\n          \"baseline\": {\"value\": \"middle\"},\n          \"fontSize\": {\"signal\": \"if(width >= height, height, width) * 0.05\"},\n          \"text\": {\"value\": \"Verification Status\"}\n        }\n      }\n    },\n    {\n      \"name\": \"mark_population\",\n      \"type\": \"text\",\n      \"from\": {\"data\": \"table\"},\n      \"encode\": {\n        \"enter\": {\n          \"text\": {\n            \"signal\": \"if(datum['endAngle'] - datum['startAngle'] < 0.3, '', format(datum['population'] / 1, '.0f'))\"\n          },\n          \"x\": {\"signal\": \"if(width >= height, height, width) / 2\"},\n          \"y\": {\"signal\": \"if(width >= height, height, width) / 2\"},\n          \"radius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.05 * 0.65, if(width >= height, height, width) / 2 * 0.65)\"\n          },\n          \"theta\": {\"signal\": \"(datum['startAngle'] + datum['endAngle'])/2\"},\n          \"fill\": {\"value\": \"#FFFFFF\"},\n          \"fontSize\": {\"value\": 12},\n          \"align\": {\"value\": \"center\"},\n          \"baseline\": {\"value\": \"middle\"}\n        }\n      }\n    }\n  ]\n}\n\n```";

    char unverifiedPopulation[10];
    sprintf(unverifiedPopulation, "%d", verificationStatusCount[0]);

    char partiallyVerifiedPopulation[10];
    sprintf(partiallyVerifiedPopulation, "%d", verificationStatusCount[1]);

    char verifiedPopulation[10];
    sprintf(verifiedPopulation, "%d", verificationStatusCount[2]);

    pieChart = replaceWord(pieChart, "DefaultUnverifiedPopulation", unverifiedPopulation);
    pieChart = replaceWord(pieChart, "DefaultPartiallyVerifiedPopulation", partiallyVerifiedPopulation);
    pieChart = replaceWord(pieChart, "DefaultVerifiedPopulation", verifiedPopulation);

    log_message(LOG_DEBUG, "Exiting function createVcdPieChart");

    return pieChart;
}

char *buildRequirementPageFromJSONRequirementList(cJSON *requirement){
    log_message(LOG_DEBUG, "Entering function buildRequirementPageFromJSONRequirementList");
    
    char *pageContent = strdup(template_REQ);

    // Get and print each item of the requirement object
    cJSON *id = cJSON_GetObjectItemCaseSensitive(requirement, "ID");
    cJSON *title = cJSON_GetObjectItemCaseSensitive(requirement, "Title");
    cJSON *description = cJSON_GetObjectItemCaseSensitive(requirement, "Description");
    cJSON *source = cJSON_GetObjectItemCaseSensitive(requirement, "Source");
    cJSON *author = cJSON_GetObjectItemCaseSensitive(requirement, "Author");
    cJSON *justification = cJSON_GetObjectItemCaseSensitive(requirement, "Justification");
    cJSON *criticality = cJSON_GetObjectItemCaseSensitive(requirement, "Criticality");
    cJSON *compliance = cJSON_GetObjectItemCaseSensitive(requirement, "Compliance");
    cJSON *verification_status = cJSON_GetObjectItemCaseSensitive(requirement, "Verification Status");
    cJSON *assignee = cJSON_GetObjectItemCaseSensitive(requirement, "Assignee");

    cJSON *path = cJSON_GetObjectItemCaseSensitive(requirement, "Path");

    

    //TITLE
    if (cJSON_IsString(id) && id->valuestring) {
        pageContent = appendStrings(pageContent, "\n# ");
        pageContent = appendStrings(pageContent, id->valuestring);
        pageContent = appendStrings(pageContent, ": ");
    }
    if (cJSON_IsString(title) && title->valuestring) {
        pageContent = appendStrings(pageContent, title->valuestring);
        pageContent = appendStrings(pageContent, "\n");
    }


    //DESCRIPTION
    if (cJSON_IsString(description) && description->valuestring) {
        pageContent = appendStrings(pageContent, ">**Description**: ");
        pageContent = appendStrings(pageContent, description->valuestring);
        pageContent = appendStrings(pageContent, "\n");
    }


    //INFORMATION BOX: SOURCES AND ASSIGNEE
    if (cJSON_IsString(source) && source->valuestring && strcmp(source->valuestring, "") != 0) {
        pageContent = appendStrings(pageContent, "\n>**Source**: ");
        pageContent = appendStrings(pageContent, source->valuestring);
        pageContent = appendStrings(pageContent, "\n");
    }
    if (cJSON_IsString(author) && author->valuestring && strcmp(author->valuestring, "") != 0) {
        pageContent = appendStrings(pageContent, ">**Author**: ");
        pageContent = appendStrings(pageContent, author->valuestring);
        pageContent = appendStrings(pageContent, "\n");
    }
    if (cJSON_IsString(assignee) && assignee->valuestring && strcmp(assignee->valuestring, "") != 0) {
        pageContent = appendStrings(pageContent, ">**Assignee**: ");
        pageContent = appendStrings(pageContent, assignee->valuestring);
        pageContent = appendStrings(pageContent, "\n");
    }
    if (strcmp(source->valuestring, "") != 0 || strcmp(author->valuestring, "") != 0 || strcmp(assignee->valuestring, "") != 0) {
        pageContent = appendStrings(pageContent, "{.is-info}\n");
    }


    //JUSTIFICATION
    if (cJSON_IsString(justification) && justification->valuestring && strcmp(justification->valuestring, "") != 0) {
        pageContent = appendStrings(pageContent, "\n## Justification\n");
        pageContent = appendStrings(pageContent, justification->valuestring);
        pageContent = appendStrings(pageContent, "\n");
    }


    //COMPLIANCE
    if (cJSON_IsString(compliance) && compliance->valuestring && strcmp(compliance->valuestring, REQ_SHEET_EMPTY_VALUE) != 0) {
        
        if(strcmp(compliance->valuestring, "Compliant") == 0){
            pageContent = appendStrings(pageContent, "\n# Compliance\n");
            pageContent = appendStrings(pageContent, ":green_circle: Compliant\n");
        }
        if(strcmp(compliance->valuestring, "Unknown") == 0){
            pageContent = appendStrings(pageContent, "\n# Compliance\n");
            pageContent = appendStrings(pageContent, ":orange_circle: Unknown\n");
        }
        if(strcmp(compliance->valuestring, "Uncompliant") == 0){
            pageContent = appendStrings(pageContent, "\n# Compliance\n");
            pageContent = appendStrings(pageContent, ":red_circle: Uncompliant\n");
        }
    }


    //CRITICALITY
    if (cJSON_IsString(criticality) && criticality->valuestring && strcmp(criticality->valuestring, REQ_SHEET_EMPTY_VALUE) != 0) {
        
        if(strcmp(criticality->valuestring, "Low") == 0){
            pageContent = appendStrings(pageContent, "\n# Criticality\n");
            pageContent = appendStrings(pageContent, ":green_circle: Low\n");
        }
        if(strcmp(criticality->valuestring, "Medium") == 0){
            pageContent = appendStrings(pageContent, "\n# Criticality\n");
            pageContent = appendStrings(pageContent, ":orange_circle: Medium\n");
        }
        if(strcmp(criticality->valuestring, "High") == 0){
            pageContent = appendStrings(pageContent, "\n# Criticality\n");
            pageContent = appendStrings(pageContent, ":red_circle: High\n");
        }
    }



    //VERIFICAITON
    int isVerification = 0;
    int verificationCount = 0;

    log_message(LOG_DEBUG, "Going to start filling in the verification section");

    for(int reviewNumber = 1; reviewNumber <= MAXIMUM_NUMBER_OF_REVIEWS; reviewNumber++){
        for (int verificationNumber = 1; verificationNumber <= MAXIMUM_NUMBER_OF_VERIFICATIONS_PER_REVIEW; verificationNumber++){

            char JsonItemNameMethod[1024];
            snprintf(JsonItemNameMethod, sizeof(JsonItemNameMethod), "Review %d Verification %d Method", reviewNumber, verificationNumber);

            char JsonItemNameStatus[1024];
            snprintf(JsonItemNameStatus, sizeof(JsonItemNameStatus), "Review %d Verification %d Status", reviewNumber, verificationNumber);

            cJSON *verificationMethod = cJSON_GetObjectItemCaseSensitive(requirement, JsonItemNameMethod);
            cJSON *verificationStatus = cJSON_GetObjectItemCaseSensitive(requirement, JsonItemNameStatus);

            char *reviewName = REVIEW_1_NAME;

            switch(reviewNumber) {
                case 1: reviewName = REVIEW_1_NAME; break;
                case 2: reviewName = REVIEW_2_NAME; break;
                case 3: reviewName = REVIEW_3_NAME; break;
                case 4: reviewName = REVIEW_4_NAME; break;
                case 5: reviewName = REVIEW_5_NAME; break;
                case 6: reviewName = REVIEW_6_NAME; break;
            }
            

            if(strcmp(verificationMethod->valuestring, REQ_SHEET_EMPTY_VALUE) != 0){

                log_message(LOG_DEBUG, "Verification method found");
                
                verificationCount++;

                if(isVerification == 0){
                    pageContent = appendStrings(pageContent, "\n# Verification");
                    isVerification = 1;
                }

                char temp_verificationNumber[100];
                snprintf(temp_verificationNumber, sizeof(temp_verificationNumber), "\n## Verification %d\n", verificationCount);
                pageContent = appendStrings(pageContent, temp_verificationNumber);

                pageContent = appendStrings(pageContent, "**Method**: ");
                pageContent = appendStrings(pageContent, verificationMethod->valuestring);
                pageContent = appendStrings(pageContent, "\n**Deadline**: ");
                pageContent = appendStrings(pageContent, reviewName);
                pageContent = appendStrings(pageContent, "\n");

                if(strcmp(verificationStatus->valuestring, REQ_SHEET_EMPTY_VALUE) != 0){
                    pageContent = appendStrings(pageContent, "**Status**: ");

                    if(strcmp(verificationStatus->valuestring, "Completed") == 0){pageContent = appendStrings(pageContent, ":green_circle:");}
                    if(strcmp(verificationStatus->valuestring, "In progress") == 0){pageContent = appendStrings(pageContent, ":orange_circle:");}
                    if(strcmp(verificationStatus->valuestring, "Uncompleted") == 0){pageContent = appendStrings(pageContent, ":red_circle:");}

                    pageContent = appendStrings(pageContent, verificationStatus->valuestring);
                    pageContent = appendStrings(pageContent, "\n");

                }

                log_message(LOG_DEBUG, "Page content: %s",  pageContent);

            }

        }
    }
    
    log_message(LOG_DEBUG, "Exiting function buildRequirementPageFromJSONRequirementList");

    return pageContent;
}

char *buildDrlFromJSONRequirementList(cJSON *requirementList, char* subSystem){
    log_message(LOG_DEBUG, "Entering function buildDrlFromJSONRequirementList");
    
    // Get the requirements array from the requirementList object
    cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");
    if (!cJSON_IsArray(requirements)) {
        log_message(LOG_ERROR, "Error: requirements is not a JSON array");
    }

    char *DRL = strdup(template_DRL);

    int isFirstGroup = 1;

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
        cJSON *description = cJSON_GetObjectItemCaseSensitive(requirement, "Description");

        if(strlen(id->valuestring) < 2){
            log_message(LOG_DEBUG, "ID is smaller than one, breaking");
            break;
        }
        
        if(title == NULL){
            log_message(LOG_DEBUG, "Found a new group");

            if(!isFirstGroup){
                DRL = appendStrings(DRL, "{.links-list}");
                isFirstGroup = 0;
            }
            else{isFirstGroup = 0;}

            DRL = appendStrings(DRL, "\n\n\n## ");
            DRL = appendStrings(DRL, id->valuestring);
            DRL = appendStrings(DRL, "\n");
            continue;
        }

        if (cJSON_IsString(id) && id->valuestring) {
            log_message(LOG_DEBUG, "ID: %s", id->valuestring);
            DRL = appendStrings(DRL, "- [");
            DRL = appendStrings(DRL, id->valuestring);
            DRL = appendStrings(DRL, "](/");
            DRL = appendStrings(DRL, "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_");
            DRL = appendStrings(DRL, subSystem);
            DRL = appendStrings(DRL, "_DRL/");
            DRL = appendStrings(DRL, id->valuestring);
            DRL = appendStrings(DRL, ") **");
        }
        
        
        
        if (cJSON_IsString(title) && title->valuestring) {
            log_message(LOG_DEBUG, "title: %s", title->valuestring);
            DRL = appendStrings(DRL, title->valuestring);
            DRL = appendStrings(DRL, "**\n");
        }
        if (cJSON_IsString(description) && description->valuestring) {
            log_message(LOG_DEBUG, "Description: %s", description->valuestring);
            DRL = appendStrings(DRL, description->valuestring);
            DRL = appendStrings(DRL, "\n");
        }

    }

    DRL = appendStrings(DRL, "{.links-list}");
    
    log_message(LOG_DEBUG, "Exiting function buildDrlFromJSONRequirementList");

    return DRL;

}

cJSON *parseArrayIntoJSONRequirementList(char *input_str) {
    log_message(LOG_DEBUG, "Entering function parseArrayIntoJSONRequirementList");
    

    //log_message(LOG_DEBUG, "input_str: %s\n", input_str);

    // Parse the input string as JSON
    cJSON *input_json = cJSON_Parse(input_str);
    if (!input_json) {
        log_message(LOG_ERROR, "Error parsing input string as JSON object");
        return NULL;
    }

    // Extract the "values" array from the JSON object
    cJSON *values_array = cJSON_GetObjectItemCaseSensitive(input_json, "values");
    if (!cJSON_IsArray(values_array)) {
        log_message(LOG_ERROR, "Error: values is not a JSON array");
        cJSON_Delete(input_json);
        return NULL;
    }

    // Create a JSON object to hold the requirements
    cJSON *json = cJSON_CreateObject();
    if (!json) {
        log_message(LOG_ERROR, "Error creating JSON object");
        cJSON_Delete(input_json);
        return NULL;
    }

    // Create a JSON array to hold the requirement objects
    cJSON *requirements = cJSON_CreateArray();
    if (!requirements) {
        log_message(LOG_ERROR, "Error creating JSON array");
        cJSON_Delete(json);
        cJSON_Delete(input_json);
        return NULL;
    }

    // Add the requirements array to the JSON object
    cJSON_AddItemToObject(json, "requirements", requirements);

    log_message(LOG_DEBUG, "about to parse requirements from value array into cJSON object");

    // Iterate over the input array of arrays and create JSON objects for each requirement
    int num_reqs = cJSON_GetArraySize(values_array);
    for (int i = 0; i < num_reqs; i++) {
        
        cJSON *req_array = cJSON_GetArrayItem(values_array, i);
        
    
        // Create a JSON object for the current requirement
        cJSON *req = cJSON_CreateObject();
        if (!req) {
            log_message(LOG_ERROR, "Error creating JSON object for requirement");
            cJSON_Delete(json);
            cJSON_Delete(input_json);
            return NULL;
        }

        if(cJSON_GetArrayItem(req_array, REQ_ID_COL) == NULL || strcmp(cJSON_GetArrayItem(req_array, REQ_ID_COL)->valuestring, "")==0){
            log_message(LOG_DEBUG, "ID is empty");
            break;
        }

        // Add the fields to the requirement JSON object
        cJSON_AddStringToObject(req, "ID", cJSON_GetArrayItem(req_array, REQ_ID_COL)->valuestring);
        log_message(LOG_DEBUG, "About to parse the rest of %s", cJSON_GetArrayItem(req_array, REQ_ID_COL)->valuestring);

        if(cJSON_GetArrayItem(req_array, REQ_TITLE_COL) == NULL){
            log_message(LOG_DEBUG, "Title is empty");
            cJSON_AddItemToArray(requirements, req);
            continue;
        }

        
        cJSON_AddStringToObject(req, "Title", cJSON_GetArrayItem(req_array, REQ_TITLE_COL)->valuestring);
        cJSON_AddStringToObject(req, "Description", cJSON_GetArrayItem(req_array, REQ_DESCRIPTION_COL)->valuestring);
        cJSON_AddStringToObject(req, "Source", cJSON_GetArrayItem(req_array, REQ_SOURCE_COL)->valuestring);
        cJSON_AddStringToObject(req, "Author", cJSON_GetArrayItem(req_array, REQ_AUTHOR_COL)->valuestring);
        cJSON_AddStringToObject(req, "Justification", cJSON_GetArrayItem(req_array, REQ_JUSTIFICATION_COL)->valuestring);
        cJSON_AddStringToObject(req, "Criticality", cJSON_GetArrayItem(req_array, REQ_CRITICALITY_COL)->valuestring);
        cJSON_AddStringToObject(req, "Compliance", cJSON_GetArrayItem(req_array, REQ_COMPLIANCE_COL)->valuestring);
        cJSON_AddStringToObject(req, "Verification Status", cJSON_GetArrayItem(req_array, REQ_VERIFICATION_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Assignee", cJSON_GetArrayItem(req_array, REQ_ASSIGNEE_COL)->valuestring);
        
        cJSON_AddStringToObject(req, "Review 1 Verification 1 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_1_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 1 Verification 1 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_1_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 1 Verification 2 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_2_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 1 Verification 2 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_2_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 1 Verification 3 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_3_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 1 Verification 3 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_3_STATUS_COL)->valuestring);
        
        cJSON_AddStringToObject(req, "Review 2 Verification 1 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_1_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 2 Verification 1 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_1_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 2 Verification 2 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_2_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 2 Verification 2 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_2_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 2 Verification 3 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_3_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 2 Verification 3 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_3_STATUS_COL)->valuestring);
        
        cJSON_AddStringToObject(req, "Review 3 Verification 1 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_1_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 3 Verification 1 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_1_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 3 Verification 2 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_2_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 3 Verification 2 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_2_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 3 Verification 3 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_3_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 3 Verification 3 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_3_STATUS_COL)->valuestring);

        cJSON_AddStringToObject(req, "Review 4 Verification 1 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_1_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 4 Verification 1 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_1_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 4 Verification 2 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_2_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 4 Verification 2 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_2_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 4 Verification 3 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_3_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 4 Verification 3 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_3_STATUS_COL)->valuestring);

        cJSON_AddStringToObject(req, "Review 5 Verification 1 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_1_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 5 Verification 1 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_1_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 5 Verification 2 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_2_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 5 Verification 2 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_2_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 5 Verification 3 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_3_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 5 Verification 3 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_3_STATUS_COL)->valuestring);

        cJSON_AddStringToObject(req, "Review 6 Verification 1 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_1_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 6 Verification 1 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_1_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 6 Verification 2 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_2_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 6 Verification 2 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_2_STATUS_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 6 Verification 3 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_3_METHOD_COL)->valuestring);
        cJSON_AddStringToObject(req, "Review 6 Verification 3 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_3_STATUS_COL)->valuestring);

        //cJSON_AddStringToObject(req, "Path", cJSON_GetArrayItem(req_array, REQ_PATH_COL)->valuestring);
        

        // Add the requirement object to the requirements array
        cJSON_AddItemToArray(requirements, req);

        log_message(LOG_DEBUG, "Finished parsing array");
    }

    
    log_message(LOG_DEBUG, "Exiting function parseArrayIntoJSONRequirementList");

    return json;
}

char* parseJSONRequirementListInToArray(cJSON* requirements){
    log_message(LOG_DEBUG, "Entering function parseJSONRequirementListInToArray");
    

    // Get the number of requirements
    int req_count = cJSON_GetArraySize(requirements);

    // Calculate the required buffer size for the output string
    size_t buffer_size = 1024; // Initial size, will be adjusted as needed
    for (int i = 0; i < req_count; i++) {
        cJSON *req = cJSON_GetArrayItem(requirements, i);
        const char *id = cJSON_GetObjectItem(req, "id")->valuestring;
        const char *path = cJSON_GetObjectItem(req, "path")->valuestring;
        const char *name = cJSON_GetObjectItem(req, "name")->valuestring;
        const char *description = cJSON_GetObjectItem(req, "description")->valuestring;
        buffer_size += strlen(id) + strlen(path) + strlen(name) + strlen(description) + 20; // Adding extra space for formatting
    }

    // Allocate memory for the output string
    char *output_str = (char *)malloc(buffer_size * sizeof(char));
    if (!output_str) {
        log_message(LOG_ERROR, "Memory allocation failed");
        return NULL;
    }
    output_str[0] = '\0'; // Initialize the string

    // Build the output string
    strcat(output_str, "[");
    for (int i = 0; i < req_count; i++) {
        cJSON *req = cJSON_GetArrayItem(requirements, i);
        const char *id = cJSON_GetObjectItem(req, "id")->valuestring;
        const char *path = cJSON_GetObjectItem(req, "path")->valuestring;
        const char *name = cJSON_GetObjectItem(req, "name")->valuestring;
        const char *description = cJSON_GetObjectItem(req, "description")->valuestring;

        char req_str[1000]; // Buffer for the current requirement string
        snprintf(req_str, sizeof(req_str), "[\"%s\", \"%s\", \"%s\"]", id, name, description);

        if (i > 0) {
            strcat(output_str, ", ");
        }
        strcat(output_str, req_str);
    }
    strcat(output_str, "]");

    // Clean up the JSON object

    log_message(LOG_DEBUG, "Exiting function parseJSONRequirementListInToArray");
    return output_str;

}

void addRequirementToCjsonObject(cJSON *requirements, char *idStr, char *pathStr, char *nameStr, char *descriptionStr){
    log_message(LOG_DEBUG, "Entering function addRequirementToCjsonObject");
    
    cJSON *id = NULL;
    cJSON *path = NULL;
    cJSON *name = NULL;
    cJSON *description = NULL;
    cJSON *requirement = NULL;

    requirement = cJSON_CreateObject();

    id = cJSON_CreateString(idStr);
    path = cJSON_CreateString(pathStr);
    name = cJSON_CreateString(nameStr);
    description = cJSON_CreateString(descriptionStr);

    cJSON_AddItemToArray(requirements, requirement);

    cJSON_AddItemToObject(requirement, "id", id);
    cJSON_AddItemToObject(requirement, "path", path);
    cJSON_AddItemToObject(requirement, "name", name);
    cJSON_AddItemToObject(requirement, "description", description);

    log_message(LOG_DEBUG, "Exiting function addRequirementToCjsonObject");

}

void parseRequirementsList(cJSON* requirements, char *content) {
    log_message(LOG_DEBUG, "Entering function parseRequirementsList");
    
    char *idStartFlag = "\\n- [";
    char *pathStartFlag = "](/";
    char *nameStartFlag = ") **";
    char *descriptionStartFlag = "**\\n";
    char *descriptionEndFlag = "\\n";
    char *groupStartFlag ="\\n## ";
    char *groupEndFlag = idStartFlag;
    char *ptr = content;
    int isLastGroup = 0;


    while (*ptr) {
    
        char *idStart = strstr(ptr, idStartFlag);
        if (idStart == NULL){
            log_message(LOG_DEBUG, "did not find idStart, Breaking");
            break; // No more links
        }

        
        char *groupStart = strstr(ptr, groupStartFlag);
        if (groupStart == NULL){
            log_message(LOG_DEBUG, "did not find groupStart, setting isLastGroup to 1");
            isLastGroup = 1;
        }
        if (groupStart < idStart && isLastGroup == 0){
            int groupLength = idStart - groupStart - strlen(groupStartFlag);
            char *group = (char *)malloc((groupLength + 1) * sizeof(char));
            strncpy(group, groupStart + strlen(groupStartFlag), groupLength);
            group[groupLength] = '\0';
            addRequirementToCjsonObject(requirements, group, "", "", "");

            ptr = idStart;

            continue;
        }

        char *pathStart = strstr(idStart, pathStartFlag);
        if (pathStart == NULL){
            log_message(LOG_DEBUG, "did not find pathStart, Breaking");
            break; // No more links
        }

        char *nameStart = strstr(pathStart, nameStartFlag);
        if (nameStart == NULL){
            log_message(LOG_DEBUG, "did not find nameStart, Breaking");
            break; // No more links
        }

        char *descriptionStart = strstr(nameStart, descriptionStartFlag);
        if (descriptionStart == NULL){
            log_message(LOG_DEBUG, "did not find descriptionStart, Breaking");
            break; // No more links
        }

        char *descriptionEnd = strstr(descriptionStart + strlen(descriptionStartFlag), descriptionEndFlag);
        if (descriptionEnd == NULL){
            log_message(LOG_DEBUG, "did not find descriptionEnd, Breaking");
            break; // No more links
        }

        // Calculate title length and link length
        int idLength = pathStart - idStart - strlen(idStartFlag);
        int pathLength = nameStart - pathStart - strlen(pathStartFlag);
        int nameLength = descriptionStart - nameStart - strlen(nameStartFlag);
        int descriptionLength = descriptionEnd - descriptionStart - strlen(descriptionStartFlag);

        if (idLength <= 0 || pathLength <= 0 || nameLength <= 0 || descriptionLength <= 0) {
            ptr = descriptionEnd + strlen(descriptionEndFlag);
            continue;
        }

        // Allocate memory for the title and link
        char *id = (char *)malloc((idLength + 1) * sizeof(char));
        char *path = (char *)malloc((pathLength + 1) * sizeof(char));
        char *name = (char *)malloc((nameLength + 1) * sizeof(char));
        char *description = (char *)malloc((descriptionLength + 1) * sizeof(char));
        if (!id || !path || !name || !description) {
            log_message(LOG_ERROR, "Memory allocation error");
            if (id) free(id);
            if (path) free(path);
            if (name) free(name);
            if (description) free(description);
        }

        // Copy title and link
        strncpy(id, idStart + strlen(idStartFlag), idLength);
        id[idLength] = '\0';
        strncpy(path, pathStart + strlen(pathStartFlag), pathLength);
        path[pathLength] = '\0';
        strncpy(name, nameStart + strlen(nameStartFlag), nameLength);
        name[nameLength] = '\0';
        strncpy(description, descriptionStart + strlen(descriptionStartFlag), descriptionLength);
        description[descriptionLength] = '\0';

        log_message(LOG_DEBUG, "adding requirement with id: %s\npath: %s\nname: %s", id, path, name);
        addRequirementToCjsonObject(requirements, id, path, name, description);


        free(id);
        free(path);
        free(name);
        free(description);
        ptr = descriptionEnd;
    }
    
    log_message(LOG_DEBUG, "Exiting function parseRequirementsList");
    return;
}

void countVerificationStatus(cJSON *requirementList, int* verificationStatusCount){
    log_message(LOG_DEBUG, "Entering function countVerificationStatus");
    
    // Get the requirements array from the requirementList object
    cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");
    if (!cJSON_IsArray(requirements)) {
        log_message(LOG_ERROR, "Error: requirements is not a JSON array");
    }

    int verifiedCount = 0;
    int partiallyCount = 0;
    int uncompleteCount = 0;

    // Iterate over each requirement object in the requirements array
    int num_reqs = cJSON_GetArraySize(requirements);
    for (int i = 0; i < num_reqs; i++) {
        cJSON *requirement = cJSON_GetArrayItem(requirements, i);
        if (!cJSON_IsObject(requirement)) {
            log_message(LOG_ERROR, "Error: requirement is not a JSON object");
            continue;
        }

        // Get and print each item of the requirement object
        cJSON *R1V1S = cJSON_GetObjectItemCaseSensitive(requirement, "Review 1 Verification 1 Status");
        cJSON *R1V2S = cJSON_GetObjectItemCaseSensitive(requirement, "Review 1 Verification 2 Status");
        cJSON *R1V3S = cJSON_GetObjectItemCaseSensitive(requirement, "Review 1 Verification 3 Status");
        cJSON *title = cJSON_GetObjectItemCaseSensitive(requirement, "Title");

        int countNA  = 0;
        int countCompleted  = 0;
        int countUncompleted  = 0;

        if(title == NULL){
            log_message(LOG_DEBUG, "Found a new group");
            continue;
        }

        for(int i = 0; i < 3; i++){

            cJSON *tempStatus = NULL;
            

            switch (i)
            {
            case 0:
                tempStatus = R1V1S;
                break;
            
            case 1:
                tempStatus = R1V2S;
                break;
            
            case 2:
                tempStatus = R1V3S;
                break;
            
            default:
                break;
            }

            log_message(LOG_DEBUG, "tempStatus string values: %s", tempStatus->valuestring);

            if (strcmp(tempStatus->valuestring, "N/A") == 0){
                countNA++ ;
            }

            if (strcmp(tempStatus->valuestring, "Completed") == 0){
                countCompleted++ ;
            }

            if (strcmp(tempStatus->valuestring, "Uncompleted") == 0){
                countUncompleted++ ;
            }

        }

        if(countUncompleted == 0 && countCompleted == 0){
            continue;
        }

        if(countUncompleted == 0 && countCompleted != 0){
            verifiedCount++;
        }

        if(countUncompleted != 0 && countCompleted != 0){
            partiallyCount++;
        }

        if(countUncompleted != 0 && countCompleted == 0){
            uncompleteCount++;
        }

    }

    log_message(LOG_DEBUG, "uncompleteCount: %d", uncompleteCount);
    log_message(LOG_DEBUG, "partiallyCount: %d", partiallyCount);
    log_message(LOG_DEBUG, "verifiedCount: %d", verifiedCount);

    verificationStatusCount[0] = uncompleteCount;
    verificationStatusCount[1] = partiallyCount;
    verificationStatusCount[2] = verifiedCount;
    
    return;
}

char *buildVcdList(cJSON *requirementList, char* subSystem){
    log_message(LOG_DEBUG, "Entering function buildVcdList");
    
    // Get the requirements array from the requirementList object
    cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");
    if (!cJSON_IsArray(requirements)) {
        log_message(LOG_ERROR, "Error: requirements is not a JSON array");
    }

    char *VCD = "\n# Verification Status for next Review\n\n# {.tabset}";

    char *UVROD = "\n### Review Of Design\n";
    char *IPVROD = "\n### Review Of Design\n";
    char *VROD  = "\n### Review Of Design\n";

    char *UVTE = "\n### Test\n";
    char *IPVTE = "\n### Test\n";
    char *VTE  = "\n### Test\n";

    char *UVIN = "\n### Inspection\n";
    char *IPVIN = "\n### Inspection\n";
    char *VIN  = "\n### Inspection\n";

    char *UVAN = "\n### Analysis\n";
    char *IPVAN = "\n### Analysis\n";
    char *VAN  = "\n### Analysis\n";
    

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
        
        if(title == NULL){
            log_message(LOG_DEBUG, "Found a new group");
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
                *targetList = appendStrings(*targetList, "- [");
                *targetList = appendStrings(*targetList, id->valuestring);
                *targetList = appendStrings(*targetList, "](/");
                *targetList = appendStrings(*targetList, "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_");
                *targetList = appendStrings(*targetList, subSystem);
                *targetList = appendStrings(*targetList, "_DRL/");
                *targetList = appendStrings(*targetList, id->valuestring);
                *targetList = appendStrings(*targetList, ") **");
            }
        
            if (cJSON_IsString(title) && title->valuestring) {
                log_message(LOG_DEBUG, "title: %s", title->valuestring);
                *targetList = appendStrings(*targetList, title->valuestring);
                *targetList = appendStrings(*targetList, "**\n");
            }

            if (cJSON_IsString(assignee) && assignee->valuestring) {
                log_message(LOG_DEBUG, "assignee: %s", assignee->valuestring);
                *targetList = appendStrings(*targetList, "**Assignee**: ");
                *targetList = appendStrings(*targetList, assignee->valuestring);
                *targetList = appendStrings(*targetList, "\n");
            }

            log_message(LOG_DEBUG, "targetList string value: %s", targetList);



        }


    }


    UVROD =  appendStrings(UVROD, "\n{.links-list}");
    IPVROD =  appendStrings(IPVROD, "\n{.links-list}");
    VROD  =  appendStrings(VROD, "\n{.links-list}");

    UVTE =  appendStrings(UVTE, "\n{.links-list}");
    IPVTE =  appendStrings(IPVTE, "\n{.links-list}");
    VTE  =  appendStrings(VTE, "\n{.links-list}");

    UVIN =  appendStrings(UVIN, "\n{.links-list}");
    IPVIN =  appendStrings(IPVIN, "\n{.links-list}");
    VIN  =  appendStrings(VIN, "\n{.links-list}");

    UVAN =  appendStrings(UVAN, "\n{.links-list}");
    IPVAN =  appendStrings(IPVAN, "\n{.links-list}");
    VAN  =  appendStrings(VAN, "\n{.links-list}");

    VCD = appendStrings(VCD, "\n## Unverified Requirements\n");
    VCD = appendStrings(VCD, UVROD);
    VCD = appendStrings(VCD, UVTE);
    VCD = appendStrings(VCD, UVIN);
    VCD = appendStrings(VCD, UVAN);

    VCD = appendStrings(VCD, "\n## Verification In Progress\n");
    VCD = appendStrings(VCD, IPVROD);
    VCD = appendStrings(VCD, IPVTE);
    VCD = appendStrings(VCD, IPVIN);
    VCD = appendStrings(VCD, IPVAN);

    VCD = appendStrings(VCD, "\n## Verified Requirements\n");
    VCD = appendStrings(VCD, VROD);
    VCD = appendStrings(VCD, VTE);
    VCD = appendStrings(VCD, VIN);
    VCD = appendStrings(VCD, VAN);
    
    log_message(LOG_DEBUG, "Exiting function buildVcdList");
    return VCD;

}