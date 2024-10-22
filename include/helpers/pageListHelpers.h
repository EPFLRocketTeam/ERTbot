#ifndef PAGE_LIST_HELPERS_H
#define PAGE_LIST_HELPERS_H

/**
 * @brief Adds a new page to the end of a linked list of pages.
 * 
 * @param[in, out] head Pointer to the pointer of the `pageList` structure, representing the head of the linked list of pages.
 * @param[in] id The unique identifier for the page.
 * @param[in] title The title of the page.
 * @param[in] path The path of the page.
 * @param[in] description A description of the page.
 * @param[in] content The content of the page.
 * @param[in] updatedAt The timestamp of when the page was last updated.
 * @param[in] createdAt The timestamp of when the page was created.
 * 
 * @return A pointer to the head of the updated `pageList` with the new page added.
 * 
 * @details This function allocates memory for a new `pageList` node, copies the provided page details into the new node, 
 *          and appends it to the end of the linked list. If the list is empty, the new node becomes the head of the list.
 * 
 * @note Ensure that the provided strings (`id`, `title`, `path`, `description`, `content`, `updatedAt`, `createdAt`) are 
 *       valid and properly formatted. Memory allocated for the new node's strings should be freed when the node is no 
 *       longer needed to prevent memory leaks.
 */
pageList* addPageToList(pageList** head,  char *id, char *title, char *path, char *description, char *content, char *updatedAt, char *createdAt, char *authorId);

/**
 * @brief Frees the memory allocated for a linked list of `pageList` nodes.
 * 
 * This function iterates through the linked list starting from the head pointer, freeing all dynamically allocated
 * memory associated with each node, including the memory for the node itself. It ensures that each field of the node is
 * checked for non-NULL before attempting to free the memory.
 * 
 * @param head A pointer to a pointer to the first node of the linked list. The list will be traversed and freed until
 *             all nodes are deallocated and the head pointer is set to NULL.
 * 
 * @details The function iterates through the linked list, freeing each node's allocated memory in the following order:
 *          - `id`
 *          - `title`
 *          - `path`
 *          - `description`
 *          - `content`
 *          - `updatedAt`
 *          - `createdAt`
 * 
 *          After freeing the memory for each field, the node itself is deallocated. Debugging print statements are included
 *          to show which memory is being freed, which can be helpful for troubleshooting memory management issues.
 * 
 * @note The `pageList` structure should have fields for dynamically allocated strings. It is important to ensure that
 *       this function is called when the list is no longer needed to prevent memory leaks.
 */
void freePageList(pageList** head);

#endif