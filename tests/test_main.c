#include <check.h>
#include "ERTbot_common.h"

#include "test_suites.h"


int main(void) {
    log_message(LOG_DEBUG, "\n\nStarting Tests\n\n");

    int number_failed;
    Suite *s;
    SRunner *sr;

    s = stringHelpers_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? 0 : 1;
}
