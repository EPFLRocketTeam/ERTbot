/**
 * @file stringTools.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief This file contains all of the helper functions which do string manipulations
 */

#include <stdbool.h>
#include <string.h>
#include "ERTbot_common.h"
#include "stringHelpers.h"



char* replaceWord_Malloc(const char* inputString, const char* wordToReplace, const char* newWord) {
    log_message(LOG_DEBUG, "Entering function replaceWord_Malloc");

    if (!inputString || !wordToReplace || !newWord) return NULL;

    char* result;
    int i;
    int cnt = 0;
    size_t newWordLength = strlen(newWord);
    size_t wordToReplaceLength = strlen(wordToReplace);

    // Counting the number of times old word
    // occur in the string
    for (i = 0; inputString[i] != '\0'; i++) {
        if (strstr(&inputString[i], wordToReplace) == &inputString[i]) {
            cnt++;

            // Jumping to index after the old word.
            i += wordToReplaceLength - 1;
      }
    }

    size_t len = i + cnt * (newWordLength - wordToReplaceLength) + 1;

    // Making new string of enough length
    result = (char*)malloc(len);
    if (!result) return NULL;  // Check malloc success

    i = 0;
    while (*inputString) {
        // compare the substring with the result
        if (strstr(inputString, wordToReplace) == inputString) {
            strlcpy(&result[i], newWord, len);
            i += newWordLength;
            inputString += wordToReplaceLength;
        }
        else{
            result[i++] = *inputString++;
        }
    }

  result[i] = '\0';

  log_message(LOG_DEBUG, "Exiting function replaceWord_Malloc");
  return result;
}

char* replaceWord_Realloc(char* inputString, const char* wordToReplace, const char* newWord) {
    log_message(LOG_DEBUG, "Entering function replaceWord_Realloc");

    if (!inputString || !wordToReplace || !newWord){ 
        return NULL;
    }
    
    int cnt = 0;
    size_t newWordLength = strlen(newWord);
    size_t wordToReplaceLength = strlen(wordToReplace);


    // Count occurrences of wordToReplace in inputString
    for (const char* temp = inputString; (temp = strstr(temp, wordToReplace)); temp += wordToReplaceLength) {
        cnt++;
    }

    // Calculate new length needed
    size_t newLength = 1 + strlen(inputString) + cnt * (newWordLength - wordToReplaceLength);

    // Reallocate inputString to new size
    char* resizedString = realloc(inputString, sizeof(char*) * newLength);
    if (resizedString == NULL){
        return NULL;  // Check if realloc succeeded
    }
    inputString = resizedString;

    // Replace occurrences of wordToReplace with newWord in place
    const char* result = inputString;
    char* pos = strstr(result, wordToReplace);
    while (pos) {
        size_t remainingLength = strlen(pos + wordToReplaceLength);
        //log_message(LOG_DEBUG, "replaceWord_Realloc: remainingLength: %zu", remainingLength);
        // Shift the remaining part of the string to make room for newWord, if needed
        memmove(pos + newWordLength, pos + wordToReplaceLength, sizeof(char) *remainingLength + 1);  // Include '\0'

        // Copy newWord into the position
        memcpy(pos, newWord, newWordLength);

        // Move result pointer to next potential position
        result = pos + newWordLength;
        pos = strstr(result, wordToReplace);
    }

    log_message(LOG_DEBUG, "Exiting function replaceWord_Realloc");
    return inputString;
}

char* replaceParagraph(char* original, char* newSubstring, char* startPtr, char* endPtr) {
    log_message(LOG_DEBUG, "Entering function replaceParagraph");

    // Check for null pointers
    if (original == NULL || newSubstring == NULL || startPtr == NULL || endPtr == NULL) {
        log_message(LOG_ERROR, "Null pointer input original:%ld, newSubstring: %ld, startPtr: %ld, endPtr: %ld", (long)original, (long)newSubstring, (long)startPtr, (long)endPtr);
        return NULL;
    }

    // Check that startPtr and endPtr are within bounds
    if (startPtr < original || endPtr >= original + strlen(original)) {
        log_message(LOG_ERROR, "startPtr or endPtr out of bounds. startPtr: %ld, endPtr: %ld, original end: %ld", (long)startPtr, (long)endPtr, (long)(original + strlen(original)));
        log_message(LOG_DEBUG, " startPtr is at: \"%ld\"", (long)startPtr);
        log_message(LOG_DEBUG, " endPtr is at: \"%ld\"", (long)endPtr);
        return NULL;
    }

    // Calculate the lengths
    size_t originalLen = strlen(original);
    size_t newSubLen = strlen(newSubstring);
    size_t replaceLen = endPtr - startPtr + 1;
    size_t finalLen = originalLen - replaceLen + newSubLen;

    log_message(LOG_DEBUG, "lengths set");

    // Create a temporary buffer to hold the modified string
    char *temp = malloc(finalLen + 1 * sizeof(char));
    if (temp == NULL) {
        log_message(LOG_ERROR, "Memory allocation failed");
        return NULL;
    }
    memset(temp, 0, finalLen + 1);
    log_message(LOG_DEBUG, "buffer set");

    // Copy the part before the replaced section
    strncpy(temp, original, startPtr - original);
    log_message(LOG_DEBUG, "copied first part");

    // Copy the new substring
    strcat(temp, newSubstring);
    log_message(LOG_DEBUG, "copied new part");

    // Copy the part after the replaced section
    strcat(temp, endPtr + 1);
    log_message(LOG_DEBUG, "copied end");

    // Copy the modified string back to the original

    log_message(LOG_DEBUG, "Exiting function replaceParagraph");
    return temp;
}

