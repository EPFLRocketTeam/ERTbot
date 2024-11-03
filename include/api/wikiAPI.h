#ifndef ERTBOT_WIKIAPI_H
#define ERTBOT_WIKIAPI_H

#include <stdbool.h>
#include "ERTbot_common.h"

/**
 * @brief Retrieves and updates the content of a page from the Wiki API.
 *
 * @param[in,out] head Pointer to a pointer to the `pageList` structure. The function updates the page details for the page
 *                     pointed to by `*head`.
 *
 * @return Pointer to the `pageList` structure with updated details.
 *
 * @details This function calls `getPageContentQuery` to fetch page content using the ID stored in the `pageList` node pointed
 *          to by `*head`. It then parses the JSON response to extract and populate various fields of the `pageList` structure:
 *          `id`, `title`, `path`, `description`, `content`, `updatedAt`, and `createdAt`. The function uses `jsonParserGetStringValue`
 *          to extract string values from the JSON response. Memory allocated for `chunk.response` is freed after use.
 *
 * @note Ensure that `getPageContentQuery` and `jsonParserGetStringValue` functions are correctly implemented and that `chunk.response`
 *       contains valid JSON data.
 */
pageList* getPage(pageList** head);

/**
 * @brief Updates the content of a page in the Wiki using a mutation query.
 *
 * @param[in] head Pointer to the `pageList` structure containing the page details to be updated.
 *
 * @details This function constructs a mutation query to update the content of a page by replacing placeholders in the
 *          `template_update_page_mutation` with the page ID and new content from the `pageList` structure pointed to by `head`.
 *          It then sends this query to the Wiki API using the `wikiApi` function.
 *
 * @note Ensure that `replaceWord` and `wikiApi` functions are correctly implemented. The `template_update_page_mutation` should
 *       be a valid mutation query template, and `head->id` and `head->content` should contain appropriate values.
 */
void updatePageContentMutation(pageList* head);

/**
 * @brief Renders pages by sending a mutation query for each page in the list.
 *
 * @param[in, out] head Pointer to the pointer of the `pageList` structure, representing the head of the linked list of pages.
 *
 * @details This function iterates through each page in the linked list starting from the `head` pointer. For each page, it
 *          constructs a mutation query using the `template_render_page_mutation` template by replacing placeholders with the
 *          current page's ID. It then sends this query to the Wiki API using the `wikiApi` function.
 *
 * @note Ensure that `replaceWord` and `wikiApi` functions are correctly implemented. The `template_render_page_mutation` should
 *       be a valid mutation query template. The list of pages should be properly initialized and linked.
 */
void renderMutation(pageList** head, bool renderEntireList);

/**
 * @brief Moves the content of pages by sending a mutation query for each page in the list.
 *
 * @param[in, out] head Pointer to the pointer of the `pageList` structure, representing the head of the linked list of pages.
 *
 * @details This function iterates through each page in the linked list starting from the `head` pointer. For each page, it
 *          constructs a mutation query using the `template_move_page_mutation` template by replacing placeholders with the
 *          current page's ID and path. It then sends this query to the Wiki API using the `wikiApi` function.
 *
 * @note Ensure that `replaceWord` and `wikiApi` functions are correctly implemented. The `template_move_page_mutation` should
 *       be a valid mutation query template. The list of pages should be properly initialized and linked.
 */
void movePageMutation(pageList** head);

/**
 * @brief Populates a linked list of pages based on a filter type and condition.
 *
 * @param[in, out] head Pointer to the pointer of the `pageList` structure, representing the head of the linked list of pages.
 * @param[in] filterType The type of filter to apply when querying the page list (e.g., "path" or "time").
 * @param[in] filterCondition The condition to use for filtering the pages (e.g., a specific path or time criteria).
 *
 * @return A pointer to the populated `pageList`. This pointer may be the same as `*head` or a new list, depending on
 *         the implementation of `parseJSON`.
 *
 * @details This function calls `getListQuery` with the specified `filterType` to fetch a list of pages. It then parses
 *          the response using `parseJSON` to populate the linked list of pages based on the `filterCondition`. The updated
 *          list is returned.
 *
 * @note Ensure that `getListQuery`, `parseJSON`, and `chunk.response` are correctly handled. The `filterType` should match
 *       the expected types used in `getListQuery`, and `filterCondition` should be relevant to the filter criteria.
 */
pageList* populatePageList(pageList** head, const char *filterType, const char *filterCondition);


void createPageMutation(const char* path, const char* content, const char* title);


char *fetchAndModifyPageContent(const char* pageId, const char* newPageContent, char* outputString);

void deletePageMutation(const char* id);
#endif
