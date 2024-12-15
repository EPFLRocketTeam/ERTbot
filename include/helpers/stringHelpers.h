#ifndef ERTBOT_STRING_HELPER_H
#define ERTBOT_STRING_HELPER_H

#include <stdbool.h>

/**
 * @brief Replaces all occurrences of a specified word in a string with a new word.
 *
 * @param[in] inputString Pointer to the string in which occurrences of `wordToReplace` will be replaced.
 * @param[in] wordToReplace Pointer to the word that needs to be replaced.
 * @param[in] newWord Pointer to the word that will replace occurrences of `wordToReplace`.
 *
 * @return char* Pointer to a newly allocated string with the replacements made. The caller is responsible for
 *         freeing the allocated memory.
 *
 * @details This function counts the number of occurrences of `wordToReplace` in `inputString`, calculates the
 *          required length for the new string with the replacements, and then creates a new string with all
 *          occurrences of `wordToReplace` replaced by `newWord`. It uses `strstr` to locate occurrences and `strcpy`
 *          to copy the new word into the result string. The function handles the memory allocation for the result
 *          string and appends a null terminator at the end.
 */
char* replaceWord_Malloc(const char* inputString, const char* wordToReplace, const char* newWord);

char* replaceWord_Realloc(char* inputString, const char* wordToReplace, const char* newWord);

/**
 * @brief Replaces a section of a string with a new substring.
 *
 * @param[in] original Pointer to the original string.
 * @param[in] newSubstring Pointer to the substring that will replace the section.
 * @param[in] startPtr Pointer to the start of the section to be replaced.
 * @param[in] endPtr Pointer to the end of the section to be replaced.
 *
 * @return char* Pointer to a newly allocated string with the section replaced. The caller is responsible for
 *         freeing the allocated memory.
 *
 * @details This function replaces a specified section of the original string, defined by the pointers `startPtr` and
 *          `endPtr`, with a new substring. It calculates the required length for the new string, allocates a temporary
 *          buffer, and constructs the modified string by concatenating the parts before the section, the new substring,
 *          and the parts after the section. The function returns a pointer to the newly allocated string containing the
 *          modified content.
 */
char* replaceParagraph(char *original, char *newSubstring, char *startPtr, char *endPtr);

/**
 * @brief Concatenates two strings and returns a new string containing the result.
 *
 * @param[in] str1 Pointer to the first input string.
 * @param[in] str2 Pointer to the second input string.
 *
 * @return char* Pointer to a newly allocated string containing the concatenated result of `str1` and `str2`.
 *         The caller is responsible for freeing the allocated memory. The function will terminate the program if
 *         memory allocation fails.
 *
 * @details This function calculates the lengths of the input strings `str1` and `str2`, allocates sufficient memory
 *          to hold the combined string including the null terminator, and concatenates the two strings. If memory
 *          allocation fails, the function prints an error message and terminates the program.
 *
 */
char* createCombinedString(const char *str1, const char *str2);

/**
 * @brief Extracts a substring from the input string based on specified start and end delimiters.
 *
 * @param[in] inputString Pointer to the input string from which the substring will be extracted.
 * @param[in] startDelimiter Pointer to the start delimiter string.
 * @param[in] endDelimiter Pointer to the end delimiter string.
 * @param[in] includeStart Boolean indicating whether to include the start delimiter in the extracted substring.
 * @param[in] includeEnd Boolean indicating whether to include the end delimiter in the extracted substring.
 *
 * @return char* Pointer to a newly allocated string containing the extracted substring. Returns `NULL` if any of the
 *         delimiters are not found, or if memory allocation fails. The caller is responsible for freeing the allocated
 *         memory.
 *
 * @details This function searches for the `startDelimiter` and `endDelimiter` within the `inputString`. It extracts the
 *          text between these delimiters based on the `includeStart` and `includeEnd` flags. If `includeStart` is `false`,
 *          the text starting right after the `startDelimiter` is considered. If `includeEnd` is `true`, the text up to and
 *          including the `endDelimiter` is extracted; otherwise, the text up to but not including the `endDelimiter` is
 *          extracted.
 */
char* extractText(const char *inputString, const char *startDelimiter, const char *endDelimiter, bool includeStart, bool includeEnd);

/**
 * @brief Appends one string to another, dynamically reallocating memory as needed.
 *
 * This function takes an original string and appends a second string (`strToAppend`) to it.
 * Memory for the combined string is reallocated, ensuring the result fits both strings.
 * If the original string is NULL, it is treated as an empty string. The caller is responsible
 * for freeing the final returned string to avoid memory leaks.
 *
 * @param original Pointer to the original string. Can be NULL, which is treated as an empty string.
 * @param strToAppend Pointer to the string that will be appended to the original string. If NULL, the original string is returned without changes.
 * @return A pointer to the combined string with `strToAppend` appended to `original`.
 *         The returned string is dynamically allocated and must be freed by the caller.
 */
char* appendToString(char *original, const char *strToAppend);

char* duplicate_Malloc(const char *src);

void allocateAndCopy(char **destination, const char *source, const char *field_name);

size_t strlcpy(char *dst, const char *src, size_t dstsize);
#endif