char* createCombinedString(const char *str1, const char *str2) {
    log_message(LOG_DEBUG, "Entering function createCombinedString");

    // Calculate the length of str1 and str2
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t len3 = len1 + len2 + 1;

    // Allocate memory for the combined string
    char *combined = (char *)malloc(len3); // +1 for the null terminator

    if (combined == NULL) {
        log_message(LOG_ERROR, "Memory allocation failed");
        exit(1);
    }

    // Copy the contents of str1 and str2 into combined
    strlcpy(combined, str1, len3);
    strcat(combined, str2);


    log_message(LOG_DEBUG, "Exiting function createCombinedString");
    return combined;
}

char* extractText(const char *inputString, const char *startDelimiter, const char *endDelimiter, bool includeStart, bool includeEnd) {
    log_message(LOG_DEBUG, "Entering function extractText");

    if (inputString == NULL || startDelimiter == NULL || endDelimiter == NULL) {
        return NULL; // Invalid parameters
    }

    // Find the start of the paragraph based on the start delimiter
    const char *startOfParagraph = strstr(inputString, startDelimiter);
    if (startOfParagraph == NULL) {
        return NULL; // Start delimiter not found
    }
    if (!includeStart) {
        startOfParagraph += strlen(startDelimiter);
    }

    // Find the end of the paragraph based on the end delimiter
    const char *endOfParagraph = strstr(startOfParagraph, endDelimiter);
    if (endOfParagraph == NULL) {
        return NULL; // End delimiter not found
    }
    if (includeEnd) {
        endOfParagraph += strlen(endDelimiter);
    }

    // Calculate the length of the substring to extract
    size_t length = endOfParagraph - startOfParagraph;
    char *substring = (char*)malloc(length + 1);

    if (!substring) {
        return NULL; // Memory allocation failed
    }

    // Copy the substring and null-terminate it
    strncpy(substring, startOfParagraph, length);
    substring[length] = '\0';  // Null-terminate the string


    log_message(LOG_DEBUG, "Exiting function extractText");
    return substring;
}

char* appendToString(char *original, const char *strToAppend) {
    log_message(LOG_DEBUG, "Entering function appendToString");

    if (strToAppend == NULL) {
        return original; // Nothing to append
    }

    // Calculate the length of original string and the string to append
    size_t lenOriginal = original ? strlen(original) : 0;
    size_t lenAppend = strlen(strToAppend);
    size_t len = lenOriginal + lenAppend + 1;

    // Reallocate memory for the combined string (original + append + null terminator)
    char *combined = realloc(original, len);

    if (combined == NULL) {
        log_message(LOG_ERROR, "Memory reallocation failed");
        exit(1);
    }

    // Copy/concatenate the new string
    strlcpy(combined + lenOriginal, strToAppend, len); // Append strToAppend to the end of original

    log_message(LOG_DEBUG, "Exiting function appendToString");
    return combined; // Return the reallocated and combined string
}

char* duplicate_Malloc(const char *src) {

    size_t len = strlen(src) + 1;    

    char *dst = malloc(len);     

    if (dst == NULL){
        return NULL;
    }

    memcpy (dst, src, len);           
      
    return dst;                         
}

size_t strlcpy(char *dst, const char *src, size_t dstsize) {
    size_t src_len = 0;
    while (src[src_len] != '\0') src_len++;

    if (dstsize > 0) {
        size_t copy_len = (src_len >= dstsize) ? dstsize - 1 : src_len;
        for (size_t i = 0; i < copy_len; i++) {
            dst[i] = src[i];
        }
        dst[copy_len] = '\0';
    }

    return src_len;
}

void allocateAndCopy(char **destination, const char *source, const char *field_name) {
    if (source != NULL) {
        size_t len = strlen(source) + 1;
        *destination = malloc(len);
        if (*destination) {
            strlcpy(*destination, source, len);
        } else {
            log_message(LOG_ERROR, "Memory allocation failed for %s", field_name);
        }
    }
}

