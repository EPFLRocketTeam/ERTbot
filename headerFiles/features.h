#ifndef FEATURES_H
#define FEATURES_H

// Mass replace all strings
void replaceText(command cmd);

//Moves the page at argument_1 to argument_2 and then replaces all occurence of argument_1 with argument_2 on all pages in the wiki
void movePage(command cmd);

//builds a wikimap starting from cmd.argument_1 all the way down
char* buildMap(command cmd);

//Finds all of the page links on the wiki and saves the to LINK_TRACKER_PAGE_ID
void buildLinksTracker(command cmd);

//Update the page at LINK_TRACKER_PAGE_ID for all of the pages which have been modified since the last time LINK_TRACKER_PAGE_ID has been modified
void updateLinksTracker( );

//Finds all of the image links on the wiki and saves the to IMAGE_TRACKER_PAGE_ID
void buildImageTracker(command cmd);

// Build local graph
void buildLocalGraph(command cmd);

//Build accronyme list and save locally
void buildAcronymsList(command cmd);

//Test function to return the contents of several pages
void getPages(command cmd);

void syncSheetToDrl(command cmd);

void syncDrlToSheet(command cmd);

void createRequirementPage(command cmd);

void createVcdPage(command cmd);
#endif