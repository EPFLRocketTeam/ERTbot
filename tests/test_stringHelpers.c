// tests/test_module.c
#include <check.h>
#include <string.h>
#include "stringHelpers.h" // Assuming module.h declares the greet function

// Test case 1: Greeting with the name "Alice"
START_TEST(test_replace_word_A) {
    char* result = replaceWord("This is a test string with an oldWord.", "oldWord", "newWord");
    ck_assert_str_eq(result, "This is a test string with an newWord.");
}
END_TEST

// Test case 2: Greeting with the name "Bob"
START_TEST(test_replace_word_B) {
    char output[50];
    char* result = replaceWord("This is a test string with an oldWord.", "old Word", "newWord");
    ck_assert_str_eq(result, "This Test should fail.");
}
END_TEST

// Test suite setup
Suite *module_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Module");

    // Core test case
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_replace_word_A);
    tcase_add_test(tc_core, test_replace_word_B);
    suite_add_tcase(s, tc_core);

    return s;
}

// Main function to run the tests
int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = module_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? 0 : 1;
}
