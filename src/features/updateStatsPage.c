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

void updateStatsPage(command cmd){
    log_message(LOG_DEBUG, "Entering function updateStatsPage");
    
    pageList* head = NULL;
    head = populatePageList(&head, "time", "none");

    pageList* current = head;

    const char* linksListOfPages_header = "# Recently Edited Pages\n";

    char *linksListOfPages = (char *)malloc(strlen(linksListOfPages_header) + 1);

    strcpy(linksListOfPages, linksListOfPages_header);

    for(int i = 0; i < 5; i++){
        linksListOfPages = appendToString(linksListOfPages, "- [");
        linksListOfPages = appendToString(linksListOfPages, current->title);
        linksListOfPages = appendToString(linksListOfPages, "](/");
        linksListOfPages = appendToString(linksListOfPages, current->path);
        linksListOfPages = appendToString(linksListOfPages, ")\n**Updated on:** ");
        linksListOfPages = appendToString(linksListOfPages, convert_timestamp_to_cest(current->updatedAt));
        linksListOfPages = appendToString(linksListOfPages, "\n");
        current = current->next;
    }

    linksListOfPages = appendToString(linksListOfPages, "{.links-list}");

    linksListOfPages = appendToString(linksListOfPages, "\n\n# Page Distribution\n");
    linksListOfPages = appendToString(linksListOfPages, buildPageDistributionPieChart(head));

    freePageList(&head);
    linksListOfPages = replaceWord(linksListOfPages, "\n", "\\\\n");
    linksListOfPages = replaceWord(linksListOfPages, "\"", "\\\\\\\"");

    pageList* statsPage = NULL;
    statsPage = addPageToList(&statsPage, "1178", "", "", "", linksListOfPages, "", "", "");
    log_message(LOG_DEBUG, "statsPage id set");
    updatePageContentMutation(statsPage);
    renderMutation(&statsPage, false);
    sendMessageToSlack("stats page updated");

    free(linksListOfPages);

    log_message(LOG_DEBUG, "Exiting function updateStatsPage");
}

