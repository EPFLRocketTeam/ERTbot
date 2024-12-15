#include <check.h>
#include "ERTbot_common.h"
#include "test_suites.h"


int main(void) {
    log_message(LOG_DEBUG, "\n\nStarting Tests\n\n");

    int number_failed;
    Suite *s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8;
    SRunner *sr;

    s1 = stringHelpers_suite();
    sr = srunner_create(s1);

    s2 = syncDrlToSheet_suite();
    srunner_add_suite(sr, s2);

    s3 = wikiAPI_suite();
    srunner_add_suite(sr, s3);

    s4 = createMissingRequirementPages_suite();
    srunner_add_suite(sr, s4);

    s5 = updateRequirementPages_suite();
    srunner_add_suite(sr, s5);

    s6 = requirementHelpers_suite();
    srunner_add_suite(sr, s6);

    s7 = errorState_suite();
    srunner_add_suite(sr, s7);

    //s8 = updateVcdPage_suite();
    //srunner_add_suite(sr, s7);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? 0 : 1;
}
