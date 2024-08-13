#ifndef MARKDOWNTOPDF_H
#define MARKDOWNTOPDF_H

/**
 * @brief retrieves a page from the wiki, processes, and saves it as a latex file locally.
 * 
 * This function handles the complete workflow of processing a page from retrieving content, downloading images, 
 * preprocessing content, generating LaTeX and ZIP files, and finally sending notifications.
 * 
 * @param cmd The command structure containing arguments for processing.
 * 
 * @details
 * - Sets the `id` of the temporary page to the provided argument from the command.
 * - Retrieves the page content based on the `id`.
 * - Downloads associated images.
 * - Generates paths for text, LaTeX, and ZIP files based on the document ID.
 * - Opens a text file for writing and writes the preprocessed content to it.
 * - Converts the Markdown file to LaTeX format.
 * - Applies default image scaling and positioning to the LaTeX file.
 * - Prepends a template file to the LaTeX file.
 * - Populates the LaTeX file with the document title and header information.
 * - Zips the folder containing the LaTeX and related files.
 * - Sends a message to Slack notifying about the file.
 * 
 * @note The function relies on several helper functions (`getPage`, `downloadImages`, `preProcessing`, `markdownToLatex`,
 *       `applyDefaultImageScaleAndPosition`, `prepend_file`, `populateTitleAndHeader`, `zipFolder`, `sendMessageToSlack`).
 *       Ensure that these functions are properly implemented and tested.
 */
void getZip(command cmd);

#endif