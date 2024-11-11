#include <stdbool.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "ERTbot_common.h"
#include "sheetAPI.h"
#include "requirementsHelpers.h"
#include "stringHelpers.h"
#include "wikiAPI.h"
#include "pageListHelpers.h"


#define DRL_TABSET_TITLE_TEMPLATE "\n\n\n## $ID$\n"
#define DRL_ID_BLOCK_TEMPLATE "- [$ID$](/"
#define DRL_PAGE_DIRECTORY "$Requirement Pages Directory$"
#define DRL_PAGE_NAME "$ID$) **"
#define DRL_TITLE_BLOCK_TEMPLATE "$Title$**\n"
#define DRL_DESCRIPTION_BLOCK_TEMPLATE "$Description$\n"

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

    cJSON* subsystem = getSubsystemInfo(cmd.argument);
    cJSON *requirementList = getRequirements(subsystem);

    char *DRL = buildDrlFromJSONRequirementList(requirementList, subsystem);

    pageList* drlPage = NULL;
    const char *drlPageId = cJSON_GetObjectItem(subsystem, "DRL Page ID")->valuestring;

    DRL = replaceWord_Realloc(DRL, "\r", "");

    drlPage = addPageToList(&drlPage, drlPageId, NULL, NULL, NULL, DRL, NULL);

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

        const cJSON *id = cJSON_GetObjectItem(requirement, "ID");
        
        if(strlen(id->valuestring) < 2){
            log_message(LOG_DEBUG, "ID is smaller than one, breaking");
            break;
        }

        if(strstr(id->valuestring, "2024_") == NULL){

            if(!isFirstGroup){
                DRL = appendToString(DRL, "{.links-list}");
            }
            else{isFirstGroup = 0;}

            (void)addSectionToPageContent(&DRL, DRL_TABSET_TITLE_TEMPLATE, requirement, "ID");

            continue;
        }

        (void)addSectionToPageContent(&DRL, DRL_ID_BLOCK_TEMPLATE, requirement, "ID");
        (void)addSectionToPageContent(&DRL, DRL_PAGE_DIRECTORY, subsystem, "Requirement Pages Directory");
        (void)addSectionToPageContent(&DRL, DRL_PAGE_NAME, requirement, "ID");
        int hasTitle = addSectionToPageContent(&DRL, DRL_TITLE_BLOCK_TEMPLATE, requirement, "Title");
        int hasDescription = addSectionToPageContent(&DRL, DRL_DESCRIPTION_BLOCK_TEMPLATE, requirement, "Description");

        if(!hasTitle || !hasDescription){
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
