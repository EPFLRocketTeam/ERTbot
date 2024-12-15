#include <check.h>
#include <stdlib.h>
#include <string.h>
#include "ERTbot_common.h" // Your error handling functions and definitions

// Test adding an error to the error list
START_TEST(test_add_error) {
    clearErrorList(); // Ensure a clean state for testing

    addError(1001, "Test error 1", "testFunction1");
    ErrorState *error = getLatestError();
    
    ck_assert_ptr_nonnull(error);
    ck_assert_int_eq(error->errorCode, 1001);
    ck_assert_str_eq(error->errorMessage, "Test error 1");
    ck_assert_str_eq(error->functionName, "testFunction1");
}
END_TEST

// Test adding multiple errors and retrieving the latest one
START_TEST(test_add_multiple_errors) {
    clearErrorList(); // Ensure a clean state for testing

    addError(1001, "Test error 1", "testFunction1");
    addError(1002, "Test error 2", "testFunction2");
    
    ErrorState *error = getLatestError();
    
    ck_assert_ptr_nonnull(error);
    ck_assert_int_eq(error->errorCode, 1002);
    ck_assert_str_eq(error->errorMessage, "Test error 2");
    ck_assert_str_eq(error->functionName, "testFunction2");
}
END_TEST

// Test clearing the error list
START_TEST(test_clear_error_state) {
    addError(1001, "Test error 1", "testFunction1");
    clearErrorList();

    ErrorState *error = getLatestError();
    ck_assert_ptr_null(error); // Error list should now be empty
}
END_TEST

// Test error memory management (ensure no memory leaks)
START_TEST(test_memory_management) {
    clearErrorList();
    addError(1001, "Test error 1", "testFunction1");
    addError(1002, "Test error 2", "testFunction2");

    clearErrorList(); // Clear all errors
    ck_assert_ptr_null(getLatestError()); // Ensure no dangling pointers
}
END_TEST

// Test handling of long error messages
START_TEST(test_long_error_message) {
    clearErrorList();

    const char *longMessage = "This is a very long error message that should still work correctly when stored in the error state.";
    addError(1003, longMessage, "testFunction3");
    
    ErrorState *error = getLatestError();
    ck_assert_ptr_nonnull(error);
    ck_assert_str_eq(error->errorMessage, longMessage);
}
END_TEST

// Test adding error without function name
START_TEST(test_add_error_no_function_name) {
    clearErrorList();

    addError(1004, "Error without function name", NULL);
    ErrorState *error = getLatestError();
    
    ck_assert_ptr_nonnull(error);
    ck_assert_str_eq(error->functionName, "Unknown"); // Assuming you handle NULL function name as "Unknown"
}
END_TEST

// Test edge case: adding an error with empty message
START_TEST(test_empty_error_message) {
    clearErrorList();

    addError(1005, "", "testFunction5");
    ErrorState *error = getLatestError();
    
    ck_assert_ptr_nonnull(error);
    ck_assert_str_eq(error->errorMessage, "Unknown error"); // Assuming you handle empty messages as "Unknown error"
}
END_TEST

START_TEST(test_add_error_2) {
    clearErrorList(); // Ensure a clean state for testing

    addError(1001, "Test error 1", "testFunction1");

    ErrorState *error = getErrorList();
    ck_assert_ptr_nonnull(error);
    ck_assert_int_eq(error->errorCode, 1001);
    ck_assert_str_eq(error->errorMessage, "Test error 1");
    ck_assert_str_eq(error->functionName, "testFunction1");
}
END_TEST

START_TEST(test_add_multiple_errors_2) {
    clearErrorList();

    addError(1001, "Test error 1", "testFunction1");
    addError(1002, "Test error 2", "testFunction2");

    // Traverse the list to the last error
    ErrorState *error = getErrorList();
    while (error && error->next != NULL) {
        error = error->next;
    }

    ck_assert_ptr_nonnull(error);
    ck_assert_int_eq(error->errorCode, 1002);
    ck_assert_str_eq(error->errorMessage, "Test error 2");
    ck_assert_str_eq(error->functionName, "testFunction2");
}
END_TEST

START_TEST(test_remove_last_error_empty_list) {
    // Test case when the error list is empty
    clearErrorList();  // Ensure the list is empty initially

    removeLastError(); // Try to remove an error when the list is empty

    ErrorState *error = getLatestError();
    ck_assert_ptr_null(error);  // The list should still be empty, so no error should exist
}
END_TEST

START_TEST(test_remove_last_error_single_error) {
    // Test case when there is only one error in the list
    clearErrorList();  // Ensure the list is empty initially

    addError(1001, "Test Error 1", "testFunction1");  // Add one error

    // The list should now have one error
    ErrorState *errorBefore = getLatestError();
    ck_assert_ptr_nonnull(errorBefore);
    ck_assert_int_eq(errorBefore->errorCode, 1001);

    // Remove the last error
    removeLastError();

    // After removal, the list should be empty
    ErrorState *errorAfter = getLatestError();
    ck_assert_ptr_null(errorAfter);  // The list should be empty now
}
END_TEST

START_TEST(test_remove_last_error_multiple_errors) {
    // Test case when there are multiple errors in the list
    clearErrorList();  // Ensure the list is empty initially

    addError(1002, "Test Error 2", "testFunction2");
    addError(1003, "Test Error 3", "testFunction3");

    // The list should have two errors now, with 1003 being the most recent
    ErrorState *errorBefore = getLatestError();
    ck_assert_ptr_nonnull(errorBefore);
    ck_assert_int_eq(errorBefore->errorCode, 1003);

    // Remove the last error
    removeLastError();

    // After removal, the most recent error should be 1002
    ErrorState *errorAfter = getLatestError();
    ck_assert_ptr_nonnull(errorAfter);
    ck_assert_int_eq(errorAfter->errorCode, 1002);
}
END_TEST


// Test suite setup
Suite *errorState_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("errorState");

    // Core test case
    tc_core = tcase_create("Core");
    tcase_set_timeout(tc_core, 45.0);

    tcase_add_test(tc_core, test_add_error);
    tcase_add_test(tc_core, test_add_multiple_errors);
    tcase_add_test(tc_core, test_add_error_2);
    tcase_add_test(tc_core, test_add_multiple_errors_2);
    tcase_add_test(tc_core, test_clear_error_state);
    tcase_add_test(tc_core, test_memory_management);
    tcase_add_test(tc_core, test_long_error_message);
    tcase_add_test(tc_core, test_add_error_no_function_name);
    tcase_add_test(tc_core, test_empty_error_message);
    tcase_add_test(tc_core, test_remove_last_error_empty_list);
    tcase_add_test(tc_core, test_remove_last_error_single_error);
    tcase_add_test(tc_core, test_remove_last_error_multiple_errors);
    suite_add_tcase(s, tc_core);

    return s;
}
