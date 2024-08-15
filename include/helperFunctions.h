#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#define MAX_WORD_LENGTH 20
#define MAX_WORDS 1000

/**
 * @brief Adds a new page to the end of a linked list of pages.
 * 
 * @param[in, out] head Pointer to the pointer of the `pageList` structure, representing the head of the linked list of pages.
 * @param[in] id The unique identifier for the page.
 * @param[in] title The title of the page.
 * @param[in] path The path of the page.
 * @param[in] description A description of the page.
 * @param[in] content The content of the page.
 * @param[in] updatedAt The timestamp of when the page was last updated.
 * @param[in] createdAt The timestamp of when the page was created.
 * 
 * @return A pointer to the head of the updated `pageList` with the new page added.
 * 
 * @details This function allocates memory for a new `pageList` node, copies the provided page details into the new node, 
 *          and appends it to the end of the linked list. If the list is empty, the new node becomes the head of the list.
 * 
 * @note Ensure that the provided strings (`id`, `title`, `path`, `description`, `content`, `updatedAt`, `createdAt`) are 
 *       valid and properly formatted. Memory allocated for the new node's strings should be freed when the node is no 
 *       longer needed to prevent memory leaks.
 */
pageList* addPageToList(pageList** head,  char *id, char *title, char *path, char *description, char *content, char *updatedAt, char *createdAt);

/**
 * @brief Counts the number of '/' characters in a given string.
 * 
 * @param[in] str The input string to be analyzed.
 * 
 * @return int The number of '/' characters found in the string.
 * 
 * @details This function iterates through the provided string and counts the occurrences of the '/' character. It returns 
 *          the total count.
 * 
 * @note The input string must be a valid null-terminated string. Ensure that the input string is not NULL to avoid undefined 
 *       behavior.
 */
int countSlashes(char *str);

/**
 * @brief Prepends the contents of one file to another file.
 * 
 * @param[in] source_filename The name of the file whose contents are to be prepended.
 * @param[in] destination_filename The name of the file to which the contents are prepended.
 * 
 * @details This function opens the source file for reading and the destination file for both reading and writing. It reads the 
 *          contents of the destination file, then writes the contents of the source file to the beginning of the destination file,
 *          followed by the original contents of the destination file. The function handles memory allocation for buffering and 
 *          ensures that all file operations are properly managed.
 * 
 * @note Ensure that the source and destination filenames are valid and accessible. The destination file is opened in read-write 
 *       mode, and the source file is opened in read mode. This function does not handle file access errors beyond basic checks.
 */
void prepend_file(char *source_filename, char *destination_filename);

/**
 * @brief Zips a folder into a zip file.
 * 
 * @param[in] folderPath The path to the folder to be zipped.
 * 
 * @return Returns 0 on success, or 1 if the zip operation fails.
 * 
 * @details This function constructs a shell command to zip the specified folder using the `zip` command-line utility. It changes the 
 *          directory to the specified folder, creates a zip file with the same name as the folder, and includes all contents of 
 *          the folder in the zip file. The function uses `system()` to execute the command and checks the result to determine if 
 *          the operation was successful.
 * 
 * @note The function assumes that the `zip` command-line utility is installed and available in the system's PATH. Error handling 
 *       is basic, focusing on whether the `system()` call succeeds or fails.
 */
int zipFolder(char *folderPath);

/**
 * @brief Creates any missing directories in the specified path.
 * 
 * @param[in] path The full path of directories to create. The function will create directories as needed, including any intermediate
 *                 directories that do not exist.
 * 
 * @details This function processes the given path, creating directories one by one. It starts from the root of the path and works its
 *          way down, creating each directory in turn if it does not already exist. The path is tokenized by '/' to handle each directory
 *          level. It uses the `mkdir` function to create directories and checks for existing directories with `stat`. If an error occurs 
 *          while creating a directory, the function prints an error message and stops.
 * 
 * @note The function assumes that the path is a valid string and that the application has the necessary permissions to create directories 
 *       in the specified location. The `mkdir` function is used with mode `0755`, which sets the directory permissions to readable and 
 *       executable by everyone and writable by the owner.
 */
