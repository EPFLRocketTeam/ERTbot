#include <check.h>
#include "ERTbot_common.h"
#include "wikiAPI.h"
#include "pageListHelpers.h"
#include "apiHelpers.h"

#define DELETE_TEST_PAGE_PATH  "management/it/ERTbot_Test_Pages/deleteTestPage"

const char* jsonParserTestData = "{\"data\": {\"query\": {\"list\": [{\"path\":\"about\",\"title\":\"About\",\"id\":380,\"updatedAt\":\"2023-12-05T23:24:18.467Z\"},{\"path\":\"about/acronyms\",\"title\":\"Acronyms\",\"id\":382,\"updatedAt\":\"2024-11-13T16:37:30.844Z\"},{\"path\":\"about/ert_spaces\",\"title\":\"ERT Spaces\",\"id\":384,\"updatedAt\":\"2024-09-27T14:14:20.403Z\"},{\"path\":\"about/memepedia\",\"title\":\"ERT Memepedia™\",\"id\":658,\"updatedAt\":\"2024-10-17T15:19:40.149Z\"},{\"path\":\"about/nomenclature\",\"title\":\"Nomenclature\",\"id\":350,\"updatedAt\":\"2024-09-27T15:11:29.606Z\"},{\"path\":\"about/stats\",\"title\":\"Wiki Statistics\",\"id\":1178,\"updatedAt\":\"2024-09-16T05:50:01.554Z\"},{\"path\":\"about/team_organisation\",\"title\":\"Team Organisation\",\"id\":381,\"updatedAt\":\"2024-10-04T12:48:39.598Z\"},{\"path\":\"admin\",\"title\":\"Wiki Help and Guidelines\",\"id\":2,\"updatedAt\":\"2024-03-01T21:41:01.996Z\"},{\"path\":\"competition\",\"title\":\"Competition\",\"id\":355,\"updatedAt\":\"2024-06-02T13:07:50.320Z\"},{\"path\":\"competition/bella_lui_2\",\"title\":\"Bella Lui II\",\"id\":27,\"updatedAt\":\"2024-11-26T17:57:56.481Z\"},{\"path\":\"competition/bella_lui_2/avionics\",\"title\":\"Avionics\",\"id\":32,\"updatedAt\":\"2023-12-01T02:18:38.855Z\"},{\"path\":\"competition/bella_lui_2/flight_dynamics\",\"title\":\"Flight Dynamics\",\"id\":37,\"updatedAt\":\"2023-12-01T02:18:41.939Z\"},{\"path\":\"competition/bella_lui_2/flight_dynamics/cernier_launch_09_10_2021\",\"title\":\"Cernier Launch (09/10/2021)\",\"id\":115,\"updatedAt\":\"2023-12-01T02:09:44.204Z\"},{\"path\":\"competition/bella_lui_2/flight_dynamics/hermes1\",\"title\":\"Hermes I Overview\",\"id\":109,\"updatedAt\":\"2023-12-01T02:09:46.582Z\"},{\"path\":\"competition/bella_lui_2/flight_dynamics/kaltbrunn_launch_04_09_2021\",\"title\":\"Kaltbrunn Launch (04/09/2021)\",\"id\":114,\"updatedAt\":\"2023-12-01T02:09:49.060Z\"},{\"path\":\"competition/bella_lui_2/flight_dynamics/kaltbrunn_launch_12_06_2021\",\"title\":\"Kaltbrunn Launch (12/06/2021)\",\"id\":113,\"updatedAt\":\"2023-12-01T02:09:51.834Z\"},{\"path\":\"competition/bella_lui_2/ground_segment\",\"title\":\"Ground Segment\",\"id\":33,\"updatedAt\":\"2023-12-01T02:18:44.501Z\"},{\"path\":\"competition/bella_lui_2/payload\",\"title\":\"Payload\",\"id\":34,\"updatedAt\":\"2023-12-01T02:18:47.428Z\"},{\"path\":\"competition/bella_lui_2/propulsion\",\"title\":\"Propulsion\",\"id\":35,\"updatedAt\":\"2023-12-01T02:18:50.445Z\"},{\"path\":\"competition/bella_lui_2/recovery\",\"title\":\"Recovery\",\"id\":36,\"updatedAt\":\"2023-12-01T02:18:53.078Z\"},{\"path\":\"competition/bella_lui_2/structure\",\"title\":\"Structure\",\"id\":38,\"updatedAt\":\"2023-12-01T02:18:55.620Z\"}]}}}";

