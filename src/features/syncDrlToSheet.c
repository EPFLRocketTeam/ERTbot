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
static char *buildDrlFromJSONRequirementList(const cJSON *requirementList, const cJSON* subsystem);


void syncDrlToSheet(command cmd){
    log_message(LOG_DEBUG, "Entering function syncDrlToSheet");

    cJSON* subsystem = getSubsystemInfo(cmd.argument_1);
    cJSON *requirementList = getRequirements(subsystem);

    char *DRL = buildDrlFromJSONRequirementList(requirementList, subsystem);

    pageList* drlPage = NULL;
    const char *drlPageId = cJSON_GetObjectItem(subsystem, "DRL Page ID")->valuestring;
    drlPage = addPageToList(&drlPage, drlPageId, NULL, NULL, NULL, DRL, NULL, NULL, NULL);

    updatePageContentMutation(drlPage);
    renderMutation(&drlPage, false);
    freePageList(&drlPage);

    cJSON_Delete(requirementList);
    cJSON_Delete(subsystem);

    free(DRL);
    log_message(LOG_DEBUG, "Exiting function syncDrlToSheet");
    return;
}

static char *buildDrlFromJSONRequirementList(const cJSON *requirementList, const cJSON* subsystem){
    log_message(LOG_DEBUG, "Entering function buildDrlFromJSONRequirementList");

    // Get the requirements array from the requirementList object
    const cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");
    if (!cJSON_IsArray(requirements)) {
        log_message(LOG_ERROR, "Error: requirements is not a JSON array");
        char *DRL = duplicate_Malloc("There was an error when parsing the requirements, you might be missing a header value.");

        return DRL;
    }

    char *DRL = duplicate_Malloc(template_DRL);

    DRL = replaceWord_Realloc(DRL, "$SubSystem$", cJSON_GetObjectItem(subsystem, "Name")->valuestring);

    int isFirstGroup = 1;

    // Iterate over each requirement object in the requirements array
    int num_reqs = cJSON_GetArraySize(requirements);
    for (int i = 0; i < num_reqs; i++) {
        const cJSON *requirement = cJSON_GetArrayItem(requirements, i);
        if (!cJSON_IsObject(requirement)) {
            log_message(LOG_ERROR, "Error: requirement is not a JSON object");
            continue;
        }

        cJSON *id = cJSON_GetObjectItem(requirement, "ID");
        cJSON *title = cJSON_GetObjectItem(requirement, "Title");
        cJSON *description = cJSON_GetObjectItem(requirement, "Description");
        

        if(strlen(id->valuestring) < 2){
            log_message(LOG_DEBUG, "ID is smaller than one, breaking");
            break;
        }

        if(title == NULL || strstr(id->valuestring, "2024_") == NULL){

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
            DRL = appendToString(DRL, "- [");
            DRL = appendToString(DRL, id->valuestring);
            DRL = appendToString(DRL, "](/");
            DRL = appendToString(DRL, cJSON_GetObjectItem(subsystem, "Requirement Pages Directory")->valuestring);
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
        if(strlen(id->valuestring)<2||strlen(title->valuestring)<2||strlen(description->valuestring)<2){
            free(DRL);
            DRL = duplicate_Malloc("You are missing an, id, description or title value.");
            break;
        }
    }

    DRL = appendToString(DRL, "{.links-list}");
    DRL = replaceWord_Realloc(DRL, "\n", "\\\\n");
    DRL = replaceWord_Realloc(DRL, "\"", "\\\\\\\"");

    log_message(LOG_DEBUG, "Exiting function buildDrlFromJSONRequirementList");

    return DRL;
}
