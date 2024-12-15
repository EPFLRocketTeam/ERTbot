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
    log_function_entry(__func__);

    if (!inputString || !wordToReplace || !newWord) return NULL;

    char* result;
    int cnt = 0;
    size_t newWordLength = strlen(newWord);
    size_t wordToReplaceLength = strlen(wordToReplace);

    const char *ptr = inputString;
    while ((ptr = strstr(ptr, wordToReplace)) != NULL) {
        cnt++;
        ptr += wordToReplaceLength;
    }

    size_t len = strlen(inputString) + cnt * (newWordLength - wordToReplaceLength) + 1;

    result = (char*)malloc(len);
    if (!result) return NULL;

    size_t i = 0;  

    while (*inputString) {
        const char *match = strstr(inputString, wordToReplace);
        if (match == inputString) {
            size_t remaining_len = len - i;
            strlcpy(&result[i], newWord, remaining_len);  
            i += newWordLength;
            inputString += wordToReplaceLength;
        } else {
            result[i++] = *inputString++;
        }
    }

    result[i] = '\0'; 

    log_function_exit(__func__);
    return result;
}


char* replaceWord_Realloc(char* inputString, const char* wordToReplace, const char* newWord) {
    log_function_entry(__func__);

    if (!inputString || !wordToReplace || !newWord){ 
        return NULL;
    }
    
    int cnt = 0;
    size_t newWordLength = strlen(newWord);
    size_t wordToReplaceLength = strlen(wordToReplace);

    const char *ptr = inputString;
    while ((ptr = strstr(ptr, wordToReplace)) != NULL) {
        cnt++;
        ptr += wordToReplaceLength;
    }

    size_t newLength = 1 + strlen(inputString) + cnt * (newWordLength - wordToReplaceLength);

    char* resizedString = realloc(inputString, sizeof(char*) * newLength);
    if (resizedString == NULL){
        return NULL;  
    }
    inputString = resizedString;

    const char* result = inputString;
    char* pos = strstr(result, wordToReplace);
    while (pos) {
        size_t remainingLength = strlen(pos + wordToReplaceLength);
        memmove(pos + newWordLength, pos + wordToReplaceLength, sizeof(char) *remainingLength + 1);
        memcpy(pos, newWord, newWordLength);
        result = pos + newWordLength;
        pos = strstr(result, wordToReplace);
    }

    log_function_exit(__func__);
    return inputString;
}

char* replaceParagraph(char* original, char* newSubstring, char* startPtr, char* endPtr) {
    log_function_entry(__func__);

    if (original == NULL || newSubstring == NULL || startPtr == NULL || endPtr == NULL) {
        log_message(LOG_ERROR, __func__, "Null pointer input original:%ld, newSubstring: %ld, startPtr: %ld, endPtr: %ld", (long)original, (long)newSubstring, (long)startPtr, (long)endPtr);
        return NULL;
    }

    if (startPtr < original || endPtr >= original + strlen(original)) {
        log_message(LOG_ERROR, __func__, "startPtr or endPtr out of bounds. startPtr: %ld, endPtr: %ld, original end: %ld", (long)startPtr, (long)endPtr, (long)(original + strlen(original)));
        log_message(LOG_DEBUG, __func__, " startPtr is at: \"%ld\"", (long)startPtr);
        log_message(LOG_DEBUG, __func__, " endPtr is at: \"%ld\"", (long)endPtr);
        return NULL;
    }

    size_t originalLen = strlen(original);
    size_t newSubLen = strlen(newSubstring);
    size_t replaceLen = endPtr - startPtr + 1;
    size_t finalLen = originalLen - replaceLen + newSubLen;

    log_message(LOG_DEBUG, __func__, "lengths set");

    char *temp = malloc(finalLen + 1 * sizeof(char));
    if (temp == NULL) {
        log_message(LOG_ERROR, __func__, "Memory allocation failed");
        return NULL;
    }
    memset(temp, 0, finalLen + 1);
    log_message(LOG_DEBUG, __func__, "buffer set");

    strncpy(temp, original, startPtr - original);
    log_message(LOG_DEBUG, __func__, "copied first part");

    strcat(temp, newSubstring);
    log_message(LOG_DEBUG, __func__, "copied new part");

    strcat(temp, endPtr + 1);
    log_message(LOG_DEBUG, __func__, "copied end");

    log_function_exit(__func__);
    return temp;
}

char* createCombinedString(const char *str1, const char *str2) {
    log_function_entry(__func__);

    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t len3 = len1 + len2 + 1;

    char *combined = (char *)malloc(len3);

    if (combined == NULL) {
        log_message(LOG_ERROR, __func__, "Memory allocation failed");
        exit(1);
    }

    strlcpy(combined, str1, len3);
    strcat(combined, str2);


    log_function_exit(__func__);
    return combined;
}

char* extractText(const char *inputString, const char *startDelimiter, const char *endDelimiter, bool includeStart, bool includeEnd) {
    log_function_entry(__func__);

    if (inputString == NULL || startDelimiter == NULL || endDelimiter == NULL) {
        return NULL;
    }

    const char *startOfParagraph = strstr(inputString, startDelimiter);
    if (startOfParagraph == NULL) {
        return NULL;
    }
    if (!includeStart) {
        startOfParagraph += strlen(startDelimiter);
    }

    const char *endOfParagraph = strstr(startOfParagraph, endDelimiter);
    if (endOfParagraph == NULL) {
        return NULL;
    }
    if (includeEnd) {
        endOfParagraph += strlen(endDelimiter);
    }

    size_t length = endOfParagraph - startOfParagraph;
    char *substring = (char*)malloc(length + 1);

    if (!substring) {
        return NULL;
    }

    strncpy(substring, startOfParagraph, length);
    substring[length] = '\0';


    log_function_exit(__func__);
    return substring;
}

char* appendToString(char *original, const char *strToAppend) {
    log_function_entry(__func__);

    if (strToAppend == NULL) {
        return original; // Nothing to append
    }

    size_t lenOriginal = original ? strlen(original) : 0;
    size_t lenAppend = strlen(strToAppend);
    size_t len = lenOriginal + lenAppend + 1;

    char *combined = realloc(original, len);

    if (combined == NULL) {
        log_message(LOG_ERROR, __func__, "Memory reallocation failed");
        exit(1);
    }

    strlcpy(combined + lenOriginal, strToAppend, len);

    log_function_exit(__func__);
    return combined;
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
            log_message(LOG_ERROR, __func__, "Memory allocation failed for %s", field_name);
        }
    }
}

