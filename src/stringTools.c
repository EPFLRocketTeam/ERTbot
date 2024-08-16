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
  return result; 
}

char* reformatNewLineCharacter(char *inputString) {
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

    return tempstr;
}

char* remove_char_at_index(char *str, int index) {
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

    return str;
}

char* replaceParagraph(char *original, char *newSubstring, char *startPtr, char *endPtr) {
    // Calculate the lengths
    int originalLen = strlen(original);
    int newSubLen = strlen(newSubstring);
    int replaceLen = endPtr - startPtr + 1;
    int finalLen = originalLen - replaceLen + newSubLen;

    // Create a temporary buffer to hold the modified string
    char *temp = malloc(finalLen + 1 * sizeof(char));
    memset(temp, 0, finalLen + 1);

    // Copy the part before the replaced section
    strncpy(temp, original, startPtr - original);

    // Copy the new substring
    strcat(temp, newSubstring);

    // Copy the part after the replaced section
    strcat(temp, endPtr + 1);

    // Copy the modified string back to the original
    return temp;
}

char* extractParagraphWithPointerDelimiters( char *inputString,  char *startOfParagraph,  char *endOfParagraph) {
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

    return substring;
}

char* getDocId( char* str) {
    // Find the last occurrence of '/'
    char* lastSlash = strrchr(str, '/');
    
    if (lastSlash == NULL) {
        // If no '/' is found, return NULL or an empty string
        // depending on what behavior is desired
        return "";
    } else {
        // Return the substring after the last '/'
        return lastSlash + 1;
    }
}

char* getParentFolder(char* str) {
    // Find the last occurrence of '/'
    char* lastSlash = strrchr(str, '/');
    
    if (lastSlash == NULL) {
        // If no '/' is found, return NULL or an empty string
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
        
        return result;
    }
}

char* getDocType(char* str) {
    int length = strlen(str);
    
    // Check if the string has at least 3 characters
    if (length < 3) {
        return "String is too short";
    }
    
    return (char*)(str + length - 3);
}

char* appendStrings(char *str1,  char *str2) {
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
    
    return combined;
}

char* removeAfterSpace(char *str) {
    char *tempstr = str;
    char *space_pos = strchr(tempstr, ' ');
    if (space_pos != NULL) {
        *space_pos = '\0'; // Replace the space with null terminator
    }

    return tempstr;
}

char* removeLastFolder(char *path) {

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

    return tempPath;
}

char* returnTextUntil(char* str, char* delimiter) {
    // Find the position of the first newline character
     char* newline_pos = strstr(str, delimiter);

    // If newline character not found, return the entire string
    if (newline_pos == NULL) {
        return strdup(str); // Make a copy of the string and return
    } else {
        // Calculate the length of the first part
        size_t length = newline_pos - str;

        // Allocate memory for the first part
        char* first_part = (char*)malloc(length + 1);

        // Copy the first part into the allocated memory
        strncpy(first_part, str, length);
        first_part[length] = '\0'; // Null-terminate the string

        return first_part;
    }
}

char* extractText(char *inputString, char *startDelimiter, char *endDelimiter, bool includeStart, bool includeEnd) {
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

    return substring;
}