const char* jsonParserTestDataTime = "{\"data\": {\"query\": {\"list\": [{\"path\":\"management/it/ERTbot_Test_Pages/Unit_Test_REQ_02\",\"title\":\"2024_C_SE_PR_REQ_02\",\"id\":1996,\"updatedAt\":\"2024-12-22T12:15:10.979Z\"},{\"path\":\"management/it/ERTbot_Test_Pages/Unit_Test_REQ_01\",\"title\":\"2024_C_SE_PR_REQ_01\",\"id\":1999,\"updatedAt\":\"2024-12-22T12:15:10.586Z\"},{\"path\":\"management/it/ERTbot_Test_Pages/fetchAndModifyTest\",\"title\":\"fetchAndModifyTest\",\"id\":2015,\"updatedAt\":\"2024-12-22T12:14:37.408Z\"},{\"path\":\"management/it/ERTbot_Test_Pages/DRL_Test\",\"title\":\"DRL_Test\",\"id\":1995,\"updatedAt\":\"2024-12-22T12:14:28.720Z\"},{\"path\":\"systems_engineering/avionics-hw-handbook\",\"title\":\"Hardware Engineering Handbook\",\"id\":2820,\"updatedAt\":\"2024-12-21T22:07:35.853Z\"}]}}}";



START_TEST(test_delete_page) {

    initializeApiTokenVariables();

    pageList* deleteTestPage = NULL;
    deleteTestPage = populatePageList(&deleteTestPage, "exact path", DELETE_TEST_PAGE_PATH);

    if(deleteTestPage != NULL){
        ck_abort_msg("A page already exists at %s", DELETE_TEST_PAGE_PATH);
    }

    createPageMutation(DELETE_TEST_PAGE_PATH, "foo", "Delete_Test_Page");
    deleteTestPage = populatePageList(&deleteTestPage, "exact path", DELETE_TEST_PAGE_PATH);

    ck_assert_ptr_nonnull(deleteTestPage);

    if(deleteTestPage == NULL){
        ck_abort_msg("Create page did not work");
    }

    deletePageMutation(deleteTestPage->id);

    freePageList(&deleteTestPage);

    pageList* searchForDeletedTestPage = NULL;
    searchForDeletedTestPage = populatePageList(&searchForDeletedTestPage, "exact path", DELETE_TEST_PAGE_PATH);

    ck_assert_ptr_null(searchForDeletedTestPage);

    freePageList(&searchForDeletedTestPage);
    freeChunkResponse();
}
END_TEST

START_TEST(test_fetchAndModifyPageContent) {

    initializeApiTokenVariables();

    pageList* testPage = NULL;
    testPage = addPageToList(&testPage, "2015", NULL, NULL, NULL, NULL, NULL);
    testPage = getPage(&testPage);

    ck_assert_str_eq(testPage->content, "foo");

    char *outputString;
    outputString = fetchAndModifyPageContent(testPage->id, "bar", outputString);
    ck_assert_str_eq(outputString, "foo");
    freePageList(&testPage);

    free(outputString);

    pageList* testPage2 = NULL;
    testPage2 = addPageToList(&testPage2, "2015", NULL, NULL, NULL, NULL, NULL);
    testPage2 = getPage(&testPage2);
    ck_assert_str_eq(testPage2->content, "bar");

    char* outputString2;
    outputString2 = fetchAndModifyPageContent(testPage2->id, "foo", outputString2);
    ck_assert_str_eq(outputString2, "bar");

    free(outputString2);
    freePageList(&testPage2);
    freeChunkResponse();
}
END_TEST


// Unit test setup
START_TEST(test_parseJSON_valid_filter_path) {
    pageList* head = NULL;
    const char* filterType = "path";
    const char* filterCondition = "about";  // filter to match

    head = parseJSON(&head, jsonParserTestData, filterType, filterCondition);

    ck_assert_ptr_nonnull(head);
    for(int i = 0; i<6; i++){
        switch (i) {
            case 0:
                ck_assert_str_eq(head->path, "about");
                ck_assert_str_eq(head->title, "About");
                ck_assert_str_eq(head->id, "380");
                ck_assert_str_eq(head->updatedAt, "2023-12-05T23:24:18.467Z");
                break;
            case 1:
                ck_assert_str_eq(head->path, "about/acronyms");
                ck_assert_str_eq(head->title, "Acronmys");
                ck_assert_str_eq(head->id, "382");
                ck_assert_str_eq(head->updatedAt, "2024-11-13T16:37:30.844Z");
                break;
            case 2:
                ck_assert_str_eq(head->path, "about/ert_spaces");
                ck_assert_str_eq(head->title, "ERT Spaces");
                ck_assert_str_eq(head->id, "384");
                ck_assert_str_eq(head->updatedAt, "2024-09-27T14:14:20.403Z");
                break;
            case 3:
                ck_assert_str_eq(head->path, "about/memepedia");
                ck_assert_str_eq(head->title, "ERT Memepedia™");
                ck_assert_str_eq(head->id, "658");
                ck_assert_str_eq(head->updatedAt, "2024-10-17T15:19:40.149Z");
                break;
            case 4:
                ck_assert_str_eq(head->path, "about/nomenclature");
                ck_assert_str_eq(head->title, "Nomenclature");
                ck_assert_str_eq(head->id, "350");
                ck_assert_str_eq(head->updatedAt, "2024-09-27T15:11:29.606Z");
                break;
            case 5:
                ck_assert_str_eq(head->path, "about/stats");
                ck_assert_str_eq(head->title, "Wiki Statistics");
                ck_assert_str_eq(head->id, "1178");
                ck_assert_str_eq(head->updatedAt, "2024-09-16T05:50:01.554Z");
                break;
            head = head->next;
        }
    }
}
END_TEST

