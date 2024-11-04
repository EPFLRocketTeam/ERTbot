#include <check.h>
#include <cjson/cJSON.h>
#include "requirementsHelpers.h"
#include "ERTbot_common.h"
#include "apiHelpers.h"
#include "sheetAPI.h"

START_TEST(test_getSubsystemInfo) {
    initializeApiTokenVariables();
    refreshOAuthToken();

    cJSON* subsystem = getSubsystemInfo("UT");

    ck_assert_str_eq(cJSON_GetObjectItem(subsystem, "Name")->valuestring, "Unit Test");
    ck_assert_str_eq(cJSON_GetObjectItem(subsystem, "Acronym")->valuestring, "UT");
    ck_assert_str_eq(cJSON_GetObjectItem(subsystem, "DRL Page ID")->valuestring, "1995");
    ck_assert_str_eq(cJSON_GetObjectItem(subsystem, "Req_DB Spreadsheet ID")->valuestring, "1i_PTwIqLuG9IUI73UaGuOvx8rVTDV1zIS7gmXNjMs1I");
    ck_assert_str_eq(cJSON_GetObjectItem(subsystem, "Req_DB Sheet Acronym and Range")->valuestring, "UT!A2:AT300");
    ck_assert_str_eq(cJSON_GetObjectItem(subsystem, "Requirement Pages Directory")->valuestring, "competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_ST_DRL/");

    cJSON_Delete(subsystem);


}
END_TEST



// Test suite setup
Suite *requirementHelpers_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("requirementHelpers");

    // Core test case
    tc_core = tcase_create("getSubsystemInfo");
    tcase_set_timeout(tc_core, 45.0);

    tcase_add_test(tc_core, test_getSubsystemInfo);
    suite_add_tcase(s, tc_core);

    return s;
}
