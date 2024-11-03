#include <check.h>
//#include "HEADER_FILE_NAME.h"

START_TEST(test_name) {
    //TEST
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