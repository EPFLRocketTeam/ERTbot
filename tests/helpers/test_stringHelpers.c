#include <check.h>
#include <stdlib.h>
#include <string.h>
#include "stringHelpers.h"

START_TEST(test_replace_word_malloc_A) {
    char* result = replaceWord_Malloc("This is a test string with an oldWord.", "oldWord", "newWord");
    ck_assert_str_eq(result, "This is a test string with an newWord.");
    free(result);
}
END_TEST

START_TEST(test_replace_word_malloc_B) {
    char* result = replaceWord_Malloc("This is a test string with an oldWord.", "old Word", "newWord");
    ck_assert_str_eq(result, "This is a test string with an oldWord.");
    free(result);
}
END_TEST

START_TEST(test_replace_word_realloc_A) {
    char* result = strdup("This is a test string with an oldWord.");
    result = replaceWord_Realloc(result, "oldWord", "newWord");
    ck_assert_str_eq(result, "This is a test string with an newWord.");
    free(result);
}
END_TEST

START_TEST(test_replace_word_realloc_B) {
    char* result = strdup("This is a test string with an oldWord.");
    result = replaceWord_Realloc(result, "old Word", "newWord");
    ck_assert_str_eq(result, "This is a test string with an oldWord.");
    free(result);
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

    tcase_add_test(tc_core, test_replace_word_malloc_A);
    tcase_add_test(tc_core, test_replace_word_malloc_B);
    tcase_add_test(tc_core, test_replace_word_realloc_A);
    tcase_add_test(tc_core, test_replace_word_realloc_B);
    suite_add_tcase(s, tc_core);

    return s;
}