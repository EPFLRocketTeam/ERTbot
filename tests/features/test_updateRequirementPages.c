#include <check.h>
#include "ERTbot_features.h"
#include "ERTbot_common.h"
#include "apiHelpers.h"
#include "pageListHelpers.h"
#include "wikiAPI.h"
#include "sheetAPI.h"

#define REQ_PAGE_1_1 "<!--2024_C_SE_PR_REQ_01-->\\n# 2024_C_SE_PR_REQ_01: PR declaration of purpose\\n>**Description**: PR shall design a bi-liquid propulsion system that will propel the LV to its target apogee.\\n\\n>**Author**: Michaël Fuser\\n{.is-info}\\n<!--2024_C_SE_PR_REQ_01-->"
#define REQ_PAGE_2_1 "<!--2024_C_SE_PR_REQ_02-->\\n# 2024_C_SE_PR_REQ_02: Total impulse\\n>**Description**: The propulsion system shall produce an impulse of [80000][+15000/-25000]Ns.\\n\\n>**Author**: Michaël Fuser\\n{.is-info}\\n\\n# Verification\\n## Verification 1\\n**Method**: Test\\n**Deadline**: SIR\\n**Status**: :red_circle:Uncompleted\\n<!--2024_C_SE_PR_REQ_02-->"

#define REQ_PAGE_1_2 "<!--2024_C_SE_PR_REQ_01-->\\n# 2024_C_SE_PR_REQ_01: PR declaration of purpose\\n>**Description**: PR shall design a bi-liquid propulsion system that will propel the LV to its target apogee.\\n\\n<!--2024_C_SE_PR_REQ_01-->"
#define REQ_PAGE_2_2 "<!--2024_C_SE_PR_REQ_02-->\\n# 2024_C_SE_PR_REQ_02: Total impulse\\n>**Description**: The propulsion system shall produce an impulse of [80000][+15000/-25000]Ns.\\n\\n\\n# Verification\\n## Verification 1\\n**Method**: Test\\n**Deadline**: SIR\\n**Status**: :red_circle:Uncompleted\\n<!--2024_C_SE_PR_REQ_02-->"

#define REQ_PAGE_1_3 "<!--2024_C_SE_PR_REQ_01-->\\n# 2024_C_SE_PR_REQ_01: PR declaration of purpose\\n>**Description**: PR shall design a bi-liquid propulsion system that will propel the LV to its target apogee.\\n\\nFoo.\\n\\n<!--2024_C_SE_PR_REQ_01-->"
#define REQ_PAGE_2_3 "<!--2024_C_SE_PR_REQ_02-->\\n# 2024_C_SE_PR_REQ_02: Total impulse\\n>**Description**: The propulsion system shall produce an impulse of [80000][+15000/-25000]Ns.\\n\\n\\n# Verification\\n## Verification 1\\n**Method**: Test\\n**Deadline**: SIR\\n**Status**: :red_circle:Uncompleted\\n<!--2024_C_SE_PR_REQ_02-->"

START_TEST(test_updateRequirementPages_1) {

    initializeApiTokenVariables();
    
    refreshOAuthToken();

    command cmd;
    cmd.function = NULL;
    cmd.argument = "UT_REQ_1";

    pageList* checkPage1 = NULL;
    checkPage1 = addPageToList(&checkPage1, "1999", NULL, NULL, NULL, NULL, NULL);
    getPage(&checkPage1);
    if(strcmp(checkPage1->content, "<!--2024_C_SE_PR_REQ_01-->\\n<!--2024_C_SE_PR_REQ_01-->")!=0){
        log_message(LOG_ERROR, "%s: %s", checkPage1->title, checkPage1->content);
        ck_abort_msg("%s was not reset properly at last test run", checkPage1->title);
    }
    freePageList(&checkPage1);

    pageList* checkPage2 = NULL;
    checkPage2 = addPageToList(&checkPage2, "1996", NULL, NULL, NULL, NULL, NULL);
    getPage(&checkPage2);
    if(strcmp(checkPage2->content, "<!--2024_C_SE_PR_REQ_02-->\\n<!--2024_C_SE_PR_REQ_02-->")!=0){
        log_message(LOG_ERROR, "%s: %s", checkPage2->title, checkPage2->content);
        ck_abort_msg("%s was not reset properly at last test run", checkPage2->title);
    }
    freePageList(&checkPage2);

    updateRequirementPage(cmd);

    char* requirementPage1 = fetchAndModifyPageContent("1999", "<!--2024_C_SE_PR_REQ_01-->\\\\n<!--2024_C_SE_PR_REQ_01-->", requirementPage1);
    char* requirementPage2 = fetchAndModifyPageContent("1996", "<!--2024_C_SE_PR_REQ_02-->\\\\n<!--2024_C_SE_PR_REQ_02-->", requirementPage2);

    ck_assert_str_eq(requirementPage1, REQ_PAGE_1_1);
    ck_assert_str_eq(requirementPage2, REQ_PAGE_2_1);

    free(requirementPage1);
    free(requirementPage2);
}
END_TEST

