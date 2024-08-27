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
pageList* addPageToList(pageList** head,  char *id, char *title, char *path, char *description, char *content, char *updatedAt, char *createdAt, char *authorId);

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
wikiFlag *parseFlags(char* text);

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

void freeWikiFlagList(wikiFlag** head);

char* convert_timestamp_to_cest(char *timestamp);

char* buildPageDistributionPieChart(pageList* head);

#endif