void createMissingFolders(char *path);

/**
 * @brief Retrieves the current time formatted as ISO 8601.
 * 
 * @return char* A string containing the current time in ISO 8601 format (UTC). The format is "YYYY-MM-DDTHH:MM:SS.000Z".
 * 
 * @details This function obtains the current UTC time and formats it into an ISO 8601 string representation. The formatted time is printed
 *          to the standard output. The function uses `strftime` to format the time according to the ISO 8601 standard, and it returns a
 *          static buffer containing the formatted time.
 */
char *getCurrentEDTTimeString();

/**
 * @brief Compares two ISO 8601 formatted timestamps.
 * 
 * @param time1 A string representing the first timestamp in ISO 8601 format (e.g., "YYYY-MM-DDTHH:MM:SS").
 * @param time2 A string representing the second timestamp in ISO 8601 format (e.g., "YYYY-MM-DDTHH:MM:SS").
 * 
 * @return int Returns -1 if `time1` is earlier than `time2`, 1 if `time1` is later than `time2`, and 0 if both timestamps are equal.
 * 
 * @details This function parses the provided ISO 8601 formatted strings into `struct tm` structures, converts these structures to `time_t`
 *          values using `mktime`, and then compares the resulting `time_t` values. It assumes the timestamps are in UTC and formatted
 *          correctly. The function adjusts year and month values to match the `struct tm` representation requirements.
 */
int compareTimes(char* time1, char* time2);

/**
 * @brief Replaces occurrences of a string in the content of all pages in a linked list, updates and rerenders the pages on the wiki.
 * 
 * @param head Pointer to the head of the linked list of pages.
 * @param oldString The string to be replaced in the page content.
 * @param newString The string to replace `oldString` with.
 * 
 * @details This function iterates through each page in the linked list, retrieves the page content using `getPage`, replaces occurrences
 *          of `oldString` with `newString`, escapes backslashes and quotes in the content, and updates the page content on the wiki
 *          using `updatePageContentMutation`. It then performs a render mutation for each page to apply the changes using `renderMutation`.
 *          After processing all pages, it frees the memory allocated for the page list using `freePageList` and sends a message to Slack
 *          indicating which pages have been updated.
 */
void replaceStringInWiki(pageList** head, char* oldString, char* newString);

/**
 * @brief Creates a graph of a directory in the Work Break Down format from a list of pages.
 * 
 * @param paths Pointer to the head of the linked list of pages.
 * 
 * @return A dynamically allocated string containing the WBS map in PlantUML format.
 * 
 * @details This function generates a WBS map in PlantUML format from a linked list of pages. It traverses the list of pages, calculates the 
 *          depth of each page based on its path, and builds a hierarchical WBS structure. The WBS structure is represented using asterisks 
 *          (`*`) to indicate levels of hierarchy. For each page, it adds an entry to the WBS with a hyperlink to the page and its title. 
 *          The function handles cases where pages are not directly related to each other by adding parent folders and adjusting the depth accordingly. 
 *          The resulting map is enclosed in PlantUML syntax for rendering.
 * 
 *          The function uses `countSlashes` to determine the depth of each page, `getDirPath` to find parent directories, and `getDocId` to 
 *          extract the document ID from the path. It uses `appendStrings` to build the final map string. The output string is dynamically 
 *          allocated and should be freed by the caller when no longer needed.
 */
