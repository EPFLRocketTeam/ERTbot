#include <check.h>

#ifndef TESTS_SUITES_H
#define TESTS_SUITES_H

Suite *stringHelpers_suite(void);

Suite *syncDrlToSheet_suite(void);

Suite *createMissingRequirementPages_suite(void);

Suite *updateRequirementPages_suite(void);

Suite *wikiAPI_suite(void);

Suite *requirementHelpers_suite(void);

Suite *updateVcdPage_suite(void);
#endif
