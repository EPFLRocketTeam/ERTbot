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

char *template_REQ = "";

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
static char *buildRequirementPageFromJSONRequirementList(cJSON *requirement);

static void updateRequirementPageContent(pageList* reqPage, cJSON *requirement);

void updateRequirementPage(command cmd){
    log_message(LOG_DEBUG, "Entering function updateRequirementPages");

    cJSON* subsystem = getSubsystemInfo(cmd.argument_1);
    char *path = cJSON_GetObjectItem(subsystem, "Requirement Pages Directory")->valuestring;
    cJSON *requirementList = getRequirements(subsystem);

    pageList* requirementPagesHead = NULL;
    requirementPagesHead = populatePageList(&requirementPagesHead, "path", path);
    pageList* currentReqPage = requirementPagesHead;

    cJSON *requirements = cJSON_GetObjectItemCaseSensitive(requirementList, "requirements");
    if (!cJSON_IsArray(requirements)) {
        log_message(LOG_ERROR, "Error: requirements is not a JSON array");
    }

    int num_reqs = cJSON_GetArraySize(requirements);
    while (currentReqPage){
        for (int i = 0; i < num_reqs; i++) {
            cJSON *requirement = cJSON_GetArrayItem(requirements, i);

            if (!cJSON_IsObject(requirement)) {
                log_message(LOG_ERROR, "Error: requirement is not a JSON object");
                continue;
            }

            cJSON *id = cJSON_GetObjectItemCaseSensitive(requirement, "ID");

            if (!cJSON_IsString(id) || strcmp(id->valuestring, currentReqPage->title) != 0){
                //found a requirement group/category
                continue;
            }

            updateRequirementPageContent(currentReqPage, requirement);

            break;
        }


        currentReqPage = currentReqPage->next;
    }

    cJSON_Delete(requirementList);
    freePageList(&requirementPagesHead);

    log_message(LOG_DEBUG, "Exiting function updateRequirementPage");
    return;
}

static void updateRequirementPageContent(pageList* reqPage, cJSON *requirement){
    reqPage = getPage(&reqPage);
    reqPage->content = replaceWord(reqPage->content, "\\n", "\n");

    cJSON *id = cJSON_GetObjectItemCaseSensitive(requirement, "ID");

    char* importedRequirementInformation = buildRequirementPageFromJSONRequirementList(requirement);
    char* flag = createCombinedString("<!--", id->valuestring);
    flag = appendToString(flag, "-->");

    char* start = reqPage->content;
    start = strstr(start, flag);
    start = start + strlen(flag);

    char* end;
    end = strstr(start + 1, flag);
    end--;

    reqPage->content = replaceParagraph(reqPage->content, importedRequirementInformation, start, end);
    reqPage->content = replaceWord(reqPage->content, "\n", "\\\\n");
    reqPage->content = replaceWord(reqPage->content, "\"", "\\\\\\\"");

    updatePageContentMutation(reqPage);
    renderMutation(&reqPage, false);

    free(flag);

    return;
}

