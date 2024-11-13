#include <check.h>
#include <string.h>
#include <unistd.h>
#include "stringHelpers.h"
#include "ERTbot_features.h"
#include "wikiAPI.h"
#include "ERTbot_common.h"
#include "pageListHelpers.h"
#include "apiHelpers.h"
#include "sheetAPI.h"

START_TEST(test_updateVcdPage_1) {
    initializeApiTokenVariables();
    refreshOAuthToken();

    command cmd;
    cmd.function = NULL;
    cmd.argument = "UT_VCD_1";

    updateVcdPage(cmd);

    pageList* vcdPage = NULL;
    vcdPage = addPageToList(&vcdPage, "2667", NULL, NULL, NULL, NULL, NULL);

    getPage(&vcdPage);

    ck_assert_str_eq(vcdPage->content, "");

    freePageList(&vcdPage);
}
END_TEST



// Test suite setup
Suite *updateVcdPage_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("updateVcdPage");

    // Core test case
    tc_core = tcase_create("core");
    tcase_set_timeout(tc_core, 45.0);

    tcase_add_test(tc_core, test_updateVcdPage_1);
    suite_add_tcase(s, tc_core);

    return s;
}
