#ifndef STRINGTOOLS
#define STRINGTOOLS

/*Replace all of occurences of wordToReplace in inputString with newWord*/
char* replaceWord(char* inputString, char* wordToReplace, char* newWord);

//Replace all occurences of "\\n" in the input string with "\n" 
char* reformatNewLineCharacter(char *inputString);

//Removes the character at index of str.
char* remove_char_at_index(char *str, int index);

/*Replace all of the string found in between startOfParagraph and
    endOfParagraph in inputString with replacementString*/
char* replaceParagraph(char *original, char *newSubstring, char *startPtr, char *endPtr);

/*Returns a sub string which is equal to the string found in between
    startOfParagraph and endOfParagraph of inputString*/
char* extractParagraphWithPointerDelimiters( char *inputString,  char *startOfParagraph,  char *endOfParagraph);

//Returns the substring after the last '/' (used to extract a documents name)
char* getDocId(char* str);

// Returns the parent folder name before the last '/' (e.g., "to" from "/path/to/document.txt")
char* getParentFolder(char* str);

// Returns the substring before the last '/' (used to extract the directory path)
char* getDirPath(char* str);

// Return a pointer to the substring starting from 3rd character from the end
char* getDocType(char* str);

//Append str2 to str1
char* appendStrings(char *str1, char *str2);

//Remove all characters after first ' ' (space character)
char* removeAfterSpace(char *str);

//removes whatever comes after the last "/" of a string
char* removeLastFolder(char *path);

//removes all of the content which comes after the delimiter argument
char* returnTextUntil(char* str, char* delimiter);

char* extractText(char *inputString, char *startDelimiter, char *endDelimiter, bool includeStart, bool includeEnd);
#endif