#include <string.h>
#include <cjson/cJSON.h>
#include "ERTbot_config.h"
#include "ERTbot_common.h"




cJSON *parseArrayIntoJSONRequirementList(char *input_str) {
    log_message(LOG_DEBUG, "Entering function parseArrayIntoJSONRequirementList");
    

    //log_message(LOG_DEBUG, "input_str: %s\n", input_str);

    // Parse the input string as JSON
    cJSON *input_json = cJSON_Parse(input_str);
    if (!input_json) {
        log_message(LOG_ERROR, "Error parsing input string as JSON object");
        return NULL;
    }

    // Extract the "values" array from the JSON object
    cJSON *values_array = cJSON_GetObjectItemCaseSensitive(input_json, "values");
    if (!cJSON_IsArray(values_array)) {
        log_message(LOG_ERROR, "Error: values is not a JSON array");
        cJSON_Delete(input_json);
        return NULL;
    }

    // Create a JSON object to hold the requirements
    cJSON *json = cJSON_CreateObject();
    if (!json) {
        log_message(LOG_ERROR, "Error creating JSON object");
        cJSON_Delete(input_json);
        return NULL;
    }

    // Create a JSON array to hold the requirement objects
    cJSON *requirements = cJSON_CreateArray();
    if (!requirements) {
        log_message(LOG_ERROR, "Error creating JSON array");
        cJSON_Delete(json);
        cJSON_Delete(input_json);
        return NULL;
    }

    // Add the requirements array to the JSON object
    cJSON_AddItemToObject(json, "requirements", requirements);

    log_message(LOG_DEBUG, "about to parse requirements from value array into cJSON object");

    // Iterate over the input array of arrays and create JSON objects for each requirement
    int num_reqs = cJSON_GetArraySize(values_array);
    for (int i = 0; i < num_reqs; i++) {
        
        cJSON *req_array = cJSON_GetArrayItem(values_array, i);
        
    
        // Create a JSON object for the current requirement
        cJSON *req = cJSON_CreateObject();
        if (!req) {
            log_message(LOG_ERROR, "Error creating JSON object for requirement");
            cJSON_Delete(json);
            cJSON_Delete(input_json);
            return NULL;
        }

        if(cJSON_GetArrayItem(req_array, REQ_ID_COL) == NULL || strcmp(cJSON_GetArrayItem(req_array, REQ_ID_COL)->valuestring, "")==0){
            log_message(LOG_DEBUG, "ID is empty");
            break;
        }

        // Add the fields to the requirement JSON object
        cJSON_AddStringToObject(req, "ID", cJSON_GetArrayItem(req_array, REQ_ID_COL)->valuestring);
        log_message(LOG_DEBUG, "About to parse the rest of %s", cJSON_GetArrayItem(req_array, REQ_ID_COL)->valuestring);

        if(cJSON_GetArrayItem(req_array, REQ_TITLE_COL) == NULL){
            log_message(LOG_DEBUG, "Title is empty");
            cJSON_AddItemToArray(requirements, req);
            continue;
        }

        
        cJSON_AddStringToObject(req, "Title", cJSON_GetArrayItem(req_array, REQ_TITLE_COL) ?
            cJSON_GetArrayItem(req_array, REQ_TITLE_COL)->valuestring :
            "");
        cJSON_AddStringToObject(req, "Description", cJSON_GetArrayItem(req_array, REQ_DESCRIPTION_COL) ?
            cJSON_GetArrayItem(req_array, REQ_DESCRIPTION_COL)->valuestring :
            "");
        cJSON_AddStringToObject(req, "Source", cJSON_GetArrayItem(req_array, REQ_SOURCE_COL) ?
            cJSON_GetArrayItem(req_array, REQ_SOURCE_COL)->valuestring :
            "");
        cJSON_AddStringToObject(req, "Author", cJSON_GetArrayItem(req_array, REQ_AUTHOR_COL) ?
            cJSON_GetArrayItem(req_array, REQ_AUTHOR_COL)->valuestring :
            "");
        cJSON_AddStringToObject(req, "Justification", cJSON_GetArrayItem(req_array, REQ_JUSTIFICATION_COL) ?
            cJSON_GetArrayItem(req_array, REQ_JUSTIFICATION_COL)->valuestring :
            "");
        cJSON_AddStringToObject(req, "Criticality", cJSON_GetArrayItem(req_array, REQ_CRITICALITY_COL) ?
            cJSON_GetArrayItem(req_array, REQ_CRITICALITY_COL)->valuestring :
            "TBD");
        cJSON_AddStringToObject(req, "Compliance", cJSON_GetArrayItem(req_array, REQ_COMPLIANCE_COL) ?
            cJSON_GetArrayItem(req_array, REQ_COMPLIANCE_COL)->valuestring :
            "TBD");
        cJSON_AddStringToObject(req, "Verification Status", cJSON_GetArrayItem(req_array, REQ_VERIFICATION_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_VERIFICATION_STATUS_COL)->valuestring :
            "");
        cJSON_AddStringToObject(req, "Assignee", cJSON_GetArrayItem(req_array, REQ_ASSIGNEE_COL) ?
            cJSON_GetArrayItem(req_array, REQ_ASSIGNEE_COL)->valuestring :
            "");
        
        cJSON_AddStringToObject(req, "Review 1 Verification 1 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_1_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_1_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 1 Verification 1 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_1_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_1_STATUS_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 1 Verification 2 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_2_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_2_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 1 Verification 2 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_2_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_2_STATUS_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 1 Verification 3 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_3_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_3_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 1 Verification 3 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_3_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_1_VERIFICATION_3_STATUS_COL)->valuestring :
            "N/A");
        
        cJSON_AddStringToObject(req, "Review 2 Verification 1 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_1_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_1_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 2 Verification 1 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_1_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_1_STATUS_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 2 Verification 2 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_2_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_2_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 2 Verification 2 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_2_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_2_STATUS_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 2 Verification 3 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_3_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_3_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 2 Verification 3 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_3_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_2_VERIFICATION_3_STATUS_COL)->valuestring :
            "N/A");
        
        cJSON_AddStringToObject(req, "Review 3 Verification 1 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_1_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_1_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 3 Verification 1 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_1_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_1_STATUS_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 3 Verification 2 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_2_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_2_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 3 Verification 2 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_2_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_2_STATUS_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 3 Verification 3 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_3_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_3_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 3 Verification 3 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_3_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_3_VERIFICATION_3_STATUS_COL)->valuestring :
            "N/A");

        cJSON_AddStringToObject(req, "Review 4 Verification 1 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_1_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_1_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 4 Verification 1 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_1_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_1_STATUS_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 4 Verification 2 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_2_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_2_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 4 Verification 2 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_2_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_2_STATUS_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 4 Verification 3 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_3_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_3_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 4 Verification 3 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_3_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_4_VERIFICATION_3_STATUS_COL)->valuestring :
            "N/A");

        cJSON_AddStringToObject(req, "Review 5 Verification 1 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_1_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_1_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 5 Verification 1 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_1_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_1_STATUS_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 5 Verification 2 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_2_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_2_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 5 Verification 2 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_2_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_2_STATUS_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 5 Verification 3 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_3_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_3_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 5 Verification 3 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_3_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_5_VERIFICATION_3_STATUS_COL)->valuestring :
            "N/A");

        cJSON_AddStringToObject(req, "Review 6 Verification 1 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_1_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_1_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 6 Verification 1 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_1_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_1_STATUS_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 6 Verification 2 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_2_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_2_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 6 Verification 2 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_2_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_2_STATUS_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 6 Verification 3 Method", cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_3_METHOD_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_3_METHOD_COL)->valuestring :
            "N/A");
        cJSON_AddStringToObject(req, "Review 6 Verification 3 Status", cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_3_STATUS_COL) ?
            cJSON_GetArrayItem(req_array, REQ_REVIEW_6_VERIFICATION_3_STATUS_COL)->valuestring :
            "N/A");

        //cJSON_AddStringToObject(req, "Path", cJSON_GetArrayItem(req_array, REQ_PATH_COL)->valuestring);
        

        // Add the requirement object to the requirements array
        cJSON_AddItemToArray(requirements, req);

        log_message(LOG_DEBUG, "Finished parsing array");
    }

    
    log_message(LOG_DEBUG, "Exiting function parseArrayIntoJSONRequirementList");


    if(chunk.response){
        free(chunk.response);
    }

    cJSON_Delete(input_json);

    return json;
}