char* createMapWBS(pageList** paths);

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
 * @brief Updates a specific section in a Markdown list with new links.
 * 
 * This function updates a section of a Markdown list by replacing an old list with a new one. 
 * The section to be updated is identified by a path, and the new list of links is appended to this section.
 * 
 * @param list The original Markdown list as a string. This string will be modified to include the updated list.
 * @param sectionTitle A pointer to a `pageList` structure representing the section to be updated. The path in this structure is used to identify the section.
 * @param links A pointer to a `pageList` structure containing the new list of links to be added under the section.
 * 
 * @return The updated Markdown list as a dynamically allocated string. The caller is responsible for freeing this memory.
 * 
 * @details The function appends the new list of links to the specified section in the original Markdown list. It locates the section in the list using 
 *          the `sectionTitle` path and replaces the old list with the new one. If the section or end of the section is not found, it prints an error 
 *          message and returns the original list. The memory allocated for the new list is freed before returning.
 * 
 * @todo Should be replaced or at least rewritten so that it uses cJSON functions
 */
char* updateList(char *list, pageList *sectionTitle, pageList* links);

/**
 * @brief Parses a text to extract and link wiki flags based on specific comment markers.
 * 
 * This function scans the provided text for comments marked with "<!--" and "-->". It extracts command information 
 * from these comments and creates a linked list of `wikiFlag` structures. The `wikiFlag` structures are populated 
 * with pointers to the extracted text segments and associated commands.
 * 
 * @param text The input text to be parsed for wiki flags.
 * @param flag A `wikiFlag` structure used as a template for new flag entries. It is not modified but serves as a base for memory allocation.
 * 
 * @return A pointer to the head of a linked list of `wikiFlag` structures. Each node in the list contains extracted command information and text pointers.
 * 
 * @details The function iterates through the text looking for comment markers. It extracts text between "<!--" and "-->" markers, creates a `wikiFlag` 
 *          structure for each pair of markers, and links these structures in a list. Odd-numbered flags are associated with pointers starting 
 *          from the end of the comment, while even-numbered flags are associated with pointers starting from the beginning of the comment. 
 *          The function handles multiple pairs of markers and continues parsing until the end of the text is reached.
 */
wikiFlag *parseFlags(char* text, wikiFlag flag);

/**
 * @brief Parses a command sentence into a `command` structure.
 * 
 * This function takes a sentence and breaks it down into individual words, then stores these words into the fields of 
 * a `command` structure. The sentence is expected to contain a function name followed by up to nine arguments.
 * 
 * @param sentence The input string containing the command sentence to be parsed.
 * @param cmd A pointer to a `command` structure where the parsed command and arguments will be stored.
 * 
 * @details The function creates a copy of the input sentence to avoid modifying the original string. It then tokenizes the 
 *          sentence using spaces as delimiters and stores each token in the appropriate field of the `command` structure. 
 *          It handles up to ten words (one function and nine arguments). If the sentence contains more than ten words, 
 *          an error message is printed. All allocated strings in the `command` structure are dynamically allocated and 
 *          should be freed by the caller when no longer needed.
 * 
 * @note The `MAX_ARGUMENTS` constant should define the maximum number of arguments allowed (including the function name). 
 *       The `command` structure should have fields for storing the function and up to nine arguments.
 */
void breakdownCommand(char* sentence, command* cmd);

/**
 * @brief Creates a PlantUML mind map representation of a page's "local" graph, which is a graph which shows all of
 *        the incoming and outgoing links.
 * 
 * This function generates a PlantUML mind map diagram for a given page. It includes the page itself, along with its 
 * incoming and outgoing links. The diagram is formatted using PlantUML syntax and includes links with associated titles 
 * and paths.
 * 
 * @param tempPage A pointer to a `pageList` structure representing the central page for which the mind map is created.
 * @param incomingPaths A pointer to a linked list of `pageList` structures representing the pages that link to the central page.
 * @param outgoingPaths A pointer to a linked list of `pageList` structures representing the pages linked from the central page.
 * 
 * @return A dynamically allocated string containing the PlantUML mind map diagram.
 * 
 * @details The function constructs the PlantUML mind map starting with the central page, then iterates through the incoming 
 *          and outgoing link lists to add corresponding entries. The mind map uses "+" for the central node, "--" for incoming 
 *          links, and "++" for outgoing links. Each link is represented with its path and title. The function also handles memory 
 *          cleanup for the incoming and outgoing link lists after generating the diagram.
 * 
 * @todo rewrite with new cJSON structured link tracker
 */
