#ifndef ERTBOT_REQUIREMENTS_HELPERS_H
#define ERTBOT_REQUIREMENTS_HELPERS_H

#include <cjson/cJSON.h>

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

cJSON* getSubsystemInfo(char* acronym);

cJSON* getRequirements(cJSON* subsystem);
#endif