START_TEST(test_parseJSON_invalid_json_format) {
    pageList* head = NULL;
    const char* jsonString = "{\"invalid\":[{\"path\":\"/page1\",\"title\":\"Page 1\",\"id\":\"1\",\"updatedAt\":\"2024-12-22\"}]}";
    const char* filterType = "path";
    const char* filterCondition = "/page1";

    head = parseJSON(&head, jsonString, filterType, filterCondition);

    // Since the JSON format is invalid, head should remain NULL
    ck_assert_ptr_eq(head, NULL);
}
END_TEST

START_TEST(test_parseJSON_no_matching_page) {
    pageList* head = NULL;
    const char* jsonString = "{\"list\": [{\"path\":\"/page1\",\"title\":\"Page 1\",\"id\":\"1\",\"updatedAt\":\"2024-12-22\"}]}";
    const char* filterType = "path";
    const char* filterCondition = "/page2";  // No match

    head = parseJSON(&head, jsonString, filterType, filterCondition);

    // No matching page should be added to the list
    ck_assert_ptr_eq(head, NULL);
}
END_TEST

START_TEST(test_parseJSON_filter_by_time) {
    pageList* head = NULL;
    const char* filterType = "time";
    const char* filterCondition = "2024-12-22T12:00:28.720Z";  // filter for earlier time

    head = parseJSON(&head, jsonParserTestDataTime, filterType, filterCondition);

    ck_assert_ptr_nonnull(head);
    for(int i = 0; i<4; i++){
        switch (i) {
            case 0:
                ck_assert_str_eq(head->path, "management/it/ERTbot_Test_Pages/Unit_Test_REQ_02");
                ck_assert_str_eq(head->title, "2024_C_SE_PR_REQ_02");
                ck_assert_str_eq(head->id, "1996");
                ck_assert_str_eq(head->updatedAt, "2024-12-22T12:15:10.979Z");
                break;
            case 1:
                ck_assert_str_eq(head->path, "management/it/ERTbot_Test_Pages/Unit_Test_REQ_01");
                ck_assert_str_eq(head->title, "2024_C_SE_PR_REQ_01");
                ck_assert_str_eq(head->id, "1999");
                ck_assert_str_eq(head->updatedAt, "2024-12-22T12:15:10.586Z");
                break;
            case 2:
                ck_assert_str_eq(head->path, "management/it/ERTbot_Test_Pages/fetchAndModifyTest");
                ck_assert_str_eq(head->title, "fetchAndModifyTest");
                ck_assert_str_eq(head->id, "2015");
                ck_assert_str_eq(head->updatedAt, "2024-12-22T12:14:37.408Z");
                break;
            case 3:
                ck_assert_str_eq(head->path, "management/it/ERTbot_Test_Pages/DRL_Test");
                ck_assert_str_eq(head->title, "DRL_Test");
                ck_assert_str_eq(head->id, "1995");
                ck_assert_str_eq(head->updatedAt, "2024-12-22T12:14:28.720Z");
                break;
            head = head->next;
        }
    }
}
END_TEST

START_TEST(test_parseJSON_exact_path_match) {
    pageList* head = NULL;
    const char* filterType = "exact path";
    const char* filterCondition = "about/acronyms";  // Exact match

    head = parseJSON(&head, jsonParserTestData, filterType, filterCondition);

    ck_assert_ptr_nonnull(head);
    ck_assert_str_eq(head->path, "about/acronyms");
    ck_assert_str_eq(head->title, "Acronmys");
    ck_assert_str_eq(head->id, "382");
    ck_assert_str_eq(head->updatedAt, "2024-11-13T16:37:30.844Z");

    // After finding an exact match, it should stop processing further
    ck_assert_ptr_eq(head->next, NULL);
}
END_TEST



// Test suite setup
Suite *wikiAPI_suite(void) {
    Suite *s;
    TCase *tc_core1;
    TCase *tc_core2;
    TCase *tc_core3;

    s = suite_create("wikiAPI");

    // Core test case
    tc_core1 = tcase_create("deletePageMutation");
    tcase_set_timeout(tc_core1, 45.0);
    tcase_add_test(tc_core1, test_delete_page);
    suite_add_tcase(s, tc_core1);

    tc_core2 = tcase_create("fetchAndModifyPageContent");
    tcase_set_timeout(tc_core2, 45.0);
    tcase_add_test(tc_core2, test_fetchAndModifyPageContent);
    suite_add_tcase(s, tc_core2);

    tc_core3 = tcase_create("parseJSON");
    tcase_set_timeout(tc_core3, 45.0);
    tcase_add_test(tc_core3, test_parseJSON_valid_filter_path);
    tcase_add_test(tc_core3, test_parseJSON_invalid_json_format);
    tcase_add_test(tc_core3, test_parseJSON_no_matching_page);
    tcase_add_test(tc_core3, test_parseJSON_filter_by_time);
    tcase_add_test(tc_core3, test_parseJSON_exact_path_match);
    suite_add_tcase(s, tc_core3);

    return s;
}