char* createLocalGraphMindMap(pageList** tempPage, pageList** incomingPaths, pageList** outgoingPaths);

/**
 * @brief Frees the memory allocated for a linked list of `pageList` nodes.
 * 
 * This function iterates through the linked list starting from the head pointer, freeing all dynamically allocated
 * memory associated with each node, including the memory for the node itself. It ensures that each field of the node is
 * checked for non-NULL before attempting to free the memory.
 * 
 * @param head A pointer to a pointer to the first node of the linked list. The list will be traversed and freed until
 *             all nodes are deallocated and the head pointer is set to NULL.
 * 
 * @details The function iterates through the linked list, freeing each node's allocated memory in the following order:
 *          - `id`
 *          - `title`
 *          - `path`
 *          - `description`
 *          - `content`
 *          - `updatedAt`
 *          - `createdAt`
 * 
 *          After freeing the memory for each field, the node itself is deallocated. Debugging print statements are included
 *          to show which memory is being freed, which can be helpful for troubleshooting memory management issues.
 * 
 * @note The `pageList` structure should have fields for dynamically allocated strings. It is important to ensure that
 *       this function is called when the list is no longer needed to prevent memory leaks.
 */
void freePageList(pageList** head);

/**
 * @brief Finds accronyms in a string and appends the accronyms to the path give in argument (void but modifies local accronymList)
 * 
 * @param[in] pathToAccronymList 
 * @param[in] str 
 * 
 * @details
 * 
 * @todo acronym list should be added to wiki instead of append file
 */
void printAcronymsToFile(char* pathToAccronymList, char *str);

/**
 * @brief Sorts an array of words using the bubble sort algorithm.
 * 
 * This function sorts an array of strings (words) in ascending lexicographical order using the bubble sort technique.
 * Each word is compared with the next, and if they are out of order, they are swapped. The process is repeated until
 * the entire array is sorted.
 * 
 * @param words An array of strings where each string represents a word to be sorted. The array should be of size `MAX_WORDS`,
 *              and each string should be of length `MAX_WORD_LENGTH`.
 * @param numWords The number of words in the `words` array. This value should be less than or equal to `MAX_WORDS`.
 * 
 * @details The function uses a nested loop structure to implement bubble sort:
 *          - The outer loop iterates through each word, while the inner loop compares adjacent words and swaps them if needed.
 *          - After each pass through the array, the largest unsorted word "bubbles" up to its correct position.
 * 
 */
void sortWords(char words[MAX_WORDS][MAX_WORD_LENGTH], int numWords);

/**
 * @brief Removes duplicate words from a file and sorts the remaining words alphabetically.
 * 
 * This function reads words from a specified file, removes duplicates, sorts the unique words in alphabetical order,
 * and then writes the sorted unique words back to the file. Each word is expected to be on a new line in the file.
 * 
 * @param filename The path to the file containing the words to be processed. The file should be in a format where each
 *                 word is on a new line.
 * 
 * @details The function performs the following steps:
 *          - Opens the file for reading and reads words line by line, storing them in an array.
 *          - Removes any newline characters from the words.
 *          - Closes the file after reading.
 *          - Sorts the words using the `sortWords` function.
 *          - Opens the file for writing and writes the sorted unique words back to the file.
 *          - If a word is the same as the previous word, it is not written to avoid duplicates.
 */
