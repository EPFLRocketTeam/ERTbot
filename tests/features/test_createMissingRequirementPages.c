#include <check.h>
#include <stddef.h>
#include "wikiAPI.h"
#include "ERTbot_features.h"
#include "ERTbot_common.h"
//#include "HEADER_FILE_NAME.h"

START_TEST(test_name) {
    //Check if requierement page exists

    pageList* pageCheck = NULL;
    pageCheck= populatePageList(&pageCheck, "exact path", "management/it/ERTbot_Test_Pages/Unit_Test_REQ_03");

    ck_assert_str_eq(pageCheck->id, NULL);

    command cmd;
    cmd.function = NULL;
    cmd.argument_1 = "UT";

    createMissingRequirementPages(cmd);

    pageList* requirementPage = NULL;
    requirementPage= populatePageList(&requirementPage, "exact path", "management/it/ERTbot_Test_Pages/Unit_Test_REQ_03");

    ck_assert_str_eq(requirementPage->title, "2024_C_SE_PR_REQ_03");

    deletePageMutation(requirementPage->id);
}
END_TEST



// Test suite setup
Suite *SOURCE_FILE_NAME_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("SOURCE_FILE_NAME");

    // Core test case
    tc_core = tcase_create("FUNCTION_NAME");
    tcase_set_timeout(tc_core, 45.0);

    tcase_add_test(tc_core, test_name);
    suite_add_tcase(s, tc_core);

    return s;
}