static char* buildPageDistributionPieChart(pageList* head){
    log_message(LOG_DEBUG, "Entering function buildPageDistributionPieChart");
    
    pageList* current = head;

    int competitionCount = 0;
    int icarusCount = 0;
    int hyperionCount = 0;
    int managementCount = 0;
    int spaceraceCount = 0;
    int otherCount = 0;

    while(current){

        if(strstr(current->path, "competition/")){
            competitionCount++;
        }
        else if(strstr(current->path, "icarus/")){
            icarusCount++;
        }
        else if(strstr(current->path, "hyperion/")){
            hyperionCount++;
        }
        else if(strstr(current->path, "management/")){
            managementCount++;
        }
        else if(strstr(current->path, "space_race/")){
            spaceraceCount++;
        }
        else{
            otherCount++;
        }
        current = current->next;
    }

    char *pieChart = "```kroki\nvega\n\n{\n  \"$schema\": \"https://vega.github.io/schema/vega/v5.0.json\",\n  \"width\": 350,\n  \"height\": 350,\n  \"autosize\": \"pad\",\n  \"signals\": [\n    {\"name\": \"startAngle\", \"value\": 0},\n    {\"name\": \"endAngle\", \"value\": 6.29},\n    {\"name\": \"padAngle\", \"value\": 0},\n    {\"name\": \"sort\", \"value\": true},\n    {\"name\": \"strokeWidth\", \"value\": 2},\n    {\n      \"name\": \"selected\",\n      \"value\": \"\",\n      \"on\": [{\"events\": \"mouseover\", \"update\": \"datum\"}]\n    }\n  ],\n  \"data\": [\n    {\n      \"name\": \"table\",\n      \"values\": [\n        {\"continent\": \"Competition\", \"population\": $competitionCount$},\n        {\"continent\": \"Hyperion\", \"population\": $hyperionCount$},\n        {\"continent\": \"Icarus\", \"population\": $icarusCount$},\n        {\"continent\": \"Space Race\", \"population\": $spaceraceCount$},\n        {\"continent\": \"Management\", \"population\": $managementCount$},\n        {\"continent\": \"Other\", \"population\": $otherCount$}\n      ],\n      \"transform\": [\n        {\n          \"type\": \"pie\",\n          \"field\": \"population\",\n          \"startAngle\": {\"signal\": \"startAngle\"},\n          \"endAngle\": {\"signal\": \"endAngle\"},\n          \"sort\": {\"signal\": \"sort\"}\n        }\n      ]\n    },\n    {\n      \"name\": \"fieldSum\",\n      \"source\": \"table\",\n      \"transform\": [\n        {\n          \"type\": \"aggregate\",\n          \"fields\": [\"population\"],\n          \"ops\": [\"sum\"],\n          \"as\": [\"sum\"]\n        }\n      ]\n    }\n  ],\n  \"legends\": [\n    {\n      \"fill\": \"color\",\n      \"title\": \"Legends\",\n      \"orient\": \"none\",\n      \"padding\": {\"value\": 10},\n      \"encode\": {\n        \"symbols\": {\"enter\": {\"fillOpacity\": {\"value\": 1}}},\n        \"legend\": {\n          \"update\": {\n            \"x\": {\n              \"signal\": \"(width / 2) + if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.1 * 0.8, if(width >= height, height, width) / 2 * 0.8)\",\n              \"offset\": 20\n            },\n            \"y\": {\"signal\": \"(height / 2)\", \"offset\": -50}\n          }\n        }\n      }\n    }\n  ],\n  \"scales\": [\n    {\"name\": \"color\", \"type\": \"ordinal\", \"range\": [\"#03071e\", \"#370617\", \"#9d0208\", \"#dc2f02\", \"#f48c06\", \"#ffba08\"]}\n  ],\n  \"marks\": [\n    {\n      \"type\": \"arc\",\n      \"from\": {\"data\": \"table\"},\n      \"encode\": {\n        \"enter\": {\n          \"fill\": {\"scale\": \"color\", \"field\": \"continent\"},\n          \"x\": {\"signal\": \"width / 2\"},\n          \"y\": {\"signal\": \"height / 2\"}\n        },\n        \"update\": {\n          \"startAngle\": {\"field\": \"startAngle\"},\n          \"endAngle\": {\"field\": \"endAngle\"},\n          \"cornerRadius\": {\"value\": 15},\n          \"padAngle\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, 0.015, 0.015)\"\n          },\n          \"innerRadius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 0.45, if(width >= height, height, width) / 2 * 0.5)\"\n          },\n          \"outerRadius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.05 * 0.8, if(width >= height, height, width) / 2 * 0.8)\"\n          },\n          \"opacity\": {\n            \"signal\": \"if(selected && selected.continent !== datum.continent, 1, 1)\"\n          },\n          \"stroke\": {\"signal\": \"scale('color', datum.continent)\"},\n          \"strokeWidth\": {\"signal\": \"strokeWidth\"},\n          \"fillOpacity\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, 0.8, 0.8)\"\n          }\n        }\n      }\n    },\n    {\n      \"type\": \"text\",\n      \"encode\": {\n        \"enter\": {\"fill\": {\"value\": \"#525252\"}, \"text\": {\"value\": \"\"}},\n        \"update\": {\n          \"opacity\": {\"value\": 1},\n          \"x\": {\"signal\": \"width / 2\"},\n          \"y\": {\"signal\": \"height / 2\"},\n          \"align\": {\"value\": \"center\"},\n          \"baseline\": {\"value\": \"middle\"},\n          \"fontSize\": {\"signal\": \"if(width >= height, height, width) * 0.05\"},\n          \"text\": {\"value\": \"Page Distribution\"}\n        }\n      }\n    },\n    {\n      \"name\": \"mark_population\",\n      \"type\": \"text\",\n      \"from\": {\"data\": \"table\"},\n      \"encode\": {\n        \"enter\": {\n          \"text\": {\n            \"signal\": \"if(datum['endAngle'] - datum['startAngle'] < 0.3, '', format(datum['population'] / 1, '.0f'))\"\n          },\n          \"x\": {\"signal\": \"if(width >= height, height, width) / 2\"},\n          \"y\": {\"signal\": \"if(width >= height, height, width) / 2\"},\n          \"radius\": {\n            \"signal\": \"if(selected && selected.continent == datum.continent, if(width >= height, height, width) / 2 * 1.05 * 0.65, if(width >= height, height, width) / 2 * 0.65)\"\n          },\n          \"theta\": {\"signal\": \"(datum['startAngle'] + datum['endAngle'])/2\"},\n          \"fill\": {\"value\": \"#FFFFFF\"},\n          \"fontSize\": {\"value\": 12},\n          \"align\": {\"value\": \"center\"},\n          \"baseline\": {\"value\": \"middle\"}\n        }\n      }\n    }\n  ]\n}\n```";

    char strcompetitionCount[10];
    sprintf(strcompetitionCount, "%d", competitionCount);

    char stricarusCount[10];
    sprintf(stricarusCount, "%d", icarusCount);

    char strhyperionCount[10];
    sprintf(strhyperionCount, "%d", hyperionCount);

    char strmanagementCount[10];
    sprintf(strmanagementCount, "%d", managementCount);

    char strspaceraceCount[10];
    sprintf(strspaceraceCount, "%d", spaceraceCount);

    char strotherCount[10];
    sprintf(strotherCount, "%d", otherCount);

    pieChart = replaceWord(pieChart, "$competitionCount$", strcompetitionCount);
    pieChart = replaceWord(pieChart, "$icarusCount$", stricarusCount);
    pieChart = replaceWord(pieChart, "$hyperionCount$", strhyperionCount);
    pieChart = replaceWord(pieChart, "$managementCount$", strmanagementCount);
    pieChart = replaceWord(pieChart, "$spaceraceCount$", strspaceraceCount);
    pieChart = replaceWord(pieChart, "$otherCount$", strotherCount);

    log_message(LOG_DEBUG, "Exiting function buildPageDistributionPieChart");
    return pieChart;
}

static char* convert_timestamp_to_cest(char *timestamp) {
    struct tm tm;
    time_t raw_time;
    char *output_buffer = malloc(100); // Allocate memory for the output string

    if (output_buffer == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    // Initialize the struct tm to avoid any potential issues
    memset(&tm, 0, sizeof(struct tm));

    // Parse the timestamp into a struct tm (UTC time)
    if (strptime(timestamp, "%Y-%m-%dT%H:%M:%S", &tm) == NULL) {
        fprintf(stderr, "Failed to parse timestamp.\n");
        free(output_buffer);
        return NULL;
    }

    // Convert the struct tm to time_t (in UTC)
    raw_time = timegm(&tm);

    // Add 2 hours for CEST (Central European Summer Time)
    raw_time += 2 * 3600;

    // Convert back to struct tm in local time (CEST)
    struct tm *cest_time = localtime(&raw_time);

    // Format the CEST time into a readable string
    if (strftime(output_buffer, 100, "%A, %B %d at %H:%M:%S", cest_time) == 0) {
        fprintf(stderr, "Failed to format time.\n");
        free(output_buffer);
        return NULL;
    }

    return output_buffer; // Return the formatted string
}