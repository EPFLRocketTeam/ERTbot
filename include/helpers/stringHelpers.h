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
 *
 * @note The function assumes that the input strings (`inputString`, `wordToReplace`, and `newWord`) are valid and
 *       null-terminated. Memory allocated for the result string should be freed by the caller when it is no longer needed.
 */
char* replaceWord_Malloc(const char* inputString, const char* wordToReplace, const char* newWord);

char* replaceWord_Realloc(char* inputString, const char* wordToReplace, const char* newWord);

/**
 * @brief Replaces occurrences of the string literal "\\n" with actual newline characters ('\n') in a string.
 *
 * @param[in] inputString Pointer to the string in which "\\n" sequences will be replaced with newline characters.
 *
 * @return char* Pointer to the modified string with "\\n" replaced by '\n'.
 *
 * @details This function iterates through the input string and replaces each occurrence of the string literal "\\n"
 *          (a backslash followed by 'n') with an actual newline character ('\n'). The function then adjusts the
 *          remaining characters in the string to account for the reduction in length caused by the replacement.
 *          A null terminator is added to ensure the string is properly terminated. The function modifies the input
 *          string in place and returns a pointer to the modified string.
 *
 * @note The function assumes that `inputString` is a valid, null-terminated string. The input string is modified
 *       directly, and no additional memory is allocated or freed by this function.
 */
char* reformatNewLineCharacter(char *inputString);

/**
 * @brief Removes a character from a string at a specified index.
 *
 * @param[in] str Pointer to the string from which the character will be removed.
 * @param[in] index The index of the character to be removed.
 *
 * @return char* Pointer to the modified string with the character removed. The original string is modified in place.
 *
 * @details This function removes a character from the input string at the specified index by shifting all characters
 *          after the index one position to the left. The string is null-terminated after the character is removed.
 *          If the index is out of bounds (less than 0 or greater than or equal to the string length), the function does
 *          nothing and returns the original string.
 *
 * @note The function modifies the input string in place. It assumes that `str` is a valid, null-terminated string.
 */
char* remove_char_at_index(char *str, int index);

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
 *
 * @note The function assumes that `original`, `newSubstring`, `startPtr`, and `endPtr` are valid and properly set up.
 *       Memory allocated for the new string should be freed by the caller when it is no longer needed.
 *
 * @warning Ensure that `startPtr` and `endPtr` are within the bounds of the original string and that `startPtr` <= `endPtr`.
 */
char* replaceParagraph(char *original, char *newSubstring, char *startPtr, char *endPtr);

/**
 * @brief Extracts a substring from an input string using specified start and end pointers.
 *
 * @param[in] inputString Pointer to the original string from which the substring will be extracted.
 * @param[in] startOfParagraph Pointer to the start of the substring to be extracted.
 * @param[in] endOfParagraph Pointer to the end of the substring to be extracted.
 *
 * @return char* Pointer to a newly allocated string containing the extracted substring. Returns NULL if any
 *         parameter is invalid or if memory allocation fails. The caller is responsible for freeing the allocated memory.
 *
 * @details This function extracts a substring from the input string starting from `startOfParagraph` and ending at
 *          `endOfParagraph`. It calculates the length of the substring, allocates memory for it, and copies the
 *          substring into the newly allocated buffer. The function ensures that the extracted substring is null-terminated.
 *          If any of the input pointers are NULL or if `startOfParagraph` is greater than `endOfParagraph`, the function
 *          returns NULL. If memory allocation fails, the function also returns NULL.
 *
 * @note The function assumes that `startOfParagraph` and `endOfParagraph` are valid pointers within `inputString` and
 *       that `startOfParagraph` is less than or equal to `endOfParagraph`.
 */
char* extractParagraphWithPointerDelimiters(const char *inputString, const char *startOfParagraph, const char *endOfParagraph);

/**
 * @brief Extracts the document ID from a URL or path by finding the substring after the last '/'.
 *
 * @param[in] str Pointer to the input string containing the URL or path.
 *
 * @return char* Pointer to the substring following the last '/' in the input string. Returns an empty string
 *         if no '/' is found. The returned pointer points to a substring within the original string and does not
 *         require additional memory management.
 *
 * @details This function locates the last occurrence of the '/' character in the input string and returns the
 *          substring that follows it. If the '/' character is not found, the function returns an empty string.
 *          The returned substring is directly obtained from the original string and does not involve additional
 *          memory allocation.
 *
 * @note The function assumes that `str` is a valid, null-terminated string. The returned substring is a view into
 *       the original string, and the original string should remain valid while the returned substring is in use.
 */
char* getDocId(const char* str);

/**
 * @brief Extracts the parent folder name from a path or URL.
 *
 * @param[in] str Pointer to the input string containing the path or URL.
 *
 * @return char* Pointer to a newly allocated string containing the parent folder name. Returns an empty string
 *         if no '/' is found or `NULL` if memory allocation fails. The caller is responsible for freeing the allocated memory.
 *
 * @details This function identifies the parent folder name by finding the substring between the second last and
 *          last occurrence of the '/' character in the input string. It temporarily modifies the input string to
 *          facilitate the extraction of the parent folder. If the second last '/' is not found, the function assumes
 *          that the parent folder is the part of the string before the last '/'. If memory allocation for the result
 *          fails, the function returns `NULL`.
 *
 * @note The function assumes that `str` is a valid, null-terminated string. It temporarily modifies the input string
 *       and restores it before returning the result. The caller must free the allocated memory for the result string
 *       when it is no longer needed.
 *
 * @warning Ensure that `str` is not `NULL` and contains at least one '/' character for meaningful results.
 */
