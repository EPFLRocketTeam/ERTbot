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
    log_function_entry(__func__);

    pageList* requirementPagesHead = NULL;

    updateCommandStatusMessage("fetching subsystem info");
    cJSON* subsystem = getSubsystemInfo(cmd.argument);
    const char *path = cJSON_GetObjectItem(subsystem, "Requirement Pages Directory")->valuestring;

    updateCommandStatusMessage("fetching existing requirements pages");
    requirementPagesHead = populatePageList(&requirementPagesHead, "path", path);

    updateCommandStatusMessage("fetching requirements");
    cJSON *requirementList = getRequirements(subsystem);
    
    // Get the requirements array from the requirementList object
    const cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");
    if (!cJSON_IsArray(requirements)) {
        log_message(LOG_ERROR, __func__, "Error: requirements is not a JSON array");
    }

    // Iterate over each requirement object in the requirements array
    int num_reqs = cJSON_GetArraySize(requirements);

    updateCommandStatusMessage("finding missing requirement pages");
    for (int i = 0; i < num_reqs; i++) {
        const cJSON *requirement = cJSON_GetArrayItem(requirements, i);

        if (!cJSON_IsObject(requirement)) {
            log_message(LOG_ERROR, __func__, "Error: requirement is not a JSON object");
            continue;
        }

        // Get and print each item of the requirement object
        cJSON *id = cJSON_GetObjectItemCaseSensitive(requirement, "ID");
        pageList* currentReqPage = requirementPagesHead;
        int foundPage = 0;

        log_message(LOG_DEBUG, __func__, "Looking for page corresponding to requiremet: %s", id->valuestring);

        if(!strstr(id->valuestring, "2024_")){
            log_message(LOG_DEBUG, __func__, "Found a group, skipping");
            continue;
        }

        while(currentReqPage){
            if (cJSON_IsString(id) && id->valuestring && strcmp(id->valuestring, currentReqPage->title) == 0){
                log_message(LOG_DEBUG, __func__, "Found %s, breaking", currentReqPage->title);
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
            log_message(LOG_DEBUG, __func__, "About to create new page path:%s\nTitle:%s", reqPath, id->valuestring);
            
            updateCommandStatusMessage("creating a new page");
            createPageMutation(reqPath, reqContent, id->valuestring);

            free(reqPath);
            free(reqContent);
        }

        sendLoadingBar(i, num_reqs);
    }

    cJSON_Delete(requirementList);
    cJSON_Delete(subsystem);
    freePageList(&requirementPagesHead);

    log_function_exit(__func__);
    return;
}
