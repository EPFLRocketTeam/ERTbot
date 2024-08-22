#ifndef REQUIREMENTS_H
#define REQUIREMENTS_H
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