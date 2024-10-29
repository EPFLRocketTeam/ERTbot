#include <check.h>
#include "ERTbot_features.h"
#include "ERTbot_common.h"
#include "apiHelpers.h"
#include "pageListHelpers.h"
#include "wikiAPI.h"

#define REQ_PAGE_2 "<!--2024_C_SE_PR_REQ_02-->\\n# 2024_C_SE_PR_REQ_02: Total impulse\\n>**Description**: The propulsion system shall produce an impulse of [80000][+15000/-25000]Ns.\\n>**Author**: Michaël Fuser\\n{.is-info}\\n\\n# Verification\\n## Verification 1\\n**Method**: Test\\n**Deadline**: SIR\\n**Status**: :red_circle:Uncompleted\\n<!--2024_C_SE_PR_REQ_02-->\\n"

START_TEST(test_updateRequirementPages) {
    //TEST

    initializeApiTokenVariables();

    command cmd;
    cmd.function = NULL;
    cmd.argument_1 = "UT";

    updateRequirementPage(cmd);

    pageList* reqPage1 = NULL;
    reqPage1 = addPageToList(&reqPage1, "1995", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    getPage(&reqPage1);

    //ck_assert_str_eq(reqPage1->content, UT_DRL);

    //modify req Page
    //update req Page

    freePageList(&reqPage1);

    pageList* reqPage2 = NULL;
    reqPage2 = addPageToList(&reqPage2, "1996", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    getPage(&reqPage2);

    ck_assert_str_eq(reqPage2->content, REQ_PAGE_2);

    freePageList(&reqPage2);

    //modify req Page
    //update req Page

}
END_TEST


// Test suite setup
Suite *updateRequirementPages_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("updateRequirementPages");

    // Core test case
    tc_core = tcase_create("updateRequirementPages");
    tcase_set_timeout(tc_core, 45.0);

    tcase_add_test(tc_core, test_updateRequirementPages);
    suite_add_tcase(s, tc_core);

    return s;
}