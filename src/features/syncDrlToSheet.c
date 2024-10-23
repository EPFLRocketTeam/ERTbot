#include <stdbool.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "ERTbot_common.h"
#include "sheetAPI.h"
#include "requirementsHelpers.h"
#include "stringHelpers.h"
#include "wikiAPI.h"
#include "pageListHelpers.h"



char *template_DRL = "# $SubSystem$ Design Requirements List\n# table {.tabset}";

/**
 * @brief Builds a DRL (Design Requirements List) string from a JSON object containing requirements.
 * 
 * This function constructs a DRL string by iterating over a JSON array of requirement objects. Each requirement
 * object is expected to contain specific fields such as "ID", "Path", "Title", and "Description". The resulting
 * DRL string is built by appending formatted information from each requirement to a template DRL string.
 * 
 * @param requirementList A `cJSON` object containing an array of requirement objects under the "requirements" key.
 * 
 * @return A dynamically allocated string containing the formatted DRL. The caller is responsible for freeing this memory. If the input JSON is not properly formatted or if memory allocation fails, the function may return an incorrect or partially filled string.
 * 
 * @details
 * - The function first retrieves the "requirements" array from the `requirementList` object.
 * - It initializes the DRL string using a predefined template.
 * - For each requirement object in the array, it extracts the fields "ID", "Path", "Title", and "Description".
 * - These fields are appended to the DRL string in a specific format, including separators and markers.
 * - After processing all requirements, the function appends "{.links-list}" to the end of the DRL string.
 * - If any errors are encountered (e.g., missing "requirements" array or incorrect object format), appropriate error messages are printed.
 * - The function returns the final DRL string.
 */
static char *buildDrlFromJSONRequirementList(cJSON *requirementList, char* subSystem);


void syncDrlToSheet(command cmd){
    log_message(LOG_DEBUG, "Entering function syncDrlToSheet");

    refreshOAuthToken();

    char *sheetId;
    char *drlPageId;

    if(strcmp(cmd.argument_1, "ST")==0){
        drlPageId = "420";
        sheetId = "ST!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "PR")==0){
        drlPageId = "414";
        sheetId = "PR!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "FD")==0){
        drlPageId = "416";
        sheetId = "FD!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "RE")==0){
        drlPageId = "419";
        sheetId = "RE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "GS")==0){
        drlPageId = "417";
        sheetId = "GS!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "AV")==0){
        drlPageId = "421";
        sheetId = "AV!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "TE")==0){
        drlPageId = "1883";
        sheetId = "TE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "PL")==0){
        drlPageId = "418";
        sheetId = "PL!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "GE")==0){
        drlPageId = "415";
        sheetId = "GE!A3:AT300";
    }
    
    batchGetSheet("1i_PTwIqLuG9IUI73UaGuOvx8rVTDV1zIS7gmXNjMs1I", sheetId);

    cJSON *requirementList = parseArrayIntoJSONRequirementList(chunk.response); 
    char *DRL = buildDrlFromJSONRequirementList(requirementList, cmd.argument_1);

    pageList* drlPage = NULL;
    drlPage = addPageToList(&drlPage, drlPageId, "", "", "", "", "", "", "");
    drlPage->content = DRL;


    drlPage->content = replaceWord(drlPage->content, "\n", "\\\\n");
    drlPage->content = replaceWord(drlPage->content, "\"", "\\\\\\\"");

    updatePageContentMutation(drlPage);
    renderMutation(&drlPage, false);

    freePageList(&drlPage);
    cJSON_Delete(requirementList);
    free(DRL);
    
    log_message(LOG_DEBUG, "Exiting function syncDrlToSheet");
    return;
}

static char *buildDrlFromJSONRequirementList(cJSON *requirementList, char* subSystem){
    log_message(LOG_DEBUG, "Entering function buildDrlFromJSONRequirementList");
    
    // Get the requirements array from the requirementList object
    cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");
    if (!cJSON_IsArray(requirements)) {
        log_message(LOG_ERROR, "Error: requirements is not a JSON array");
    }

    char *DRL = strdup(template_DRL);

    if(strcmp(subSystem, "GE") == 0){
        DRL = replaceWord(DRL, "$SubSystem$", "General");
    }
    if(strcmp(subSystem, "ST") == 0){
        DRL = replaceWord(DRL, "$SubSystem$", "Structure");
    }
    if(strcmp(subSystem, "PR") == 0){
        DRL = replaceWord(DRL, "$SubSystem$", "Propulsion");
    }
    if(strcmp(subSystem, "FD") == 0){
        DRL = replaceWord(DRL, "$SubSystem$", "Flight Dynamics");
    }
    if(strcmp(subSystem, "RE") == 0){
        DRL = replaceWord(DRL, "$SubSystem$", "Recovery");
    }
    if(strcmp(subSystem, "GS") == 0){
        DRL = replaceWord(DRL, "$SubSystem$", "Ground Segment");
    }
    if(strcmp(subSystem, "AV") == 0){
        DRL = replaceWord(DRL, "$SubSystem$", "Avionics");
    }
    if(strcmp(subSystem, "PL") == 0){
        DRL = replaceWord(DRL, "$SubSystem$", "Payload");
    }
    if(strcmp(subSystem, "TE") == 0){
        DRL = replaceWord(DRL, "$SubSystem$", "Test");
    }

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
        
        if(title == NULL || strstr(id->valuestring, "2024_") == NULL){
            log_message(LOG_DEBUG, "Found a new group");

            if(!isFirstGroup){
                DRL = appendToString(DRL, "{.links-list}");
                isFirstGroup = 0;
            }
            else{isFirstGroup = 0;}

            DRL = appendToString(DRL, "\n\n\n## ");
            DRL = appendToString(DRL, id->valuestring);
            DRL = appendToString(DRL, "\n");
            continue;
        }

        if (cJSON_IsString(id) && id->valuestring) {
            log_message(LOG_DEBUG, "ID: %s", id->valuestring);
            log_message(LOG_DEBUG, "title: %s", title->valuestring);
            DRL = appendToString(DRL, "- [");
            DRL = appendToString(DRL, id->valuestring);
            DRL = appendToString(DRL, "](/");
            DRL = appendToString(DRL, "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_");
            DRL = appendToString(DRL, subSystem);
            DRL = appendToString(DRL, "_DRL/");
            DRL = appendToString(DRL, id->valuestring);
            DRL = appendToString(DRL, ") **");
        }
        
        
        
        if (cJSON_IsString(title) && title->valuestring) {
            log_message(LOG_DEBUG, "title: %s", title->valuestring);
            DRL = appendToString(DRL, title->valuestring);
            DRL = appendToString(DRL, "**\n");
        }
        if (cJSON_IsString(description) && description->valuestring) {
            log_message(LOG_DEBUG, "Description: %s", description->valuestring);
            DRL = appendToString(DRL, description->valuestring);
            DRL = appendToString(DRL, "\n");
        }

    }

    DRL = appendToString(DRL, "{.links-list}");
    
    log_message(LOG_DEBUG, "Exiting function buildDrlFromJSONRequirementList");

    return DRL;

}