void removeDuplicatesAndSort(char *filename);

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
 * @brief Prints the title and content of each page in the linked list to Slack.
 * 
 * This function iterates through the linked list of `pageList` nodes and sends the title and content of each page to Slack
 * using `sendMessageToSlack`. It also includes formatting to clearly separate each page's information.
 * 
 * @param head A pointer to a pointer to the head of the linked list of `pageList` structures. The function processes each
 *             node in the list and sends its title and content to Slack.
 * 
 * @details The function performs the following steps:
 *          - Iterates through each node in the linked list.
 *          - Retrieves the current page using `getPage` and processes the page's title and content.
 *          - Sends the page title and content to Slack using `sendMessageToSlack`, including appropriate labels and spacing.
 */
void printPages(pageList** head);

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

/**
 * @brief Finds and adds outgoing links to a linked list. The links must be in a specific subject page's content.
 * 
 * This function searches through the provided content to find sections where the path matches the `subjectPagePath`. It
 * extracts links from these sections and adds them to the `outgoingLinks` list. The function processes a copy of the
 * content to avoid modifying the original content.
 * 
 * @param head A pointer to a pointer to the head of the linked list where the outgoing links will be added.
 * @param linkTrackerContent A string containing the content to be searched for outgoing links.
 * @param subjectPagePath The path of the subject page to be matched in the content.
 * 
 * @return A pointer to the updated list of outgoing links, which includes any newly found links that reference the
 *         `subjectPagePath`.
 * 
 * @details The function performs the following steps:
 *          - Creates a copy of the `linkTrackerContent` to process.
 *          - Searches for sections in the content that start with "## [".
 *          - Extracts titles, paths, and links from these sections.
 *          - If the path matches `subjectPagePath`, it extracts and adds the links found in that section to the
 *            `outgoingLinks` list.
 *          - Updates the pointers to continue searching through the content.
 *          - Frees the allocated memory for the content copy before returning.
 * 
 * @todo replace with a cJSON version
 */
pageList* findOutgoingLinks(pageList **head, char *linkTrackerContent, char *subjectPagePath);

/**
 * @brief Parses a list of requirements from a content string and adds them to a cJSON object (convert from wiki-DRL to cJSON requirements).
 * 
 * This function extracts requirement details from the given `content` string, where each requirement is formatted in a specific
 * manner. It parses each requirement's ID, path, name, and description, and adds these details to the provided `requirements` 
 * cJSON array.
 * 
 * The content string is expected to contain requirements formatted with the following flags:
 * - ID: Begins with "\\n- ["
 * - Path: Follows "](/"
 * - Name: Follows ") **"
 * - Description: Starts with "**\\n" and ends with "\\n"
 * 
 * @param requirements A pointer to the `cJSON` array to which parsed requirements will be added.
 * @param content The string containing the requirements in a specified format.
 * 
 * @details The function performs the following steps:
 * - Searches for the start of each requirement section using specific start flags.
 * - Extracts the ID, path, name, and description for each requirement by calculating their lengths.
 * - Allocates memory for the extracted strings and copies them from the content.
 * - Adds the extracted requirement to the `requirements` cJSON array using the `addRequirementToCjsonObject` function.
 * - Frees allocated memory for the strings after they are added to the JSON object.
 */
void parseRequirementsList(cJSON *requirementList, char *content);

/**
 * @brief Adds a new requirement to a JSON array object.
 * 
 * This function creates a new JSON object representing a requirement and adds it to the provided JSON array. The requirement
 * object includes fields for id, path, name, and description, each of which is initialized with the corresponding string values.
 * 
 * @param requirements A pointer to the `cJSON` array where the new requirement object will be added.
 * @param idStr The string representing the ID of the requirement.
 * @param pathStr The string representing the path of the requirement.
 * @param nameStr The string representing the name of the requirement.
 * @param descriptionStr The string representing the description of the requirement.
 * 
 * @details The function performs the following steps:
 *          - Creates a new JSON object for the requirement.
 *          - Creates JSON strings for the `id`, `path`, `name`, and `description` fields.
 *          - Adds these fields to the requirement object.
 *          - Adds the requirement object to the provided `requirements` JSON array.
 */
