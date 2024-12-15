#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <cjson/cJSON.h>
#include "ERTbot_config.h"
#include "ERTbot_common.h"
#include "sheetAPI.h"
#include "stringHelpers.h"
#include "wikiAPI.h"
#include "requirementsHelpers.h"
#include "pageListHelpers.h"
#include "slackAPI.h"

#define ID_BLOCK_TEMPLATE "\n# $ID$: "
#define TITLE_BLOCK_TEMPLATE "$Title$\n"
#define DESCRIPTION_BLOCK_TEMPLATE ">**Description**: $Description$\n\n"
#define SOURCE_BLOCK_TEMPLATE ">**Source**: $Source$\n"
#define AUTHOR_BLOCK_TEMPLATE ">**Author**: $Author$\n"
#define ASSIGNEE_BLOCK_TEMPLATE ">**Assignee**: $Assignee$\n"
#define JUSTIFICATION_BLOCK_TEMPLATE "\n## Justification\n$Justification$\n"
#define COMPLIANCE_BLOCK_TEMPLATE "\n# Compliance\n$Compliance$\n"
#define CRITICALITY_BLOCK_TEMPLATE "\n# Criticality\n$Criticality$\n"
#define FIRST_VERIFICATION_METHOD_BLOCK_TEMPLATE "\n# Verification\n##Verification 1\n**Method**: "

#define VERIFICATION_METHOD_BLOCK_TEMPLATE "\n## Verification $Verification Number$\n**Method**: $Verification Method$"
#define VERIFICATION_DEADLINE_BLOCK_TEMPLATE "\n**Deadline**: $Verification Deadline$"
#define VERIFICATION_STATUS_BLOCK_TEMPLATE "\n**Status**: $Verification Status$\n"


/**
 * @brief Builds a `pageList` entry from a JSON object containing requirements for a specific requirement ID.
 *        Each page's content is a requirement page.
 *
 * This function generates a `pageList` entry by extracting details for a specific requirement from a JSON object. It constructs a page content string using a template and appends information such as ID, Title, Description, Source, Author, Justification, Compliance, Criticality, and Verification details. The page content is formatted in a specific markdown-like structure.
 *
 * @param requirementList A `cJSON` object containing an array of requirement objects under the "requirements" key.
 * @param requirementId A string representing the ID of the requirement to be processed.
 *
 * @return A pointer to a `pageList` structure containing the generated page. If no requirement with the specified ID is found or if an error occurs, the function may return `NULL`.
 *
 * @details
 * - The function first retrieves the "requirements" array from the `requirementList` object.
 * - It initializes the page content using a predefined template.
 * - For each requirement object in the array, it extracts fields including "ID", "Title", "Description", "Source", "Author", "Justification", "Criticality", "Compliance", and "Verification" details.
 * - If the ID matches the `requirementId` parameter, the page content is updated with formatted information from the requirement.
 * - Sections are added to the page content based on the presence and values of the fields:
 *   - **Title**: Displays the requirement ID and title.
 *   - **Description**: Displays the description of the requirement.
 *   - **Information Box**: Shows additional information such as Source, Author, and Assignee, if they are not empty.
 *   - **Justification**: Adds justification if available.
 *   - **Compliance**: Shows the compliance status with corresponding icons.
 *   - **Criticality**: Shows the criticality level with corresponding icons.
 *   - **Verification**: Adds verification details for multiple reviews and verification methods, including status and deadlines.
 * - After processing, the page content is added to the `pageList` structure and the list is returned.
 * - If any errors are encountered (e.g., missing "requirements" array or incorrect object format), appropriate error messages are printed.
 */
static char *buildRequirementPageFromJSONRequirementList(const cJSON *requirement);

static void updateRequirementPageContent(pageList* reqPage, const cJSON *requirement);

static void addVerificationInformationToPageContent(char** pageContent, const cJSON* requirement);