char* getParentFolder(const char* str);

/**
 * @brief Extracts the directory path from a given string by removing the filename or last segment after the last '/'.
 *
 * @param[in] str Pointer to the input string containing the path or URL.
 *
 * @return char* Pointer to a newly allocated string containing the directory path. Returns an empty string if no '/'
 *         is found, or `NULL` if memory allocation fails. The caller is responsible for freeing the allocated memory.
 *
 * @details This function identifies the directory path by finding the substring before the last occurrence of the '/'
 *          character in the input string. It allocates memory for the result, copies the relevant portion of the input
 *          string into the newly allocated buffer, and null-terminates the string. If the input string does not contain
 *          a '/', the function returns an empty string. If memory allocation fails, the function returns `NULL`.
 *
 * @note The function assumes that `str` is a valid, null-terminated string. The caller must free the allocated memory
 *       for the result string when it is no longer needed.
 *
 * @warning Ensure that `str` is not `NULL` and contains at least one '/' character for meaningful results.
 */
char* getDirPath(const char* str);

/**
 * @brief Extracts the last three characters from a given string.
 *
 * @param[in] str Pointer to the input string.
 *
 * @return char* Pointer to a substring containing the last three characters of the input string. If the input string
 *         has fewer than three characters, returns a pointer to the message "String is too short". The returned pointer
 *         points to a substring within the original string and does not require additional memory management.
 *
 * @details This function calculates the length of the input string and returns a pointer to the substring consisting of
 *          the last three characters. If the input string is shorter than three characters, the function returns a pointer
 *          to a static message indicating that the string is too short. The returned substring is directly obtained from
 *          the original string and does not involve additional memory allocation.
 *
 * @note The function assumes that `str` is a valid, null-terminated string. The returned pointer is a view into the
 *       original string and the original string should remain valid while the returned substring is in use.
 *
 * @warning Ensure that `str` is not `NULL` and has at least three characters for meaningful results. The static message
 *          returned for short strings should not be modified or freed.
 */
char* getDocType(char* str);

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
 * @note The function assumes that both `str1` and `str2` are valid, null-terminated strings. The allocated memory for
 *       the combined string should be freed by the caller when it is no longer needed.
 *
 * @warning Ensure that `str1` and `str2` are not `NULL`. Memory allocation failure will result in the program exiting
 *          with an error message.
 */
char* createCombinedString(const char *str1, const char *str2);

/**
 * @brief Truncates a string at the first occurrence of a space character.
 *
 * @param[in] str Pointer to the input string.
 *
 * @return char* Pointer to the modified input string, with content after the first space character removed.
 *         If no space character is found, the original string is returned unchanged.
 *
 * @details This function searches for the first occurrence of a space character in the input string and replaces it
 *          with a null terminator, effectively truncating the string at that point. If a space is not found, the original
 *          string remains unchanged.
 *
 * @note The function assumes that `str` is a valid, null-terminated string. The modification is done in place and the
 *       original string is directly altered.
 *
 * @warning Ensure that `str` is not `NULL`. The function performs in-place modification of the input string.
 */
char* removeAfterSpace(char *str);

/**
 * @brief Removes the last folder from a given path.
 *
 * @param[in] path Pointer to the input path string.
 *
 * @return char* Pointer to the modified path string with the last folder removed. If the input path does not contain
 *         any folder separators (`/`), the function returns an empty string.
 *
 * @details This function searches for the last occurrence of the '/' character in the input path and truncates the
 *          string at that point, effectively removing the last folder from the path. If no '/' is found, indicating that
 *          the path is just a file name or empty, the function sets the path to an empty string.
 *
 * @note The function performs in-place modification of the input string. The original string is directly altered.
 *
 * @warning Ensure that `path` is not `NULL`. The function assumes that `path` is a valid, null-terminated string and
 *          performs in-place modification.
 */
char* removeLastFolder(char *path);

/**
 * @brief Extracts a substring from the beginning of the input string up to (but not including) the first occurrence
 *        of a specified delimiter.
 *
 * @param[in] str Pointer to the input string.
 * @param[in] delimiter Pointer to the delimiter string.
 *
 * @return char* Pointer to a newly allocated string containing the substring from the beginning of `str` up to the
 *         delimiter. If the delimiter is not found, a copy of the entire input string is returned. The caller is
 *         responsible for freeing the allocated memory.
 *
 * @details This function searches for the first occurrence of the `delimiter` in the input string `str`. If the delimiter
 *          is found, it extracts and returns the substring from the start of `str` up to (but not including) the delimiter.
 *          If the delimiter is not present, it returns a copy of the entire input string. The allocated memory for the
 *          resulting substring should be freed by the caller when it is no longer needed.
 *
 * @note The function allocates memory for the resulting substring, so ensure to free it to avoid memory leaks.
 *
 * @warning Ensure that `str` and `delimiter` are not `NULL`. Memory allocation failure is not explicitly handled, so
 *          ensure sufficient memory is available.
 */
char* returnTextUntil(const char* str, const char* delimiter);

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
 *
 * @note The function allocates memory for the resulting substring. Ensure to free the allocated memory to avoid memory leaks.
 *
 * @warning Ensure that `inputString`, `startDelimiter`, and `endDelimiter` are not `NULL`. Handle potential memory
 *          allocation failure and check for null pointers in practice.
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
 *
 * @note The function internally uses `realloc`, so it may change the memory address of the `original` string.
 * @note If memory reallocation fails, the program will log an error and terminate with `exit(1)`.
 *
 * @warning The caller is responsible for freeing the memory of the returned string to avoid memory leaks.
 */
char* appendToString(char *original, const char *strToAppend);
#endif
