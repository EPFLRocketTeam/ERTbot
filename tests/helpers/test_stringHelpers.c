#include <check.h>
#include "stringHelpers.h"

START_TEST(test_replace_word_A) {
    char* result = replaceWord("This is a test string with an oldWord.", "oldWord", "newWord");
    ck_assert_str_eq(result, "This is a test string with an newWord.");
}
END_TEST

START_TEST(test_replace_word_B) {
    char output[50];
    char* result = replaceWord("This is a test string with an oldWord.", "old Word", "newWord");
    ck_assert_str_eq(result, "This is a test string with an oldWord.");
}
END_TEST


// Test suite setup
Suite *stringHelpers_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("stringHelpers");

    // Core test case
    tc_core = tcase_create("replaceWord");
    tcase_set_timeout(tc_core, 45.0);

    tcase_add_test(tc_core, test_replace_word_A);
    tcase_add_test(tc_core, test_replace_word_B);
    suite_add_tcase(s, tc_core);

    return s;
}