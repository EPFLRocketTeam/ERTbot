#ifndef ERTBOT_FEATURES_H
#define ERTBOT_FEATURES_H

#include "ERTbot_common.h"


/**
 * @brief Synchronizes data from a Google Sheets document to a DRL page ont the wiki.
 * 
 * This function retrieves data from a specified range in a Google Sheets document, parses the data into a JSON requirements list, and updates the content of a DRL page with the formatted data. The DRL page is then updated and rendered with the new content.
 * 
 * @param cmd A command structure containing the following:
 * - `cmd.argument_1`: Not used in this function, but part of the `command` structure.
 * 
 * @details
 * - Retrieves data from a Google Sheets document using `batchGetSheet` and parses the response into a JSON requirements list.
 * - Converts the JSON requirements list into a DRL format using `buildDrlFromJSONRequirementList`.
 * - Allocates and updates a DRL page with the new content, ensuring special characters are escaped correctly.
 * - Updates the DRL page content using `updatePageContentMutation` and renders it with `renderMutation`.
 * - Frees allocated memory and handles JSON and page list deallocation.
 */
void syncDrlToSheet(command cmd);

/**
 * @brief Creates and updates a requirement page based on data from a Google Sheets document.
 * 
 * This function retrieves data from a specified range in a Google Sheets document, parses the data into a JSON requirements list, and builds a requirement page with the formatted content. The content of the page is updated and rendered accordingly.
 * 
 * @param cmd A command structure containing the following:
 * - `cmd.argument_1`: The identifier for the requirement page to be created.
 * 
 * @details
 * - Retrieves data from a Google Sheets document using `batchGetSheet` and parses the response into a JSON requirements list with `parseArrayIntoJSONRequirementList`.
 * - Creates a requirement page using `buildRequirementPageFromJSONRequirementList`, incorporating data from the JSON requirements list.
 * - Escapes newline and quote characters in the page content for proper formatting.
 * - Updates the requirement page content using `updatePageContentMutation` and renders it with `renderMutation`.
 * - Frees allocated memory for the JSON requirements list and page list.
 * 
 * @warning not finished yet
 */
void updateRequirementPage(command cmd);

/**
 * @brief Creates and updates a VCD (Verification Control Document) page based on data from a Google Sheets document and chart updates.
 * 
 * This function retrieves data from a Google Sheets document, processes and updates VCD content, and integrates charts into the VCD page. The updated content is then used to refresh the VCD page.
 * 
 * @param cmd A command structure. This function currently does not use `cmd` directly but may rely on data fetched from a Google Sheets document.
 * 
 * @details
 * - Retrieves data from a specified range in a Google Sheets document using `batchGetSheet` and parses the response into a JSON requirements list with `parseArrayIntoJSONRequirementList`.
 * - Generates a pie chart using `createVcdPieChart`.
 * - Initializes and fetches content for the VCD draft page using `addPageToList` and `getPage`.
 * - Extracts and processes text from the VCD draft page to update with new chart data and status history using `extractText` and `updateVcdStackedAreaChart`.
 * - Updates the VCD draft content with the new stacked area chart and status history.
 * - Frees allocated memory for temporary strings and the VCD draft page.
 * 
 * @warning NOT FINISHED
 */
void updateVcdPage(command cmd);

void createMissingRequirementPages(command cmd);
#endif