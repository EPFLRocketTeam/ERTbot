#include "../include/struct.h"
#include "../include/api.h"
#include "../include/config.h"
#include "../include/features.h"
#include "../include/githubAPI.h"
#include "../include/helperFunctions.h"
#include "../include/markdownToPDF.h"
#include "../include/slackAPI.h"
#include "../include/stringTools.h"
#include "../include/wikiAPI.h"
#include "../include/sheetAPI.h"
#include "../include/command.h"
#include "../include/log.h"
#include "../include/requirements.h"

void syncSheetToDrl(command cmd){
    log_message(LOG_DEBUG, "Entering function syncSheetToDRL");

    refreshOAuthToken();

    pageList* drlPage = (pageList*) malloc(sizeof(pageList)); // Allocate memory for linkTrackerPage
    if (drlPage == NULL) {
        log_message(LOG_ERROR, "Memory allocation failed for drlPage");
        return;
    }
    char *sheetId;

    if(strcmp(cmd.argument_1, "ST")==0){
        drlPage->id = "420";
        sheetId = "ST!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "PR")==0){
        drlPage->id = "414";
        sheetId = "PR!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "FD")==0){
        drlPage->id = "416";
        sheetId = "FD!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "RE")==0){
        drlPage->id = "419";
        sheetId = "RE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "GS")==0){
        drlPage->id = "417";
        sheetId = "GS!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "AV")==0){
        drlPage->id = "421";
        sheetId = "AV!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "TE")==0){
        drlPage->id = 
        sheetId = "TE!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "PL")==0){
        drlPage->id = "418";
        sheetId = "PL!A3:AT300";
    }
    if(strcmp(cmd.argument_1, "GE")==0){
        drlPage->id = "415";
        sheetId = "GE!A3:AT300";
    }



    drlPage = getPage(&drlPage); //get content and updated at values

    cJSON *requirementList = cJSON_CreateObject();

    cJSON *documentName = NULL;
    documentName = cJSON_CreateString(getDocId(drlPage->path));
    cJSON_AddItemToObject(requirementList, "documentName", documentName);

    cJSON *requirements = NULL;
    requirements = cJSON_CreateArray();
    cJSON_AddItemToObject(requirementList, "requirements", requirements);

    parseRequirementsList(requirements, drlPage->content);

    char* output = parseJSONRequirementListInToArray(requirements);

    log_message(LOG_DEBUG, "calling batchUpdateSheet with values set to: %s", output);

    batchUpdateSheet("1i_PTwIqLuG9IUI73UaGuOvx8rVTDV1zIS7gmXNjMs1I", sheetId, output);
    
    log_message(LOG_DEBUG, "deallocating memory");
    cJSON_Delete(requirementList);
    free(output);
    
    log_message(LOG_DEBUG, "Exiting function syncSheetToDRL");
    return;
}

static void parseRequirementsList(cJSON* requirements, char *content) {
    log_message(LOG_DEBUG, "Entering function parseRequirementsList");
    
    char *idStartFlag = "\\n- [";
    char *pathStartFlag = "](/";
    char *nameStartFlag = ") **";
    char *descriptionStartFlag = "**\\n";
    char *descriptionEndFlag = "\\n";
    char *groupStartFlag ="\\n## ";
    char *groupEndFlag = idStartFlag;
    char *ptr = content;
    int isLastGroup = 0;


    while (*ptr) {
    
        char *idStart = strstr(ptr, idStartFlag);
        if (idStart == NULL){
            log_message(LOG_DEBUG, "did not find idStart, Breaking");
            break; // No more links
        }

        
        char *groupStart = strstr(ptr, groupStartFlag);
        if (groupStart == NULL){
            log_message(LOG_DEBUG, "did not find groupStart, setting isLastGroup to 1");
            isLastGroup = 1;
        }
        if (groupStart < idStart && isLastGroup == 0){
            int groupLength = idStart - groupStart - strlen(groupStartFlag);
            char *group = (char *)malloc((groupLength + 1) * sizeof(char));
            strncpy(group, groupStart + strlen(groupStartFlag), groupLength);
            group[groupLength] = '\0';
            addRequirementToCjsonObject(requirements, group, "", "", "");

            ptr = idStart;

            continue;
        }

        char *pathStart = strstr(idStart, pathStartFlag);
        if (pathStart == NULL){
            log_message(LOG_DEBUG, "did not find pathStart, Breaking");
            break; // No more links
        }

        char *nameStart = strstr(pathStart, nameStartFlag);
        if (nameStart == NULL){
            log_message(LOG_DEBUG, "did not find nameStart, Breaking");
            break; // No more links
        }

        char *descriptionStart = strstr(nameStart, descriptionStartFlag);
        if (descriptionStart == NULL){
            log_message(LOG_DEBUG, "did not find descriptionStart, Breaking");
            break; // No more links
        }

        char *descriptionEnd = strstr(descriptionStart + strlen(descriptionStartFlag), descriptionEndFlag);
        if (descriptionEnd == NULL){
            log_message(LOG_DEBUG, "did not find descriptionEnd, Breaking");
            break; // No more links
        }

        // Calculate title length and link length
        int idLength = pathStart - idStart - strlen(idStartFlag);
        int pathLength = nameStart - pathStart - strlen(pathStartFlag);
        int nameLength = descriptionStart - nameStart - strlen(nameStartFlag);
        int descriptionLength = descriptionEnd - descriptionStart - strlen(descriptionStartFlag);

        if (idLength <= 0 || pathLength <= 0 || nameLength <= 0 || descriptionLength <= 0) {
            ptr = descriptionEnd + strlen(descriptionEndFlag);
            continue;
        }

        // Allocate memory for the title and link
        char *id = (char *)malloc((idLength + 1) * sizeof(char));
        char *path = (char *)malloc((pathLength + 1) * sizeof(char));
        char *name = (char *)malloc((nameLength + 1) * sizeof(char));
        char *description = (char *)malloc((descriptionLength + 1) * sizeof(char));
        if (!id || !path || !name || !description) {
            log_message(LOG_ERROR, "Memory allocation error");
            if (id) free(id);
            if (path) free(path);
            if (name) free(name);
            if (description) free(description);
        }

        // Copy title and link
        strncpy(id, idStart + strlen(idStartFlag), idLength);
        id[idLength] = '\0';
        strncpy(path, pathStart + strlen(pathStartFlag), pathLength);
        path[pathLength] = '\0';
        strncpy(name, nameStart + strlen(nameStartFlag), nameLength);
        name[nameLength] = '\0';
        strncpy(description, descriptionStart + strlen(descriptionStartFlag), descriptionLength);
        description[descriptionLength] = '\0';

        log_message(LOG_DEBUG, "adding requirement with id: %s\npath: %s\nname: %s", id, path, name);
        addRequirementToCjsonObject(requirements, id, path, name, description);


        free(id);
        free(path);
        free(name);
        free(description);
        ptr = descriptionEnd;
    }
    
    log_message(LOG_DEBUG, "Exiting function parseRequirementsList");
    return;
}

