#ifndef WIKI_LINKS_HELPER_H
#define WIKI_LINKS_HELPER_H


/**
 * @brief Creates a formatted list of links for a given section title.
 * 
 * @param list The initial string to which the list will be appended.
 * @param sectionTitle Pointer to a `pageList` structure containing the section title and path.
 * @param links Pointer to a `pageList` structure representing a list of links to be included under the section.
 * 
 * @return A dynamically allocated string containing the formatted list.
 * 
 * @details This function generates a formatted list in Markdown style, starting with a section header based on the `sectionTitle` and 
 *          followed by a list of links. It appends the section title as a link to the section's path, then adds each link from the `links` 
 *          list, each on a new line. The resulting string is dynamically allocated and should be freed by the caller when no longer needed.
 * 
 *          The function uses `appendStrings` to concatenate strings. After building the list, it calls `freePageList` to free the memory 
 *          allocated for the `links` list.
 */
char* createList(char *list, pageList** sectionTitle, pageList* links);

/**
 * @brief Extracts page links from the given content and adds them to a linked list.
 * 
 * This function parses the provided content to find all page links formatted as `[title](link)`. It extracts the title
 * and link for each found page link, allocates memory for them, and adds them to a linked list of `pageList` structures.
 * 
 * @param content The text content from which page links are to be extracted. The content should contain links in the
 *                format `[title](link)`.
 * @param links A pointer to a pointer to a `pageList` structure. This is used to build the linked list of found page links.
 * 
 * @return A pointer to the updated linked list of `pageList` structures containing the extracted links and titles.
 * 
 * @details The function performs the following steps:
 *          - Searches for the start and end of each page link using delimiters `](` and `)`.
 *          - Extracts the title and link from each page link and allocates memory for them.
 *          - Adds the extracted page link and title to the linked list of `pageList` structures.
 *          - Handles potential memory allocation errors by freeing previously allocated memory and calling `freePageList`.
 */
pageList* findPageLinks(char *content, pageList** links);

/**
 * @brief Extracts image links from the given input and adds them to a linked list.
 * 
 * This function scans the provided text input to find all image links formatted as `![alt_text](link)`. It extracts the
 * link for each found image link, allocates memory for it, and adds it to a linked list of `pageList` structures.
 * 
 * @param input The text content from which image links are to be extracted. The content should contain links in the
 *              format `![alt_text](link)`.
 * @param head A pointer to a pointer to a `pageList` structure. This is used to build the linked list of found image links.
 * 
 * @return A pointer to the updated linked list of `pageList` structures containing the extracted image links.
 * 
 * @details The function performs the following steps:
 *          - Searches for image link patterns using delimiters `![` and `](`.
 *          - Extracts the link from each image link and allocates memory for it.
 *          - Adds the extracted link to the linked list of `pageList` structures.
 *          - Handles potential memory allocation errors by freeing previously allocated memory and calling `freePageList`.
 */
pageList* findImageLinks(char *input, pageList** head);

/**
 * @brief Filters and cleans up the paths in the linked list of page links. Used to remove sizing and positioning options
 *        from images on the wiki.
 * 
 * This function iterates through each `pageList` node in the linked list and processes the `path` field. It removes any
 * characters that appear after the first space in the path, effectively cleaning up the paths to ensure they do not contain
 * unwanted trailing information.
 * 
 * @param head A pointer to a pointer to the head of the linked list of `pageList` structures. The function modifies
 *             the `path` field of each node in the list.
 * 
 * @details The function performs the following steps:
 *          - Iterates through each node in the linked list.
 *          - Calls `removeAfterSpace` on the `path` field of each node to remove any characters after the first space.
 * 
 * @warning Used for wiki to pdf conversion
 */
void filterLinks(pageList** head);

/**
 * @brief Finds and adds incoming links to a linked list. The links must reference a specific subject page.
 * 
 * This function searches through the provided content for sections that reference links. It extracts titles and paths from
 * these sections and adds them to the `incomingLinks` list if they match the `subjectPagePath`. The function operates on a
 * copy of the content to avoid modifying the original content.
 * 
 * @param head A pointer to a pointer to the head of the linked list where the incoming links will be added.
 * @param linkTrackerContent A string containing the content to be searched for incoming links.
 * @param subjectPagePath The path of the subject page being searched for in the content.
 * 
 * @return A pointer to the updated list of incoming links, which includes any newly found links that reference the
 *         `subjectPagePath`.
 * 
 * @details The function performs the following steps:
 *          - Creates a copy of the `linkTrackerContent` to process.
 *          - Searches for sections in the content that start with "## [".
 *          - Extracts titles, paths, and links from these sections.
 *          - Compares each link to `subjectPagePath`. If a match is found, it adds the corresponding title and path to the
 *            `incomingLinks` list.
 *          - Updates the pointers to continue searching through the content.
 *          - Frees the allocated memory for the content copy before returning.
 * 
 * @todo replace with a cJSON version
 */
pageList* findIncomingLinks(pageList **head, char *linkTrackerContent, char *subjectPagePath);

#endif