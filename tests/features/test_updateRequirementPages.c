#include <check.h>
#include "ERTbot_features.h"
#include "ERTbot_common.h"
#include "apiHelpers.h"
#include "pageListHelpers.h"
#include "wikiAPI.h"

#define REQ_PAGE_1 "<!--2024_C_SE_PR_REQ_01-->\\n# 2024_C_SE_PR_REQ_01: PR declaration of purpose\\n>**Description**: PR shall design a bi-liquid propulsion system that will propel the LV to its target apogee.\\n>**Author**: Michaël Fuser\\n{.is-info}\\n<!--2024_C_SE_PR_REQ_01-->\\n"
#define REQ_PAGE_2 "<!--2024_C_SE_PR_REQ_02-->\\n# 2024_C_SE_PR_REQ_02: Total impulse\\n>**Description**: The propulsion system shall produce an impulse of [80000][+15000/-25000]Ns.\\n>**Author**: Michaël Fuser\\n{.is-info}\\n\\n# Verification\\n## Verification 1\\n**Method**: Test\\n**Deadline**: SIR\\n**Status**: :red_circle:Uncompleted\\n<!--2024_C_SE_PR_REQ_02-->\\n"

START_TEST(test_updateRequirementPages) {

    initializeApiTokenVariables();

    command cmd;
    cmd.function = NULL;
    cmd.argument_1 = "UT";

    updateRequirementPage(cmd);

    char* requirementPage1 = fetchAndModifyPageContent("1999", "<!--2024_C_SE_PR_REQ_01-->\\n<!--2024_C_SE_PR_REQ_01-->", requirementPage1);
    ck_assert_str_eq(requirementPage1, REQ_PAGE_1);
    free(requirementPage1);

    char* requirementPage2 = fetchAndModifyPageContent("1995", "<!--2024_C_SE_PR_REQ_02-->\\n<!--2024_C_SE_PR_REQ_02-->", requirementPage2);
    ck_assert_str_eq(requirementPage2, REQ_PAGE_2);
    free(requirementPage2);
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
