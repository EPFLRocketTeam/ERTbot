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
    refreshOAuthToken();
    char *sheetId;
    pageList* requirementPagesHead = NULL;
    char *path;

    path = cmd.argument_1;

    if(strcmp(cmd.argument_1, "ST")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_ST_DRL/";
        sheetId = "ST!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "PR")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/";
        sheetId = "PR!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "FD")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_FD_DRL/";
        sheetId = "FD!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "RE")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_RE_DRL/";
        sheetId = "RE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "GS")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_GS_DRL/";
        sheetId = "GS!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "AV")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_AV_DRL/";
        sheetId = "AV!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "TE")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_TE_DRL/";
        sheetId = "TE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "PL")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PL_DRL/";
        sheetId = "PL!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "GE")==0){
        path = "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_GE_DRL/";
        sheetId = "GE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "UT")==0){
        path = "management/it/ERTbot_Test_Pages/";
        sheetId = "UT2!A3:AT300";
    }

    requirementPagesHead = populatePageList(&requirementPagesHead, "path", path);
    batchGetSheet("1i_PTwIqLuG9IUI73UaGuOvx8rVTDV1zIS7gmXNjMs1I", sheetId);

    cJSON *requirementList = parseArrayIntoJSONRequirementList(chunk.response);

    // Get the requirements array from the requirementList object
    cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");
    if (!cJSON_IsArray(requirements)) {
        log_message(LOG_ERROR, "Error: requirements is not a JSON array");
    }

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
            reqContent = appendToString(reqContent, reqContent);
            log_message(LOG_DEBUG, "About to create new page path:%s\nTitle:%s", reqPath, id->valuestring);
            createPageMutation(reqPath, reqContent, id->valuestring);

            free(reqPath);
            free(reqContent);
        }

    }

    cJSON_Delete(requirementList);
    freePageList(&requirementPagesHead);

    log_message(LOG_DEBUG, "Exiting function createMissingRequirementPages");
    return;
}
