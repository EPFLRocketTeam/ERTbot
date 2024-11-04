#include <check.h>
#include <stddef.h>
#include "wikiAPI.h"
#include "ERTbot_features.h"
#include "ERTbot_common.h"
#include "pageListHelpers.h"
#include "apiHelpers.h"

START_TEST(test_createMissingRequirementPages) {

    initializeApiTokenVariables();

    //check if requirement page already exists
    pageList* pageCheck = NULL;
    pageCheck = populatePageList(&pageCheck, "exact path", "management/it/ERTbot_Test_Pages/2024_C_SE_PR_REQ_03");

    if(pageCheck != NULL){
        ck_abort_msg("A page already exists at %s", "management/it/ERTbot_Test_Pages/2024_C_SE_PR_REQ_03");
    }

    //call createMissingRequirementPages
    command cmd;
    cmd.function = NULL;
    cmd.argument_1 = "UT2";
    createMissingRequirementPages(cmd);

    //Check if page was created
    pageList* requirementPage = NULL;
    requirementPage= populatePageList(&requirementPage, "exact path", "management/it/ERTbot_Test_Pages/2024_C_SE_PR_REQ_03");
    ck_assert_str_eq(requirementPage->title, "2024_C_SE_PR_REQ_03");

    //Delete temporary requirement page
    deletePageMutation(requirementPage->id);
    freePageList(&requirementPage);
}
END_TEST



// Test suite setup
Suite *createMissingRequirementPages_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("createMissingRequirementPages");

    // Core test case
    tc_core = tcase_create("createMissingRequirementPages");
    tcase_set_timeout(tc_core, 45.0);

    tcase_add_test(tc_core, test_createMissingRequirementPages);
    suite_add_tcase(s, tc_core);

    return s;
}