START_TEST(test_updateRequirementPages_2) {

    initializeApiTokenVariables();
    
    refreshOAuthToken();

    command cmd;
    cmd.function = NULL;
    cmd.argument = "UT_REQ_2";

    pageList* checkPage1 = NULL;
    checkPage1 = addPageToList(&checkPage1, "1999", NULL, NULL, NULL, NULL, NULL);
    getPage(&checkPage1);
    if(strcmp(checkPage1->content, "<!--2024_C_SE_PR_REQ_01-->\\n<!--2024_C_SE_PR_REQ_01-->")!=0){
        log_message(LOG_ERROR, "%s: %s", checkPage1->title, checkPage1->content);
        ck_abort_msg("%s was not reset properly at last test run", checkPage1->title);
    }
    freePageList(&checkPage1);

    pageList* checkPage2 = NULL;
    checkPage2 = addPageToList(&checkPage2, "1996", NULL, NULL, NULL, NULL, NULL);
    getPage(&checkPage2);
    if(strcmp(checkPage2->content, "<!--2024_C_SE_PR_REQ_02-->\\n<!--2024_C_SE_PR_REQ_02-->")!=0){
        log_message(LOG_ERROR, "%s: %s", checkPage2->title, checkPage2->content);
        ck_abort_msg("%s was not reset properly at last test run", checkPage2->title);
    }
    freePageList(&checkPage2);

    updateRequirementPage(cmd);

    char* requirementPage1 = fetchAndModifyPageContent("1999", "<!--2024_C_SE_PR_REQ_01-->\\\\n<!--2024_C_SE_PR_REQ_01-->", requirementPage1);
    char* requirementPage2 = fetchAndModifyPageContent("1996", "<!--2024_C_SE_PR_REQ_02-->\\\\n<!--2024_C_SE_PR_REQ_02-->", requirementPage2);

    ck_assert_str_eq(requirementPage1, REQ_PAGE_1_1);
    ck_assert_str_eq(requirementPage2, REQ_PAGE_2_1);

    free(requirementPage1);
    free(requirementPage2);
}
END_TEST

START_TEST(test_updateRequirementPages_3) {

    initializeApiTokenVariables();
    
    refreshOAuthToken();

    command cmd;
    cmd.function = NULL;
    cmd.argument = "UT_REQ_3";

    pageList* checkPage1 = NULL;
    checkPage1 = addPageToList(&checkPage1, "1999", NULL, NULL, NULL, NULL, NULL);
    getPage(&checkPage1);
    if(strcmp(checkPage1->content, "<!--2024_C_SE_PR_REQ_01-->\\n<!--2024_C_SE_PR_REQ_01-->")!=0){
        log_message(LOG_ERROR, "%s: %s", checkPage1->title, checkPage1->content);
        ck_abort_msg("%s was not reset properly at last test run", checkPage1->title);
    }
    freePageList(&checkPage1);

    pageList* checkPage2 = NULL;
    checkPage2 = addPageToList(&checkPage2, "1996", NULL, NULL, NULL, NULL, NULL);
    getPage(&checkPage2);
    if(strcmp(checkPage2->content, "<!--2024_C_SE_PR_REQ_02-->\\n<!--2024_C_SE_PR_REQ_02-->")!=0){
        log_message(LOG_ERROR, "%s: %s", checkPage2->title, checkPage2->content);
        ck_abort_msg("%s was not reset properly at last test run", checkPage2->title);
    }
    freePageList(&checkPage2);

    updateRequirementPage(cmd);

    char* requirementPage1 = fetchAndModifyPageContent("1999", "<!--2024_C_SE_PR_REQ_01-->\\\\n<!--2024_C_SE_PR_REQ_01-->", requirementPage1);
    char* requirementPage2 = fetchAndModifyPageContent("1996", "<!--2024_C_SE_PR_REQ_02-->\\\\n<!--2024_C_SE_PR_REQ_02-->", requirementPage2);

    ck_assert_str_eq(requirementPage1, REQ_PAGE_1_2);
    ck_assert_str_eq(requirementPage2, REQ_PAGE_2_2);

    free(requirementPage1);
    free(requirementPage2);
}
END_TEST

