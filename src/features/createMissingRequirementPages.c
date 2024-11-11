#include <string.h>
#include <cjson/cJSON.h>
#include "ERTbot_common.h"
#include "sheetAPI.h"
#include "wikiAPI.h"
#include "requirementsHelpers.h"
#include "stringHelpers.h"
#include "slackAPI.h"
#include "pageListHelpers.h"

void createMissingRequirementPages(command cmd){
    log_message(LOG_DEBUG, "Entering function createMissingRequirementPages");

    pageList* requirementPagesHead = NULL;

    cJSON* subsystem = getSubsystemInfo(cmd.argument);
    const char *path = cJSON_GetObjectItem(subsystem, "Requirement Pages Directory")->valuestring;
    const char *sheetId = cJSON_GetObjectItem(subsystem, "Req_DB Sheet Acronym and Range")->valuestring;
    const char *reqDbId = cJSON_GetObjectItem(subsystem, "Req_DB Spreadsheet ID")->valuestring;

    requirementPagesHead = populatePageList(&requirementPagesHead, "path", path);
    batchGetSheet(reqDbId, sheetId);

    cJSON *requirementList = parseArrayIntoJSONRequirementList(chunk.response);

    // Get the requirements array from the requirementList object
    const cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");
    if (!cJSON_IsArray(requirements)) {
        log_message(LOG_ERROR, "Error: requirements is not a JSON array");
    }

    // Iterate over each requirement object in the requirements array
    int num_reqs = cJSON_GetArraySize(requirements);

    for (int i = 0; i < num_reqs; i++) {
        const cJSON *requirement = cJSON_GetArrayItem(requirements, i);

        if (!cJSON_IsObject(requirement)) {
            log_message(LOG_ERROR, "Error: requirement is not a JSON object");
            continue;
        }

        // Get and print each item of the requirement object
        cJSON *id = cJSON_GetObjectItemCaseSensitive(requirement, "ID");
        pageList* currentReqPage = requirementPagesHead;
        int foundPage = 0;

        log_message(LOG_DEBUG, "Looking for page corresponding to requiremet: %s", id->valuestring);

        if(!strstr(id->valuestring, "2024_")){
            log_message(LOG_DEBUG, "Found a group, skipping");
            continue;
        }

        while(currentReqPage){
            if (cJSON_IsString(id) && id->valuestring && strcmp(id->valuestring, currentReqPage->title) == 0){
                log_message(LOG_DEBUG, "Found %s, breaking", currentReqPage->title);
                foundPage = 1;
                break;
            }

            else{
                currentReqPage = currentReqPage->next;
            }
        }

        if (foundPage == 0){
            char *reqPath = createCombinedString(path, id->valuestring);
            char *reqContent = "<!--";
            reqContent = createCombinedString(reqContent, id->valuestring);
            reqContent = appendToString(reqContent, "-->\\\\n");
            reqContent = appendToString(reqContent, "<!--");
            reqContent = appendToString(reqContent, id->valuestring);
            reqContent = appendToString(reqContent, "-->");
            log_message(LOG_DEBUG, "About to create new page path:%s\nTitle:%s", reqPath, id->valuestring);
            createPageMutation(reqPath, reqContent, id->valuestring);

            free(reqPath);
            free(reqContent);
        }

    }

    cJSON_Delete(requirementList);
    cJSON_Delete(subsystem);
    freePageList(&requirementPagesHead);

    log_message(LOG_DEBUG, "Exiting function createMissingRequirementPages");
    return;
}