static void addRequirementToCjsonObject(cJSON *requirements, char *idStr, char *pathStr, char *nameStr, char *descriptionStr){
    log_message(LOG_DEBUG, "Entering function addRequirementToCjsonObject");
    
    cJSON *id = NULL;
    cJSON *path = NULL;
    cJSON *name = NULL;
    cJSON *description = NULL;
    cJSON *requirement = NULL;

    requirement = cJSON_CreateObject();

    id = cJSON_CreateString(idStr);
    path = cJSON_CreateString(pathStr);
    name = cJSON_CreateString(nameStr);
    description = cJSON_CreateString(descriptionStr);

    cJSON_AddItemToArray(requirements, requirement);

    cJSON_AddItemToObject(requirement, "id", id);
    cJSON_AddItemToObject(requirement, "path", path);
    cJSON_AddItemToObject(requirement, "name", name);
    cJSON_AddItemToObject(requirement, "description", description);

    log_message(LOG_DEBUG, "Exiting function addRequirementToCjsonObject");

}

static char* parseJSONRequirementListInToArray(cJSON* requirements){
    log_message(LOG_DEBUG, "Entering function parseJSONRequirementListInToArray");
    

    // Get the number of requirements
    int req_count = cJSON_GetArraySize(requirements);

    // Calculate the required buffer size for the output string
    size_t buffer_size = 1024; // Initial size, will be adjusted as needed
    for (int i = 0; i < req_count; i++) {
        cJSON *req = cJSON_GetArrayItem(requirements, i);
        const char *id = cJSON_GetObjectItem(req, "id")->valuestring;
        const char *path = cJSON_GetObjectItem(req, "path")->valuestring;
        const char *name = cJSON_GetObjectItem(req, "name")->valuestring;
        const char *description = cJSON_GetObjectItem(req, "description")->valuestring;
        buffer_size += strlen(id) + strlen(path) + strlen(name) + strlen(description) + 20; // Adding extra space for formatting
    }

    // Allocate memory for the output string
    char *output_str = (char *)malloc(buffer_size * sizeof(char));
    if (!output_str) {
        log_message(LOG_ERROR, "Memory allocation failed");
        return NULL;
    }
    output_str[0] = '\0'; // Initialize the string

    // Build the output string
    strcat(output_str, "[");
    for (int i = 0; i < req_count; i++) {
        cJSON *req = cJSON_GetArrayItem(requirements, i);
        const char *id = cJSON_GetObjectItem(req, "id")->valuestring;
        const char *path = cJSON_GetObjectItem(req, "path")->valuestring;
        const char *name = cJSON_GetObjectItem(req, "name")->valuestring;
        const char *description = cJSON_GetObjectItem(req, "description")->valuestring;

        char req_str[1000]; // Buffer for the current requirement string
        snprintf(req_str, sizeof(req_str), "[\"%s\", \"%s\", \"%s\"]", id, name, description);

        if (i > 0) {
            strcat(output_str, ", ");
        }
        strcat(output_str, req_str);
    }
    strcat(output_str, "]");

    // Clean up the JSON object

    log_message(LOG_DEBUG, "Exiting function parseJSONRequirementListInToArray");
    return output_str;

}