void updateRequirementPage(command cmd){
    log_function_entry(__func__);

    updateCommandStatusMessage("fetching subsystem info");
    cJSON* subsystem = getSubsystemInfo(cmd.argument);
    const char *path = cJSON_GetObjectItem(subsystem, "Requirement Pages Directory")->valuestring;
    
    updateCommandStatusMessage("fetching requirements");
    cJSON *requirementList = getRequirements(subsystem);

    updateCommandStatusMessage("fetching requirement pages");
    pageList* requirementPagesHead = NULL;
    requirementPagesHead = populatePageList(&requirementPagesHead, "path", path);
    pageList* currentReqPage = requirementPagesHead;

    const cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");


    if (!cJSON_IsArray(requirements)) {
        log_message(LOG_ERROR, __func__, "Error: requirements is not a JSON array");
    }

    updateCommandStatusMessage("updating requirement pages");
    int cnt = 0;
    int num_reqs = cJSON_GetArraySize(requirements);
    while (currentReqPage){
        for (int i = 0; i < num_reqs; i++) {
            const cJSON *requirement = cJSON_GetArrayItem(requirements, i);

            if (!cJSON_IsObject(requirement)) {
                log_message(LOG_ERROR, __func__, "Error: requirement is not a JSON object");
                continue;
            }

            const cJSON *id = cJSON_GetObjectItem(requirement, "ID");

            if (!cJSON_IsString(id) || strcmp(id->valuestring, currentReqPage->title) != 0){
                continue;
            }

            updateRequirementPageContent(currentReqPage, requirement);

            break;
        }

        cnt++;
        sendLoadingBar(cnt, num_reqs);

        currentReqPage = currentReqPage->next;
    }

    cJSON_Delete(requirementList);
    cJSON_Delete(subsystem);
    freePageList(&requirementPagesHead);

    log_function_exit(__func__);
    return;
}

static void updateRequirementPageContent(pageList* reqPage, const cJSON *requirement){

    free(reqPage->title);
    reqPage->title = NULL;

    free(reqPage->path);
    reqPage->path = NULL;

    free(reqPage->updatedAt);
    reqPage->updatedAt = NULL;

    reqPage = getPage(&reqPage);
    reqPage->content = replaceWord_Realloc(reqPage->content, "\\n", "\n");

    char* importedRequirementInformation = buildRequirementPageFromJSONRequirementList(requirement);

    const cJSON *id = cJSON_GetObjectItem(requirement, "ID");

    char* flag = createCombinedString("<!--", id->valuestring);
    flag = appendToString(flag, "-->");

    char* start = reqPage->content;
    start = strstr(start, flag);
    start = start + strlen(flag);

    char* end;
    end = strstr(start + 1, flag);
    end--;

    char *newContent = replaceParagraph(reqPage->content, importedRequirementInformation, start, end);
    
    if(strcmp(newContent, reqPage->content)==0){
        free(newContent);
        free(reqPage->content);
        reqPage->content = NULL;
        free(importedRequirementInformation);
        free(flag);
        log_message(LOG_DEBUG, __func__, "updateRequirementPageContent: Requirement Page is already up to date.");
        return;
    }

    if (newContent != NULL) {
        free(reqPage->content);  // Free the old content
        reqPage->content = newContent;  // Update to point to the new content
    }

    reqPage->content = replaceWord_Realloc(reqPage->content, "\n", "\\\\n");
    reqPage->content = replaceWord_Realloc(reqPage->content, "\"", "\\\\\\\"");
    reqPage->content = replaceWord_Realloc(reqPage->content, "\r", "");
    reqPage->content = replaceWord_Realloc(reqPage->content, "\t", "");
    reqPage->content = replaceWord_Realloc(reqPage->content, "   ", "");

    updatePageContentMutation(reqPage);
    renderMutation(&reqPage, false);

    free(reqPage->content);
    reqPage->content = NULL;

    free(importedRequirementInformation);
    free(flag);

    return;
}