void addRequirementToCjsonObject(cJSON *requirements, char *idStr, char *pathStr, char *nameStr, char *descriptionStr);

/**
 * @brief Converts a cJSON array of requirements into a formatted string array.
 * 
 * This function takes a `cJSON` array of requirements and generates a JSON-formatted string that represents the array. Each 
 * requirement in the `cJSON` array is formatted as a list of strings, including the ID, path, name, and description. The 
 * resulting string is formatted as a JSON array of arrays.
 * 
 * The function performs the following steps:
 * - Calculates the required buffer size for the output string based on the number of requirements and their individual lengths.
 * - Allocates memory for the output string with the calculated buffer size.
 * - Iterates through each requirement in the `cJSON` array, extracts the ID, path, name, and description, and appends them to the 
 *   output string in a JSON array format.
 * - Returns the resulting formatted string.
 * 
 * @param requirements A pointer to the `cJSON` array containing the requirements to be converted.
 * 
 * @return A pointer to a dynamically allocated string containing the JSON-formatted requirements. Returns NULL if memory 
 *         allocation fails.
 */
char* parseJSONRequirementListInToArray(cJSON* requirements);

/**
 * @brief Parses a JSON string representing an array of requirements into a structured cJSON object.
 * 
 * This function converts a JSON string containing an array of requirements into a `cJSON` object. Each item in the input array is expected to be an array of 35 strings, which are mapped to specific fields in the output JSON object.
 * 
 * @param input_str The input JSON string representing the array of requirements.
 * 
 * @return A `cJSON` object containing the requirements. If there is an error in parsing or creating the JSON structure, the function returns `NULL`.
 * 
 * @details
 * - The function parses the input string to extract a JSON array named "values".
 * - Each element of this "values" array should be an array of exactly 35 strings.
 * - The function creates a new JSON object with a single field "requirements" which is a JSON array.
 * - For each requirement in the input array, a JSON object is created with fields corresponding to specific columns (e.g., ID, Title, Description, etc.).
 * - These fields are added to the "requirements" array in the output JSON object.
 * - If the input format is incorrect or memory allocation fails, the function prints an error message and returns `NULL`.
 */
cJSON *parseArrayIntoJSONRequirementList(char *input_str);

/**
 * @brief Builds a DRL (Design Requirements List) string from a JSON object containing requirements.
 * 
 * This function constructs a DRL string by iterating over a JSON array of requirement objects. Each requirement
 * object is expected to contain specific fields such as "ID", "Path", "Title", and "Description". The resulting
 * DRL string is built by appending formatted information from each requirement to a template DRL string.
 * 
 * @param requirementList A `cJSON` object containing an array of requirement objects under the "requirements" key.
 * 
 * @return A dynamically allocated string containing the formatted DRL. The caller is responsible for freeing this memory. If the input JSON is not properly formatted or if memory allocation fails, the function may return an incorrect or partially filled string.
 * 
 * @details
 * - The function first retrieves the "requirements" array from the `requirementList` object.
 * - It initializes the DRL string using a predefined template.
 * - For each requirement object in the array, it extracts the fields "ID", "Path", "Title", and "Description".
 * - These fields are appended to the DRL string in a specific format, including separators and markers.
 * - After processing all requirements, the function appends "{.links-list}" to the end of the DRL string.
 * - If any errors are encountered (e.g., missing "requirements" array or incorrect object format), appropriate error messages are printed.
 * - The function returns the final DRL string.
 */
char *buildDrlFromJSONRequirementList(cJSON *requirementList);

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
 * 
 * @todo Function is too long it needs to be broken up into smaller functions.
 */
pageList* buildRequirementPageFromJSONRequirementList(cJSON *requirementList, char *requirementId);

