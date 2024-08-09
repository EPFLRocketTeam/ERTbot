#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#define MAX_WORD_LENGTH 20
#define MAX_WORDS 1000

//Function to insert a pageList at the end of the linked list
pageList* addPageToList(pageList** head,  char *id, char *title, char *path, char *description, char *content, char *updatedAt, char *createdAt);

//prints the argument linked list
void printList(pageList** paths);

//returns the number of '/' characters in a string.
int countSlashes(char *str);

//append string to file
void appendToFile(char *filePath, char *str);

//Prepends source_filename to destination_filename (used for adding the template to the latex file)
void prepend_file(char *source_filename, char *destination_filename);

//zips the folder
int zipFolder(char *folderPath);

//Creates the missing folders (locally) for a given path
void createMissingFolders(char *path);

//returns the current time (wiki.js timezone)
char *currentTime();

//returns -1 if time1 < time2, 1 if if time1 > time2
int compareTimes(char* time1, char* time2);

//replaces every occurence of oldString in the pages in paths *paths with newString
void replaceStringInWiki(pageList** head, char* oldString, char* newString);

//Creates plantUML diagram for the wikiMMaps
char* createMapWBS(pageList** paths);

//adds elements to a wiki list along with its properties
char* createList(char *list, pageList** sectionTitle, pageList* links);

//adds elements to a wiki list along with its properties
char* updateList(char *list, pageList *sectionTitle, pageList* links);

//Prepend the current time on a new line on the automaticUpdateTracker page
void updateLastAutomaticUpdate();

//get the time at which the last automatic update happened from the automaticUpdateTracker page
char *getLastAutomaticUpdateTime();

//used to find and parse wikiFlags for a given string
wikiFlag *parseFlags(char* text, wikiFlag flag);

//used to breakdown the command line sent through the slack into individual arguments
void breakdownCommand(char* sentence, command* cmd);

//Creates plantUML diagram for the local graphs
char* createLocalGraphMindMap(pageList** tempPage, pageList** incomingPaths, pageList** outgoingPaths);

//Function to free the links list
void freePageList(pageList** head);

//Finds accronyms in a string and appends the accronyms to the path give in argument (void but modifies local accronymList) 
void printAcronymsToFile(char* pathToAccronymList, char *str);


void sortWords(char words[MAX_WORDS][MAX_WORD_LENGTH], int numWords);


void removeDuplicatesAndSort(char *filename);

//Find page links in a string and saves the links to a pageList struct (modifies the linked list it receives and gives back pointer to head)
pageList* findPageLinks(char *content, pageList** links);

//Find image links in a string
pageList* findImageLinks(char *input, pageList** head);

// Function to filter the linked list
void filterLinks(pageList** head);

void printPages(pageList** head);

pageList* findIncomingLinks(pageList **head, char *linkTrackerContent, char *subjectPagePath);

pageList* findOutgoingLinks(pageList **head, char *linkTrackerContent, char *subjectPagePath);

void parseRequirementsList(cJSON *requirementList, char *content);

void addRequirementToCjsonObject(cJSON *requirements, char *idStr, char *pathStr, char *nameStr, char *descriptionStr);

char* parseJSONRequirementListInToArray(cJSON* requirements);

cJSON *parseArrayIntoJSONRequirementList(char *input_str);

char *buildDrlFromJSONRequirementList(cJSON *requirementList);

pageList* buildRequirementPageFromJSONRequirementList(cJSON *requirementList, char *requirementId);

void appendMentionedIn(pageList** head);

char *createVcdPieChart(char *unverifiedPopulation, char *partiallyVerifiedPopulation, char *verifiedPopulation);

char *updateVcdStackedAreaChart(char *json_str, char *week, int verifiedValue, int partiallyVerifiedValue, int unverifiedValue);
#endif