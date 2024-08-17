/**
 * @file stringTools.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief This file contains all of the helper functions which do string manipulations
 */

#include "../include/struct.h"
#include "../include/api.h"
#include "../include/config.h"
#include "../include/features.h"
#include "../include/githubAPI.h"
#include "../include/helperFunctions.h"
#include "../include/markdownToPDF.h"
#include "../include/slackAPI.h"
#include "../include/stringTools.h"
#include "../include/wikiAPI.h"
#include "../include/sheetAPI.h"
#include "../include/command.h"
#include "../include/log.h"

char* replaceWord( char* inputString,  char* wordToReplace,  char* newWord) {
    log_message(LOG_DEBUG, "Entering function replaceWord");
    
  char* result; 
  int i, cnt = 0; 
  int newWordLength = strlen(newWord); 
  int wordToReplaceLength = strlen(wordToReplace); 

  // Counting the number of times old word 
  // occur in the string 
  for (i = 0; inputString[i] != '\0'; i++) { 
      if (strstr(&inputString[i], wordToReplace) == &inputString[i]) { 
          cnt++; 

          // Jumping to index after the old word. 
          i += wordToReplaceLength - 1; 
      } 
  } 

  // Making new string of enough length 
  result = (char*)malloc(i + cnt * (newWordLength - wordToReplaceLength) + 1); 

  i = 0; 
  while (*inputString) { 
      // compare the substring with the result 
      if (strstr(inputString, wordToReplace) == inputString) { 
        strcpy(&result[i], newWord); 
        i += newWordLength; 
        inputString += wordToReplaceLength; 
      } 
      else
        result[i++] = *inputString++; 
  } 

  result[i] = '\0';
  
  log_message(LOG_DEBUG, "Exiting function replaceWord"); 
  return result; 
}

char* reformatNewLineCharacter(char *inputString) {
    log_message(LOG_DEBUG, "Entering function reformatNewLineCharacter");
    
    int i, j;
    char *tempstr = inputString;
    int len = strlen(tempstr);
    
    // Loop through the string
    for (i = 0; i < len - 1; i++) {
        // Check for "\n"
        if (tempstr[i] == '\\' && tempstr[i + 1] == 'n') {
            // Replace "\\n" with "\n"
            tempstr[i] = '\n';
            // Shift the characters to the left
            for (j = i + 1; j < len - 1; j++) {
                tempstr[j] = tempstr[j + 1];
            }
            tempstr[j] = '\0'; // Add null terminator at the end
            len -= 1; // Decrease the length
        }
    }

    
    log_message(LOG_DEBUG, "Exiting function reformatNewLineCharacter");
    return tempstr;
}

char* remove_char_at_index(char *str, int index) {
    log_message(LOG_DEBUG, "Entering function remove_char_at_index");
    
    int length = strlen(str);
    if (index < 0 || index >= length) {
        // Invalid index, do nothing
        return str;
    }

    for (int i = index; i < length - 1; i++) {
        // Shift characters to the left
        str[i] = str[i + 1];
    }
    str[length - 1] = '\0'; // Null-terminate the modified string
    
    log_message(LOG_DEBUG, "Exiting function remove_char_at_index");
    return str;
}

