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

static int addSectionToPageContent(char** pageContent, const char* template, const cJSON* object, const char* item);

static void addVerificationInformationToPageContent(char** pageContent, const cJSON* requirement);

void updateRequirementPage(command cmd){
    log_message(LOG_DEBUG, "Entering function updateRequirementPages");

    cJSON* subsystem = getSubsystemInfo(cmd.argument_1);
    const char *path = cJSON_GetObjectItem(subsystem, "Requirement Pages Directory")->valuestring;
    cJSON *requirementList = getRequirements(subsystem);

    pageList* requirementPagesHead = NULL;
    requirementPagesHead = populatePageList(&requirementPagesHead, "path", path);
    pageList* currentReqPage = requirementPagesHead;

    const cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");


    if (!cJSON_IsArray(requirements)) {
        log_message(LOG_ERROR, "Error: requirements is not a JSON array");
    }

    int num_reqs = cJSON_GetArraySize(requirements);
    while (currentReqPage){
        for (int i = 0; i < num_reqs; i++) {
            const cJSON *requirement = cJSON_GetArrayItem(requirements, i);

            if (!cJSON_IsObject(requirement)) {
                log_message(LOG_ERROR, "Error: requirement is not a JSON object");
                continue;
            }

            const cJSON *id = cJSON_GetObjectItem(requirement, "ID");

            if (!cJSON_IsString(id) || strcmp(id->valuestring, currentReqPage->title) != 0){
                continue;
            }

            updateRequirementPageContent(currentReqPage, requirement);

            break;
        }


        currentReqPage = currentReqPage->next;
    }

    cJSON_Delete(requirementList);
    cJSON_Delete(subsystem);
    freePageList(&requirementPagesHead);

    log_message(LOG_DEBUG, "Exiting function updateRequirementPage");
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
    if (newContent != NULL) {
        free(reqPage->content);  // Free the old content
        reqPage->content = newContent;  // Update to point to the new content
    }

    reqPage->content = replaceWord_Realloc(reqPage->content, "\n", "\\\\n");
    reqPage->content = replaceWord_Realloc(reqPage->content, "\"", "\\\\\\\"");

    updatePageContentMutation(reqPage);
    renderMutation(&reqPage, false);

    free(reqPage->content);
    reqPage->content = NULL;

    free(importedRequirementInformation);

    free(flag);

    return;
}

static char* buildRequirementPageFromJSONRequirementList(const cJSON *requirement){
    log_message(LOG_DEBUG, "Entering function buildRequirementPageFromJSONRequirementList");

    char* pageContent = duplicate_Malloc("");

    char* requirement_print = cJSON_Print(requirement);
    log_message(LOG_DEBUG, "buildRequirementPageFromJSONRequirementList: requirement: %s", requirement_print);
    free(requirement_print);


    log_message(LOG_DEBUG, "buildRequirementPageFromJSONRequirementList: starting to append standard block");

    (void)addSectionToPageContent(&pageContent, ID_BLOCK_TEMPLATE, requirement, "ID");
    (void)addSectionToPageContent(&pageContent, TITLE_BLOCK_TEMPLATE, requirement, "Title");
    (void)addSectionToPageContent(&pageContent, DESCRIPTION_BLOCK_TEMPLATE, requirement, "Description");

    log_message(LOG_DEBUG, "buildRequirementPageFromJSONRequirementList: Description finished");

    int hasSource = addSectionToPageContent(&pageContent, SOURCE_BLOCK_TEMPLATE, requirement, "Source");
    int hasAuthor = addSectionToPageContent(&pageContent, AUTHOR_BLOCK_TEMPLATE, requirement, "Author");
    int hasAssignee = addSectionToPageContent(&pageContent, ASSIGNEE_BLOCK_TEMPLATE, requirement, "Assignee");

    if(hasSource || hasAuthor || hasAssignee){
        pageContent = appendToString(pageContent, "{.is-info}\n");
    }

    log_message(LOG_DEBUG, "buildRequirementPageFromJSONRequirementList: Assignee finished");

    (void)addSectionToPageContent(&pageContent, JUSTIFICATION_BLOCK_TEMPLATE, requirement, "Justification");
    (void)addSectionToPageContent(&pageContent, JUSTIFICATION_BLOCK_TEMPLATE, requirement, "Compliance");
    (void)addSectionToPageContent(&pageContent, JUSTIFICATION_BLOCK_TEMPLATE, requirement, "Criticality");

    log_message(LOG_DEBUG, "buildRequirementPageFromJSONRequirementList: Criticality finished");

    log_message(LOG_DEBUG, "Going to start filling in the verification section");

    addVerificationInformationToPageContent(&pageContent, requirement);

    log_message(LOG_DEBUG, "Exiting function buildRequirementPageFromJSONRequirementList");

    return pageContent;
}

static char* addDollarSigns(const char* characteristic){
    char* wordToReplace = duplicate_Malloc("$word$");
    wordToReplace = replaceWord_Realloc(wordToReplace, "word", characteristic);
    return wordToReplace;
}