START_TEST(test_updateRequirementPages_4) {
    log_message(LOG_DEBUG, "Entering function test_updateRequirementPages_4");
    initializeApiTokenVariables();
    
    refreshOAuthToken();

    command cmd;
    cmd.function = NULL;
    cmd.argument = "UT_REQ_4";

    pageList* checkPage1 = NULL;
    checkPage1 = addPageToList(&checkPage1, "1999", NULL, NULL, NULL, NULL, NULL);
    getPage(&checkPage1);
    if(strcmp(checkPage1->content, "<!--2024_C_SE_PR_REQ_01-->\\n<!--2024_C_SE_PR_REQ_01-->")!=0){
        log_message(LOG_ERROR, "%s: %s", checkPage1->title, checkPage1->content);
        ck_abort_msg("%s was not reset properly at last test run", checkPage1->title);
    }
    freePageList(&checkPage1);

    pageList* checkPage2 = NULL;
    checkPage2 = addPageToList(&checkPage2, "1996", NULL, NULL, NULL, NULL, NULL);
    getPage(&checkPage2);
    if(strcmp(checkPage2->content, "<!--2024_C_SE_PR_REQ_02-->\\n<!--2024_C_SE_PR_REQ_02-->")!=0){
        log_message(LOG_ERROR, "%s: %s", checkPage2->title, checkPage2->content);
        ck_abort_msg("%s was not reset properly at last test run", checkPage2->title);
    }
    freePageList(&checkPage2);

    updateRequirementPage(cmd);

    char* requirementPage1 = fetchAndModifyPageContent("1999", "<!--2024_C_SE_PR_REQ_01-->\\\\n<!--2024_C_SE_PR_REQ_01-->", requirementPage1);
    char* requirementPage2 = fetchAndModifyPageContent("1996", "<!--2024_C_SE_PR_REQ_02-->\\\\n<!--2024_C_SE_PR_REQ_02-->", requirementPage2);

    ck_assert_str_eq(requirementPage1, REQ_PAGE_1_1);
    ck_assert_str_eq(requirementPage2, REQ_PAGE_2_1);

    free(requirementPage1);
    free(requirementPage2);
    log_message(LOG_DEBUG, "Exiting function test_updateRequirementPages_4");
}
END_TEST

START_TEST(test_updateRequirementPages_5) {

    initializeApiTokenVariables();
    
    refreshOAuthToken();

    command cmd;
    cmd.function = NULL;
    cmd.argument = "UT_REQ_5";

    pageList* checkPage1 = NULL;
    checkPage1 = addPageToList(&checkPage1, "1999", NULL, NULL, NULL, NULL, NULL);
    getPage(&checkPage1);
    if(strcmp(checkPage1->content, "<!--2024_C_SE_PR_REQ_01-->\\n<!--2024_C_SE_PR_REQ_01-->")!=0){
        log_message(LOG_ERROR, "%s: %s", checkPage1->title, checkPage1->content);
        ck_abort_msg("%s was not reset properly at last test run", checkPage1->title);
    }
    freePageList(&checkPage1);

    pageList* checkPage2 = NULL;
    checkPage2 = addPageToList(&checkPage2, "1996", NULL, NULL, NULL, NULL, NULL);
    getPage(&checkPage2);
    if(strcmp(checkPage2->content, "<!--2024_C_SE_PR_REQ_02-->\\n<!--2024_C_SE_PR_REQ_02-->")!=0){
        log_message(LOG_ERROR, "%s: %s", checkPage2->title, checkPage2->content);
        ck_abort_msg("%s was not reset properly at last test run", checkPage2->title);
    }
    freePageList(&checkPage2);

    updateRequirementPage(cmd);

    char* requirementPage1 = fetchAndModifyPageContent("1999", "<!--2024_C_SE_PR_REQ_01-->\\\\n<!--2024_C_SE_PR_REQ_01-->", requirementPage1);
    char* requirementPage2 = fetchAndModifyPageContent("1996", "<!--2024_C_SE_PR_REQ_02-->\\\\n<!--2024_C_SE_PR_REQ_02-->", requirementPage2);

    ck_assert_str_eq(requirementPage1, REQ_PAGE_1_3);
    ck_assert_str_eq(requirementPage2, REQ_PAGE_2_3);

    free(requirementPage1);
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

    tcase_add_test(tc_core, test_updateRequirementPages_1);
    tcase_add_test(tc_core, test_updateRequirementPages_2);
    tcase_add_test(tc_core, test_updateRequirementPages_3);
    tcase_add_test(tc_core, test_updateRequirementPages_4);
    tcase_add_test(tc_core, test_updateRequirementPages_5);
    suite_add_tcase(s, tc_core);

    return s;
}
