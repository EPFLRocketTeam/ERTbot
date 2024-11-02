#include <check.h>
#include "wikiAPI.h"
#include "pageListHelpers.h"
#include "apiHelpers.h"

#define DELETE_TEST_PAGE_PATH  "management/it/ERTbot_Test_Pages/deleteTestPage"

//#define DELETE_TEST_PAGE_PATH  "competition/firehorn/systems_engineering/other/ryan_homepage/deleteTestPage"



START_TEST(test_delete_page) {

    initializeApiTokenVariables();

    pageList* deleteTestPage = NULL;
    deleteTestPage = populatePageList(&deleteTestPage, "exact path", DELETE_TEST_PAGE_PATH);

    if(deleteTestPage != NULL){
        ck_abort_msg("A page already exists at %s", DELETE_TEST_PAGE_PATH);
    }

    createPageMutation(DELETE_TEST_PAGE_PATH, "foo", "Delete_Test_Page");
    deleteTestPage = populatePageList(&deleteTestPage, "exact path", DELETE_TEST_PAGE_PATH);

    ck_assert_ptr_nonnull(deleteTestPage);

    if(deleteTestPage == NULL){
        ck_abort_msg("Create page did not work");
    }

    deletePageMutation(deleteTestPage->id);

    freePageList(&deleteTestPage);

    pageList* searchForDeletedTestPage = NULL;
    searchForDeletedTestPage = populatePageList(&searchForDeletedTestPage, "exact path", DELETE_TEST_PAGE_PATH);

    ck_assert_ptr_null(searchForDeletedTestPage);

    freePageList(&searchForDeletedTestPage);
}
END_TEST



// Test suite setup
Suite *wikiAPI_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("wikiAPI");

    // Core test case
    tc_core = tcase_create("deletePageMutation");
    tcase_set_timeout(tc_core, 45.0);

    tcase_add_test(tc_core, test_delete_page);
    suite_add_tcase(s, tc_core);

    return s;
}