static char *buildRequirementPageFromJSONRequirementList(cJSON *requirement){
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
        pageContent = appendToString(pageContent, "\n# ");
        pageContent = appendToString(pageContent, id->valuestring);
        pageContent = appendToString(pageContent, ": ");
    }
    if (cJSON_IsString(title) && title->valuestring) {
        pageContent = appendToString(pageContent, title->valuestring);
        pageContent = appendToString(pageContent, "\n");
    }


    //DESCRIPTION
    if (cJSON_IsString(description) && description->valuestring) {
        pageContent = appendToString(pageContent, ">**Description**: ");
        pageContent = appendToString(pageContent, description->valuestring);
        pageContent = appendToString(pageContent, "\n");
    }


    //INFORMATION BOX: SOURCES AND ASSIGNEE
    if (cJSON_IsString(source) && source->valuestring && strcmp(source->valuestring, "") != 0) {
        pageContent = appendToString(pageContent, "\n>**Source**: ");
        pageContent = appendToString(pageContent, source->valuestring);
        pageContent = appendToString(pageContent, "\n");
    }
    if (cJSON_IsString(author) && author->valuestring && strcmp(author->valuestring, "") != 0) {
        pageContent = appendToString(pageContent, ">**Author**: ");
        pageContent = appendToString(pageContent, author->valuestring);
        pageContent = appendToString(pageContent, "\n");
    }
    if (cJSON_IsString(assignee) && assignee->valuestring && strcmp(assignee->valuestring, "") != 0) {
        pageContent = appendToString(pageContent, ">**Assignee**: ");
        pageContent = appendToString(pageContent, assignee->valuestring);
        pageContent = appendToString(pageContent, "\n");
    }
    if (strcmp(source->valuestring, "") != 0 || strcmp(author->valuestring, "") != 0 || strcmp(assignee->valuestring, "") != 0) {
        pageContent = appendToString(pageContent, "{.is-info}\n");
    }


    //JUSTIFICATION
    if (cJSON_IsString(justification) && justification->valuestring && strcmp(justification->valuestring, "") != 0) {
        pageContent = appendToString(pageContent, "\n## Justification\n");
        pageContent = appendToString(pageContent, justification->valuestring);
        pageContent = appendToString(pageContent, "\n");
    }


    //COMPLIANCE
    if (cJSON_IsString(compliance) && compliance->valuestring && strcmp(compliance->valuestring, REQ_SHEET_EMPTY_VALUE) != 0) {

        if(strcmp(compliance->valuestring, "Compliant") == 0){
            pageContent = appendToString(pageContent, "\n# Compliance\n");
            pageContent = appendToString(pageContent, ":green_circle: Compliant\n");
        }
        if(strcmp(compliance->valuestring, "Unknown") == 0){
            pageContent = appendToString(pageContent, "\n# Compliance\n");
            pageContent = appendToString(pageContent, ":orange_circle: Unknown\n");
        }
        if(strcmp(compliance->valuestring, "Uncompliant") == 0){
            pageContent = appendToString(pageContent, "\n# Compliance\n");
            pageContent = appendToString(pageContent, ":red_circle: Uncompliant\n");
        }
    }


    //CRITICALITY
    if (cJSON_IsString(criticality) && criticality->valuestring && strcmp(criticality->valuestring, REQ_SHEET_EMPTY_VALUE) != 0) {

        if(strcmp(criticality->valuestring, "Low") == 0){
            pageContent = appendToString(pageContent, "\n# Criticality\n");
            pageContent = appendToString(pageContent, ":green_circle: Low\n");
        }
        if(strcmp(criticality->valuestring, "Medium") == 0){
            pageContent = appendToString(pageContent, "\n# Criticality\n");
            pageContent = appendToString(pageContent, ":orange_circle: Medium\n");
        }
        if(strcmp(criticality->valuestring, "High") == 0){
            pageContent = appendToString(pageContent, "\n# Criticality\n");
            pageContent = appendToString(pageContent, ":red_circle: High\n");
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
                    pageContent = appendToString(pageContent, "\n# Verification");
                    isVerification = 1;
                }

                char temp_verificationNumber[100];
                snprintf(temp_verificationNumber, sizeof(temp_verificationNumber), "\n## Verification %d\n", verificationCount);
                pageContent = appendToString(pageContent, temp_verificationNumber);

                pageContent = appendToString(pageContent, "**Method**: ");
                pageContent = appendToString(pageContent, verificationMethod->valuestring);
                pageContent = appendToString(pageContent, "\n**Deadline**: ");
                pageContent = appendToString(pageContent, reviewName);
                pageContent = appendToString(pageContent, "\n");

                if(strcmp(verificationStatus->valuestring, REQ_SHEET_EMPTY_VALUE) != 0){
                    pageContent = appendToString(pageContent, "**Status**: ");

                    if(strcmp(verificationStatus->valuestring, "Completed") == 0){pageContent = appendToString(pageContent, ":green_circle:");}
                    if(strcmp(verificationStatus->valuestring, "In progress") == 0){pageContent = appendToString(pageContent, ":orange_circle:");}
                    if(strcmp(verificationStatus->valuestring, "Uncompleted") == 0){pageContent = appendToString(pageContent, ":red_circle:");}

                    pageContent = appendToString(pageContent, verificationStatus->valuestring);
                    pageContent = appendToString(pageContent, "\n");

                }

                log_message(LOG_DEBUG, "Page content: %s",  pageContent);

            }

        }
    }

    log_message(LOG_DEBUG, "Exiting function buildRequirementPageFromJSONRequirementList");

    return pageContent;
}
