#include "common.h"
#include "config.h"
#include <stdio.h>


#define MAX_WORD_LENGTH 20
#define MAX_WORDS 1000

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
static void printAcronymsToFile(char* pathToAccronymList, char *str);

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
static void sortWords(char words[MAX_WORDS][MAX_WORD_LENGTH], int numWords);

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
static void removeDuplicatesAndSort(char *filename);

void buildAcronymsList(command cmd) {
    log_message(LOG_DEBUG, "Entering function buildAcronymsList");
    
    pageList* listOfAllPages;
    if (cmd.argument_1 == NULL){
        listOfAllPages = populatePageList(&listOfAllPages, "path", "none");
    }
    else{listOfAllPages = populatePageList(&listOfAllPages, "path", cmd.argument_1);}

    //Watch out maybe getPage whould not be done all in one go ?
    listOfAllPages = getPage(&listOfAllPages);
    pageList* current = listOfAllPages;
    listOfAllPages = getPage(&listOfAllPages);
    while (current != NULL) {
        printAcronymsToFile(ACCRONYM_LIST_PATH, current->content);
        current = current->next;
    }
    freePageList(&listOfAllPages);
    freePageList(&current);
    removeDuplicatesAndSort(ACCRONYM_LIST_PATH);
    
    log_message(LOG_DEBUG, "Exiting function buildAcronymsList");
}

static void printAcronymsToFile(char* pathToAccronymList, char *str) {
    log_message(LOG_DEBUG, "Entering function printAcronymsToFile");
    
    int len = strlen(str);
    int i;
    int j = 1;
    char acronym[11]; // Maximum acronym length is 10, plus 1 for null terminator

    for (i = 0; i < len-1; i++) {
        // Check if current character is uppercase letter
        if (str[i] == '('){
            while (str[i] != ')'){
                i++;
            }
        }

        if (str[i] == '`' && str[i+1] == '`' && str[i+2] == '`'){
            i++;
            while (!(str[i] == '`' && str[i+1] == '`' && str[i+2] == '`')){
                i++;
            }
        }

        if (isupper(str[i])) {
            // Check for words of length 2 to 10
            while(isupper(str[i+j]) && j < 9 && str[i+j] != ' '){
                j++;
            }
            if(j>1){
                strncpy(acronym, &str[i], j);
                acronym[j] = '\0';
            }
            i = i+j;
            j = 1;
        }
    }
    
    log_message(LOG_DEBUG, "Exiting function printAcronymsToFile");
}

static void sortWords(char words[MAX_WORDS][MAX_WORD_LENGTH], int numWords) {
    log_message(LOG_DEBUG, "Entering function sortWords");
    
    // Bubble sort algorithm
    for (int i = 0; i < numWords - 1; i++) {
        for (int j = 0; j < numWords - i - 1; j++) {
            if (strcmp(words[j], words[j + 1]) > 0) {
                char temp[MAX_WORD_LENGTH];
                strcpy(temp, words[j]);
                strcpy(words[j], words[j + 1]);
                strcpy(words[j + 1], temp);
            }
        }
    }
    
    log_message(LOG_DEBUG, "Exiting function sortWords");
}

static void removeDuplicatesAndSort(char *filename) {
    log_message(LOG_DEBUG, "Entering function removeDuplicatesAndSort");
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        log_message(LOG_ERROR, "Error opening file.");
        return;
    }

    char words[MAX_WORDS][MAX_WORD_LENGTH];
    int numWords = 0;

    // Read words from the file and store them in an array
    while (fgets(words[numWords], MAX_WORD_LENGTH, file) != NULL) {
        // Remove newline character
        words[numWords][strcspn(words[numWords], "\n")] = '\0';
        numWords++;
    }

    fclose(file);

    // Sort the words alphabetically
    sortWords(words, numWords);

    // Open file for writing
    file = fopen(filename, "w");
    if (file == NULL) {
        log_message(LOG_ERROR, "Error opening file.");
        return;
    }

    // Write unique words back to the file
    for (int i = 0; i < numWords; i++) {
        if (i == 0 || strcmp(words[i], words[i - 1]) != 0) {
            fprintf(file, "%s\n", words[i]);
        }
    }

    fclose(file);
    
    log_message(LOG_DEBUG, "Exiting function removeDuplicatesAndSort");
}