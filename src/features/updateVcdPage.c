#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <cjson/cJSON.h>
#include "sheetAPI.h"
#include "ERTbot_common.h"
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

/**
 * @brief Updates a JSON string representing a stacked area chart with new weekly data.
 *
 * This function updates a JSON string that contains data for a stacked area chart by adding new data points for the specified week. The new data points include verified, partially verified, and unverified values.
 *
 * @param json_str A string containing the JSON data of the existing chart. This JSON is expected to have a "data" object with a "values" array where new entries will be added.
 * @param week A string representing the week for which the data is being added. This value will be included in each new data entry.
 * @param verifiedValue An integer representing the value for the "Verified" status for the given week.
 * @param partiallyVerifiedValue An integer representing the value for the "Partially Verified" status for the given week.
 * @param unverifiedValue An integer representing the value for the "Unverified" status for the given week.
 *
 * @return A dynamically allocated string containing the updated JSON data, with new data points added to the "values" array. The caller is responsible for freeing this string.
 *
 * @details
 * - The function parses the input JSON string and retrieves the "values" array from the "data" object.
 * - Three new JSON objects are created, each representing one of the statuses ("Verified", "Partially Verified", "Unverified") with their corresponding values and the specified week.
 * - These new JSON objects are added to the "values" array.
 * - The updated JSON structure is converted back to a string and returned.
 * - If any error occurs during parsing or updating, the function returns `NULL`.
 */
static char *updateVcdStackedAreaChart(char *json_str, char *week, int verifiedValue, int partiallyVerifiedValue, int unverifiedValue);


static void countVerificationStatus(cJSON *requirementList, int* verificationStatusCount);


static char *buildVcdList(cJSON *requirementList, char* subSystem);

void updateVcdPage(command cmd){
    log_message(LOG_DEBUG, "Entering function updateVcdPage");

    refreshOAuthToken();

    char *sheetId;
    char *vcdPageId;

    if(strcmp(cmd.argument_1, "ST")==0){
        vcdPageId = "1186";
        sheetId = "ST!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "PR")==0){
        vcdPageId = "1187";
        sheetId = "PR!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "FD")==0){
        vcdPageId = "1182";
        sheetId = "FD!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "RE")==0){
        vcdPageId = "1185";
        sheetId = "RE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "GS")==0){
        vcdPageId = "1183";
        sheetId = "GS!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "AV")==0){
        vcdPageId = "1181";
        sheetId = "AV!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "TE")==0){
        vcdPageId =
        sheetId = "TE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "PL")==0){
        vcdPageId = "1184";
        sheetId = "PL!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "GE")==0){
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
    char *listOfRequirements = buildVcdList(requirementList, cmd.argument_1);
    VCD = appendToString(VCD, listOfRequirements);

    free(listOfRequirements);

    pageList* vcdPage = NULL;
    vcdPage = addPageToList(&vcdPage, vcdPageId, "", "", "", VCD, "", "", "");

    vcdPage->content = replaceWord(vcdPage->content, "\n", "\\\\n");
    vcdPage->content = replaceWord(vcdPage->content, "\"", "\\\\\\\"");

    updatePageContentMutation(vcdPage);
    renderMutation(&vcdPage, false);

    cJSON_Delete(requirementList);
    freePageList(&vcdPage);

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

        for(int j = 0; j < 3; j++){

            cJSON *tempStatus = NULL;


            switch (j)
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

static char *createVcdPieChart(const int* verificationStatusCount){
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

static char *updateVcdStackedAreaChart(char *json_str, char *week, int verifiedValue, int partiallyVerifiedValue, int unverifiedValue) {
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