static int addSectionToPageContent(char** pageContent, const char* template, const cJSON* object, const char* item){
    log_message(LOG_DEBUG, "Entering function addSectionToPageContent");

    if(!cJSON_HasObjectItem(object, item)){
        log_message(LOG_DEBUG, "addSectionToPageContent: characteristic does not exist");
        return 0;
    }

    const cJSON* jsonCharacteristic = cJSON_GetObjectItem(object, item);

    if(!cJSON_IsString(jsonCharacteristic) || strcmp(jsonCharacteristic->valuestring, "") == 0 || strcmp(jsonCharacteristic->valuestring, "N/A") == 0 || strcmp(jsonCharacteristic->valuestring, "TBD")==0){
        log_message(LOG_DEBUG, "addSectionToPageContent: Characteristic has no value");
        return 0;
    }
    
    char *newSection = duplicate_Malloc(template);
    char *wordToReplace = addDollarSigns(item);

    newSection = replaceWord_Realloc(newSection, wordToReplace, jsonCharacteristic->valuestring);

    *pageContent = appendToString(*pageContent, newSection);

    free(wordToReplace);
    free(newSection);


    log_message(LOG_DEBUG, "Exiting function addSectionToPageContent");
    return 1;
}

static void addVerificationInformationToPageContent(char** pageContent, const cJSON* requirement){
    log_message(LOG_DEBUG, "Enter function addVerificationInformationToPageContent");


    //VERIFICAITON
    bool verificationTitleAdded = 0;
    int verificationCount = 0;
    
    for (int verificationNumber = 1; verificationNumber <= MAXIMUM_NUMBER_OF_VERIFICATIONS; verificationNumber++){

        log_message(LOG_DEBUG, "addVerificationInformationToPageContent: starting for loop");

        char JsonItemNameMethod[1024];
        snprintf(JsonItemNameMethod, sizeof(JsonItemNameMethod), "Verification Method %d", verificationNumber);

        log_message(LOG_DEBUG, "addVerificationInformationToPageContent: JsonItemNameMethod initialised: %s", JsonItemNameMethod);

        
        if(!cJSON_HasObjectItem(requirement, JsonItemNameMethod)){
            continue;
        }

        log_message(LOG_DEBUG, "addVerificationInformationToPageContent: passed HasObjectItem check");

        cJSON *verificationMethod = cJSON_GetObjectItem(requirement, JsonItemNameMethod);
        if(strcmp(verificationMethod->valuestring, "N/A") == 0 || strcmp(verificationMethod->valuestring, "") == 0){
            continue;
        }

        log_message(LOG_DEBUG, "addVerificationInformationToPageContent: passed empty value check");

        verificationCount++;

        log_message(LOG_DEBUG, "Verification method found: verification count: %d", verificationCount);
        log_message(LOG_DEBUG, "Verification method found: JsonItemNameMethod: %s, verificationMethod: %s", JsonItemNameMethod, verificationMethod->valuestring);

        if(!verificationTitleAdded){
            *pageContent = appendToString(*pageContent, "\n# Verification");
            verificationTitleAdded = true;
        }

        char temp_verificationNumber[100];
        snprintf(temp_verificationNumber, sizeof(temp_verificationNumber), "\n## Verification %d\n", verificationCount);
        *pageContent = appendToString(*pageContent, temp_verificationNumber);
        *pageContent = appendToString(*pageContent, "**Method**: ");
        *pageContent = appendToString(*pageContent, verificationMethod->valuestring);


        char JsonItemNameDeadline[1024];
        snprintf(JsonItemNameDeadline, sizeof(JsonItemNameDeadline), "Verification Deadline %d", verificationNumber);

        if(cJSON_HasObjectItem(requirement, JsonItemNameDeadline)){
            const cJSON *verificationDeadline = cJSON_GetObjectItemCaseSensitive(requirement, JsonItemNameDeadline);

            if(strcmp(verificationDeadline->valuestring, REQ_SHEET_EMPTY_VALUE) != 0 && strcmp(verificationDeadline->valuestring, "") != 0){
                *pageContent = appendToString(*pageContent, "\n**Deadline**: ");
                *pageContent = appendToString(*pageContent, verificationDeadline->valuestring);
            }

        }
        
        char JsonItemNameStatus[1024];
        snprintf(JsonItemNameStatus, sizeof(JsonItemNameStatus), "Verification Status %d", verificationNumber);

        if(cJSON_HasObjectItem(requirement, JsonItemNameStatus)){
            const cJSON *verificationStatus = cJSON_GetObjectItemCaseSensitive(requirement, JsonItemNameStatus);
        
            if(strcmp(verificationStatus->valuestring, REQ_SHEET_EMPTY_VALUE) != 0 && strcmp(verificationStatus->valuestring, "") != 0){
                *pageContent = appendToString(*pageContent, "\n**Status**: ");

                if(strcmp(verificationStatus->valuestring, "Completed") == 0){*pageContent = appendToString(*pageContent, ":green_circle:");}
                if(strcmp(verificationStatus->valuestring, "In progress") == 0){*pageContent = appendToString(*pageContent, ":orange_circle:");}
                if(strcmp(verificationStatus->valuestring, "Uncompleted") == 0){*pageContent = appendToString(*pageContent, ":red_circle:");}

                *pageContent = appendToString(*pageContent, verificationStatus->valuestring);
                *pageContent = appendToString(*pageContent, "\n");

            }
        }
    }

    log_message(LOG_DEBUG, "Exiting function addVerificationInformationToPageContent");
}