static char* buildRequirementPageFromJSONRequirementList(const cJSON *requirement){
    log_function_entry(__func__);

    char* pageContent = duplicate_Malloc("");

    char* requirement_print = cJSON_Print(requirement);
    log_message(LOG_DEBUG, __func__, "buildRequirementPageFromJSONRequirementList: requirement: %s", requirement_print);
    free(requirement_print);


    log_message(LOG_DEBUG, __func__, "buildRequirementPageFromJSONRequirementList: starting to append standard block");

    (void)addSectionToPageContent(&pageContent, ID_BLOCK_TEMPLATE, requirement, "ID");
    (void)addSectionToPageContent(&pageContent, TITLE_BLOCK_TEMPLATE, requirement, "Title");
    (void)addSectionToPageContent(&pageContent, DESCRIPTION_BLOCK_TEMPLATE, requirement, "Description");

    log_message(LOG_DEBUG, __func__, "buildRequirementPageFromJSONRequirementList: Description finished");

    int hasSource = addSectionToPageContent(&pageContent, SOURCE_BLOCK_TEMPLATE, requirement, "Source");
    int hasAuthor = addSectionToPageContent(&pageContent, AUTHOR_BLOCK_TEMPLATE, requirement, "Author");
    int hasAssignee = addSectionToPageContent(&pageContent, ASSIGNEE_BLOCK_TEMPLATE, requirement, "Assignee");

    if(hasSource || hasAuthor || hasAssignee){
        pageContent = appendToString(pageContent, "{.is-info}\n");
    }

    log_message(LOG_DEBUG, __func__, "buildRequirementPageFromJSONRequirementList: Assignee finished");

    (void)addSectionToPageContent(&pageContent, JUSTIFICATION_BLOCK_TEMPLATE, requirement, "Justification");
    (void)addSectionToPageContent(&pageContent, COMPLIANCE_BLOCK_TEMPLATE, requirement, "Compliance");
    (void)addSectionToPageContent(&pageContent, CRITICALITY_BLOCK_TEMPLATE, requirement, "Criticality");

    log_message(LOG_DEBUG, __func__, "buildRequirementPageFromJSONRequirementList: Criticality finished");

    log_message(LOG_DEBUG, __func__, "Going to start filling in the verification section");

    addVerificationInformationToPageContent(&pageContent, requirement);

    log_function_exit(__func__);

    return pageContent;
}

static void addVerificationInformationToPageContent(char** pageContent, const cJSON* requirement){
    log_function_entry(__func__);

    //VERIFICAITON
    bool verificationTitleAdded = false;
    int verificationCount = 0;
    
    for (int verificationNumber = 1; verificationNumber <= MAXIMUM_NUMBER_OF_VERIFICATIONS; verificationNumber++){

        char JsonItemNameMethod[50];
        snprintf(JsonItemNameMethod, sizeof(JsonItemNameMethod), "Verification Method %d", verificationNumber);

        if(!cJSON_HasObjectItem(requirement, JsonItemNameMethod) || strcmp(cJSON_GetObjectItem(requirement, JsonItemNameMethod)->valuestring, "N/A") == 0 
            || strcmp(cJSON_GetObjectItem(requirement, JsonItemNameMethod)->valuestring, "") == 0){
            continue;
        }

        verificationCount++;
        if(!verificationTitleAdded){
            *pageContent = appendToString(*pageContent, "\n# Verification");
            verificationTitleAdded = true;
        }

        (void)addVerificationSectionToPageContent(pageContent, VERIFICATION_METHOD_BLOCK_TEMPLATE, requirement, "Verification Method", verificationNumber, verificationCount);
        (void)addVerificationSectionToPageContent(pageContent, VERIFICATION_DEADLINE_BLOCK_TEMPLATE, requirement, "Verification Deadline", verificationNumber, verificationCount);
        (void)addVerificationSectionToPageContent(pageContent, VERIFICATION_STATUS_BLOCK_TEMPLATE, requirement, "Verification Status", verificationNumber, verificationCount);
    }

    log_function_exit(__func__);
}