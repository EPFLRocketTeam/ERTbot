#ifndef WIKIAPI_H
#define WIKIAPI_H

//Uses page.id to get all of the other information for a given page and parses the information into the page struct (modifies the linked list it receives and gives back pointer to head)
pageList* getPage(pageList** head);

//Fills in the default update page mutation query (void but modifies pages on the wiki)
void updatePageContentMutation(pageList* head);

//renders the argument page (equivalent to refreshing the page once changes have been made to it) (void but modifies wiki)
void renderMutation(pageList** head);

//Fills in the default update page mutation query (void but modifies wiki)
void movePageContentMutation(pageList** head);

//populates a linked list with a filtered and ordered list of pages (modifies the linked list it receives and gives back pointer to head)
pageList* populatePageList(pageList** head, char *filterType, char *filterCondition);

#endif