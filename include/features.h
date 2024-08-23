#ifndef FEATURES_H
#define FEATURES_H

void replaceText(command cmd);

/**
 * @brief Moves a page on the wiki to a new path and updates all references to it.
 * 
 * This function moves a page from its current path to a new path and updates all references to the page in other documents. It also updates the link tracker page to reflect these changes.
 * 
 * @param cmd A command structure containing the following:
 * - `cmd.argument_1`: The current path of the page to be moved.
 * - `cmd.argument_2`: The new path to which the page should be moved.
 * 
 * @details
 * - The function first sanitizes the paths by removing backslashes.
 * - It then retrieves the page to be moved and updates its path.
 * - It updates the content of all pages that reference the moved page, replacing occurrences of the old path with the new path.
 * - The function also updates the link tracker page to reflect the changes in paths.
 * - After updating the content, the function renders the changes and frees the allocated memory.
 */
void movePage(command cmd);

/**
 * @brief Builds a map of all of the files in a given directory.
 * 
 * This function populates a list of daughter pages from a specified path, generates a map based on these pages,
 * and returns the map as a string. After generating the map, it cleans up by freeing the memory used for the page list.
 * 
 * @param cmd A command structure containing `argument_1`, which specifies the path to the mother file.
 * 
 * @return A dynamically allocated string containing the generated map. The caller is responsible for freeing this string.
 * 
 * @details 
 * - The function first calls `populatePageList` to generate a linked list of daughter pages based on the given path.
 * - It then calls `createMapWBS` to generate the map in Work Breakdown Structure (WBS) format.
 * - Finally, the function cleans up by freeing the linked list and returns the generated map as a string.
 */
char* buildMap(command cmd);

/**
 * @brief Builds/populates the link tracker page.
 * 
 * This function generates a link tracker page by iterating through all the pages in the system, 
 * identifying links on each page, and appending this information to the link tracker page. 
 * The link tracker page is then updated with the collected data.
 * 
 * 
 * @details
 * - The function begins by creating and retrieving the link tracker page.
 * - It then populates a list of all pages, excluding the link tracker and broken links tracker pages.
 * - For each page, the function retrieves its content, finds links within it, and updates the link tracker page with this information.
 * - Finally, the link tracker page is updated and rendered, and all allocated memory for the page lists is freed.
 * 
 * @todo update in order to format the link tracker page in JSON
 */
void buildLinksTracker( );

/**
 * @brief Updates the link tracker page with information from recently modified pages.
 * 
 * This function updates the link tracker page by examining pages that have been modified after the last update of the link tracker page.
 * It identifies links in these pages and appends or updates the link tracker page accordingly.
 *  
 * @details
 * - The function begins by allocating memory for and retrieving the link tracker page.
 * - It then fetches a list of pages that have been modified after the last update of the link tracker page.
 * - For each recently modified page, the function retrieves its content, identifies any links, and updates the link tracker page.
 * - The function ensures that the content of the link tracker page is properly formatted before updating and rendering it.
 * - Finally, the function frees the allocated memory for the page lists.
 * 
 * @todo rewrite the code in order to save the information in JSON format, test code.
 * @warning the code is untested so assume that it does not work in the current state.
 */
void updateLinksTracker( );

void buildImageTracker(command cmd);

/**
 * @brief Builds a local graph for a specified page and returns it as a string.
 * 
 * This function creates a local graph by identifying incoming and outgoing links related to a specified page. It constructs a mind map that represents these connections and returns it as a string.
 * 
 * @param cmd A command structure containing the path of the subject page in `cmd.argument_1`.
 * @return char* A string representing the local graph mind map.
 * 
 * @details
 * - The function begins by allocating memory for the link tracker page and retrieves its content.
 * - It then fetches the subject page based on the path provided in `cmd.argument_1`.
 * - Incoming and outgoing links related to the subject page are identified using the content of the link tracker page.
 * - A local graph mind map is generated using the subject page, incoming links, and outgoing links.
 * - The generated graph is returned as a string.
 */
char* buildLocalGraph(command cmd);

/**
 * @brief Builds a list of acronyms from the content of pages and writes them to a file.
 * 
 * This function iterates through a list of pages, extracts acronyms from each page's content, and writes them to a specified file. The resulting acronym list is then processed to remove duplicates and sort the entries.
 * 
 * @param cmd A command structure containing an optional path in `cmd.argument_1`. If `cmd.argument_1` is NULL, all pages are considered.
 * 
 * @details
 * - The function starts by populating the list of all pages based on the provided path or considers all pages if no path is specified.
 * - The content of each page is retrieved and acronyms are extracted and printed to a file located at `ACCRONYM_LIST_PATH`.
 * - After processing all pages, the list of acronyms is cleaned by removing duplicates and sorting the entries.
 * - Memory used by the page list is freed after processing.
 */
void buildAcronymsList(command cmd);

/**
 * @brief Retrieves and prints a list of pages based on a specified path.
 * 
 * This function retrieves pages based on the path provided in the `cmd` structure and prints the details of each page. The function then sends a confirmation message to Slack and frees the allocated memory.
 * 
 * @param cmd A command structure containing the path in `cmd.argument_1` used to filter pages.
 * 
 * @details
 * - The function first populates a linked list of pages using the specified path.
 * - It then prints the details of each page in the list using `printPages`.
 * - A confirmation message "All Pages Printed" is sent to Slack after printing.
 * - Finally, the memory allocated for the linked list is freed.
 */
void getPages(command cmd);

/**
 * @brief Synchronizes data from a Design Requirements List page on the wiki to a Google Sheets document.
 * 
 * This function extracts requirement data from a specified DRL page and synchronizes it with a Google Sheets document. The DRL page content is parsed and converted into a format compatible with the Google Sheets API, and then it is used to update the specified sheet.
 * 
 * @param cmd A command structure containing the following:
 * - `cmd.argument_1`: Not used in this function, but part of the `command` structure.
 * 
 * @details
 * - Allocates memory for a `pageList` structure to hold the DRL page data and retrieves the page content.
 * - Creates a JSON object with the document name and requirements list.
 * - Parses the requirements from the DRL page content and adds them to the JSON object.
 * - Converts the JSON requirements list to a format suitable for Google Sheets and updates the specified sheet using the `batchUpdateSheet` function.
 * - Frees allocated memory and handles error cases where memory allocation fails.
 * 
 * @note
 * - The `batchUpdateSheet` function requires a valid Google Sheets API token and sheet ID, which are hardcoded in this function.
 * 
 * @todo update so that it can be used with any DRL page (not hardcoded to a single page), needs to be compatible with tabs
 */
void syncSheetToDrl(command cmd);

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
 * 
 * @todo update so that it can be used with any DRL page (not hardcoded to a single page), needs to be compatible with tabs
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
void createRequirementPage(command cmd);

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
void createVcdPage(command cmd);

void onPageUpdate(command cmd);

void updateStatsPage(command cmd);
#endif