/**
 * @brief Appends a section listing pages that mention the current page.
 * 
 * This function updates the content of the given `pageList` to include a section that lists all pages that mention the current page. The function finds incoming links from a link tracker page, formats them as a markdown list, and appends them to the current page's content. Special characters in the content are escaped before the page is updated and rendered.
 * 
 * @param head A pointer to a pointer to a `pageList` structure representing the current page. The content of this page will be updated to include a list of pages that mention it.
 * 
 * @return This function does not return a value.
 * 
 * @details
 * - The function first initializes a `linkTrackerPage` and retrieves its content.
 * - It then uses this content to find incoming links related to the current page by calling `findIncomingLinks`.
 * - The section titled "Mentioned in" is appended to the current page's content.
 * - For each incoming link, a markdown list item is added to the content, linking to the page that mentions the current page.
 * - After processing all incoming links, the content is updated with a links-list class identifier.
 * - Special characters in the content (newline and quotation marks) are escaped using `replaceWord`.
 * - Finally, the updated page content is saved using `updatePageContentMutation`, and the page is rendered using `renderMutation`.
 */
void appendMentionedIn(pageList** head);

/**
 * @brief Creates a Vega chart configuration for a pie chart depicting verification statuses.
 * 
 * This function generates a Vega chart specification for a pie chart that visualizes the distribution of three categories: unverified, partially verified, and verified populations. The chart is formatted using Vega's schema and is designed to be displayed with the `kroki` tool.
 * 
 * @param unverifiedPopulation A string representing the population of unverified items. This value is inserted into the Vega chart configuration.
 * @param partiallyVerifiedPopulation A string representing the population of partially verified items. This value is inserted into the Vega chart configuration.
 * @param verifiedPopulation A string representing the population of verified items. This value is inserted into the Vega chart configuration.
 * 
 * @return A dynamically allocated string containing the Vega chart specification. The returned string includes the provided population values substituted into the template.
 * 
 * @details
 * - The function starts with a predefined Vega chart template in string format.
 * - It replaces placeholder values (`DefaultUnverifiedPopulation`, `DefaultPartiallyVerifiedPopulation`, `DefaultVerifiedPopulation`) in the template with the provided arguments.
 * - The final Vega chart specification is returned, ready to be used for rendering a pie chart.
 */
char *createVcdPieChart(char *unverifiedPopulation, char *partiallyVerifiedPopulation, char *verifiedPopulation);

/**
 * @brief Updates a JSON string representing a stacked area chart with new weekly data.
 * 
 * This function updates a JSON string that contains data for a stacked area chart by adding new data points for the specified week. The new data points include verified, partially verified, and unverified values.
 * 
 * @param json_str A string containing the JSON data of the existing chart. This JSON is expected to have a "data" object with a "values" array where new entries will be added.
 * @param week A string representing the week for which the data is being added. This value will be included in each new data entry.
 * @param verifiedValue An integer representing the value for the "Verified" status for the given week.
 * @param partiallyVerifiedValue An integer representing the value for the "Partially Verified" status for the given week.
 * @param unverifiedValue An integer representing the value for the "Unverified" status for the given week.
 * 
 * @return A dynamically allocated string containing the updated JSON data, with new data points added to the "values" array. The caller is responsible for freeing this string.
 * 
 * @details
 * - The function parses the input JSON string and retrieves the "values" array from the "data" object.
 * - Three new JSON objects are created, each representing one of the statuses ("Verified", "Partially Verified", "Unverified") with their corresponding values and the specified week.
 * - These new JSON objects are added to the "values" array.
 * - The updated JSON structure is converted back to a string and returned.
 * - If any error occurs during parsing or updating, the function returns `NULL`.
 */
char *updateVcdStackedAreaChart(char *json_str, char *week, int verifiedValue, int partiallyVerifiedValue, int unverifiedValue);
#endif