#include <check.h>
#include "ERTbot_common.h"
#include "wikiAPI.h"
#include "pageListHelpers.h"
#include "apiHelpers.h"

#define DELETE_TEST_PAGE_PATH  "management/it/ERTbot_Test_Pages/deleteTestPage"

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

START_TEST(test_fetchAndModifyPageContent) {

    initializeApiTokenVariables();

    pageList* testPage = NULL;
    testPage = addPageToList(&testPage, "2015", "", "", "", "", "", "", "");
    testPage = getPage(&testPage);

    ck_assert_str_eq(testPage->content, "foo");

    char *outputString;
    outputString = fetchAndModifyPageContent(testPage->id, "bar", outputString);
    ck_assert_str_eq(outputString, "foo");

    free(outputString);

    testPage = getPage(&testPage);
    ck_assert_str_eq(testPage->content, "bar");

    char* outputString2;
    outputString2 = fetchAndModifyPageContent(testPage->id, "foo", outputString2);
    ck_assert_str_eq(outputString2, "bar");

    free(outputString2);
    freePageList(&testPage);
}
END_TEST

// Test suite setup
Suite *wikiAPI_suite(void) {
    Suite *s;
    TCase *tc_core1, *tc_core2;

    s = suite_create("wikiAPI");

    // Core test case
    tc_core1 = tcase_create("deletePageMutation");
    tcase_set_timeout(tc_core1, 45.0);
    tcase_add_test(tc_core1, test_delete_page);
    suite_add_tcase(s, tc_core1);

    tc_core2 = tcase_create("fetchAndModifyPageContent");
    tcase_set_timeout(tc_core2, 45.0);
    tcase_add_test(tc_core2, test_fetchAndModifyPageContent);
    suite_add_tcase(s, tc_core2);

    return s;
}