char* replaceParagraph(char* original, char* newSubstring, char* startPtr, char* endPtr) {
    log_message(LOG_DEBUG, "Entering function replaceParagraph");
    
    // Check for null pointers
    if (original == NULL || newSubstring == NULL || startPtr == NULL || endPtr == NULL) {
        log_message(LOG_ERROR, "Null pointer input original:%ld, %ld, %ld, %ld", (long)original, (long)newSubstring, (long)startPtr, (long)endPtr);
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
    int originalLen = strlen(original);
    int newSubLen = strlen(newSubstring);
    int replaceLen = endPtr - startPtr + 1;
    int finalLen = originalLen - replaceLen + newSubLen;

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
    fprintf(stderr, "exiting replaceParagraph\n");
    return temp;
}

char* extractParagraphWithPointerDelimiters( char *inputString,  char *startOfParagraph,  char *endOfParagraph) {
    log_message(LOG_DEBUG, "Entering function extractParagraphWithPointerDelimiters");
    
    if (inputString == NULL || startOfParagraph == NULL || endOfParagraph == NULL 
        || startOfParagraph > endOfParagraph) {
        return NULL; // Invalid parameters
    }
    
    size_t length = endOfParagraph - startOfParagraph + 1;
    char *substring = (char*)malloc(length + 1);

    if (!substring) {
        return NULL; // Memory allocation failed
    }
    
    strncpy(substring, startOfParagraph, length);
    substring[length] = '\0';  // Null-terminate the string

    
    log_message(LOG_DEBUG, "Exiting function extractParagraphWithPointerDelimiters");
    return substring;
}

char* getDocId( char* str) {
    log_message(LOG_DEBUG, "Entering function getDocId");
    
    // Find the last occurrence of '/'
    char* lastSlash = strrchr(str, '/');
    
    if (lastSlash == NULL) {
        // If no '/' is found, return NULL or an empty string
        // depending on what behavior is desired
        
        log_message(LOG_DEBUG, "Exiting function getDocId");
        return "";
    } else {
        // Return the substring after the last '/'
        
        log_message(LOG_DEBUG, "Exiting function getDocId");
        return lastSlash + 1;
    }
}

char* getParentFolder(char* str) {
    log_message(LOG_DEBUG, "Entering function getParentFolder");
    
    // Find the last occurrence of '/'
    char* lastSlash = strrchr(str, '/');
    
    if (lastSlash == NULL) {
        // If no '/' is found, return NULL or an empty string
        
        log_message(LOG_DEBUG, "Exiting function getParentFolder");
        return "";
    } else {
        // Temporarily terminate the string at the last '/'
        *lastSlash = '\0';
        
        // Find the second last occurrence of '/'
        char* secondLastSlash = strrchr(str, '/');
        
        // Restore the last slash in the original string
        *lastSlash = '/';
        
        if (secondLastSlash == NULL) {
            // If no second last '/' is found, the parent folder is the whole string before the last '/'
            
            log_message(LOG_DEBUG, "Exiting function getParentFolder");
            return strdup(str);
        } else {
            // Calculate the length of the parent folder name
            size_t len = lastSlash - secondLastSlash - 1;
            
            // Allocate memory for the result
            char* result = (char*)malloc(len + 1);
            if (result == NULL) {
                // Handle memory allocation failure
                return NULL;
            }
            
            // Copy the parent folder name
            strncpy(result, secondLastSlash + 1, len);
            result[len] = '\0'; // Null-terminate the string
            
            log_message(LOG_DEBUG, "Exiting function getParentFolder");
            return result;
        }
    }
}

char* getDirPath(char* str) {
    // Find the last occurrence of '/'
    char* lastSlash = strrchr(str, '/');
    
    if (lastSlash == NULL) {
        // If no '/' is found, return NULL or an empty string
        // depending on what behavior is desired
        // In this case, we'll return the original string
        
        log_message(LOG_DEBUG, "Exiting function getDirPath");
        return "";
    } else {
        // Calculate the length of the substring before the last '/'
        size_t len = lastSlash - str;
        
        // Allocate memory for the result
        char* result = (char*)malloc(len + 1);
        if (result == NULL) {
            // Handle memory allocation failure
            return NULL;
        }
        
        // Copy the substring before the last '/'
        strncpy(result, str, len);
        result[len] = '\0'; // Null-terminate the string
        
        log_message(LOG_DEBUG, "Exiting function getDirPath");
        return result;
    }
}

char* getDocType(char* str) {
    log_message(LOG_DEBUG, "Entering function getDocType");
    
    int length = strlen(str);
    
    // Check if the string has at least 3 characters
    if (length < 3) {
        return "String is too short";
    }
    
    
    log_message(LOG_DEBUG, "Exiting function getDocType");
    return (char*)(str + length - 3);
}

char* appendStrings(char *str1,  char *str2) {
    log_message(LOG_DEBUG, "Entering function appendStrings");
    
    // Calculate the length of str1 and str2
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    
    // Allocate memory for the combined string
    char *combined = (char *)malloc(len1 + len2 + 1); // +1 for the null terminator
    
    if (combined == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    // Copy the contents of str1 and str2 into combined
    strcpy(combined, str1);
    strcat(combined, str2);
    
    
    log_message(LOG_DEBUG, "Exiting function appendStrings");
    return combined;
}

char* removeAfterSpace(char *str) {
    log_message(LOG_DEBUG, "Entering function removeAfterSpace");
    
    char *tempstr = str;
    char *space_pos = strchr(tempstr, ' ');
    if (space_pos != NULL) {
        *space_pos = '\0'; // Replace the space with null terminator
    }

    
    log_message(LOG_DEBUG, "Exiting function removeAfterSpace");
    return tempstr;
}

char* removeLastFolder(char *path) {
    log_message(LOG_DEBUG, "Entering function removeLastFolder");

    char* tempPath = path;

    // Find the last '/'
    char *lastSlash = strrchr(tempPath, '/');
    
    // If there is a '/', terminate the string there
    if (lastSlash != NULL) {
        *lastSlash = '\0';
    } else {
        // If no '/' found, path is just a file name or empty
        // In this case, we'll just set path to an empty string
        tempPath[0] = '\0';
    }

    
    log_message(LOG_DEBUG, "Exiting function removeLastFolder");
    return tempPath;
}

char* returnTextUntil(char* str, char* delimiter) {
    log_message(LOG_DEBUG, "Entering function returnTextUntil");
    
    // Find the position of the first newline character
     char* newline_pos = strstr(str, delimiter);

    // If newline character not found, return the entire string
    if (newline_pos == NULL) {

        log_message(LOG_DEBUG, "Exiting function returnTextUntil");
        return strdup(str); // Make a copy of the string and return
    } else {
        // Calculate the length of the first part
        size_t length = newline_pos - str;

        // Allocate memory for the first part
        char* first_part = (char*)malloc(length + 1);

        // Copy the first part into the allocated memory
        strncpy(first_part, str, length);
        first_part[length] = '\0'; // Null-terminate the string

        
        log_message(LOG_DEBUG, "Exiting function returnTextUntil");
        return first_part;
    }
}

char* extractText(char *inputString, char *startDelimiter, char *endDelimiter, bool includeStart, bool includeEnd) {
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