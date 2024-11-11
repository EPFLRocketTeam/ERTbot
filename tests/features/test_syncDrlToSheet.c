// tests/test_module.c
#include <check.h>
#include <string.h>
#include "stringHelpers.h"
#include "ERTbot_features.h"
#include "wikiAPI.h"
#include "ERTbot_common.h"
#include "pageListHelpers.h"
#include "apiHelpers.h"

#define UT_DRL "# Propulsion Design Requirements List\\n# table {.tabset}\\n\\n\\n## General\\n- [2024_C_SE_PR_REQ_01](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_REQ_01) **PR declaration of purpose**\\nPR shall design a bi-liquid propulsion system that will propel the LV to its target apogee.\\n- [2024_C_SE_PR_REQ_02](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_REQ_02) **Total impulse**\\nThe propulsion system shall produce an impulse of [80000][+15000/-25000]Ns.\\n- [2024_C_SE_PR_REQ_03](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_REQ_03) **Safe mode**\\nThe propulsion system shall be in a fail safe mode when not powered, meaning it cannot be ignited.\\n- [2024_C_SE_PR_REQ_04](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_REQ_04) **Ignition conditions**\\nThe propulsion system shall require [2] disctinct commands to start the ignition sequence.\\n- [2024_C_SE_PR_REQ_05](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_REQ_05) **Manual abort function**\\nThe propulsion subsystem shall be compatible with the launch abort procedure designed by AV.\\n- [2024_C_SE_PR_REQ_06](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_REQ_06) **Bottles fitting**\\nThe propulsion subsystem shall be compatible with the propellant bottles provided by EuRoC.\\n- [2024_C_SE_PR_REQ_08](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_REQ_08) **Tank offloading, launch abort**\\nThe tanks offloading or venting shall be performed remotely in the event of a launch abort.\\n- [2024_C_SE_PR_REQ_10](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_REQ_10) **Propellant toxicity**\\nAll propellants used shall be non-toxic, ie requiring no special storage and transport infrastructure or extensive personal protection equipment \\n- [2024_C_SE_PR_REQ_11](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_REQ_11) **Assembly human needs**\\nThe assembly of the PR systems shall require at most [3] operators.\\n- [2024_C_SE_PR_REQ_13](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_REQ_13) **Tanks venting measures**\\nPressure relief measures shall be implemented to account for the LV potentially sitting a long time in waiting on the launch rail.\\n- [2024_C_SE_PR_REQ_15](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_REQ_15) **Relief device**\\nThe LV pressure vessels shall implement a relief device, set to open at no greater than the proof pressure.\\n- [2024_C_SE_PR_REQ_18](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_REQ_18) **Water-hammer phenomena**\\nWater‐hammer phenomena shall have no detrimental effect on the structural and the functional behaviour of the propulsion system.\\n{.links-list}\\n\\n\\n## Engine\\n- [2024_C_SE_PR_ENGINE_REQ_01](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_ENGINE_REQ_01) **Declaration of purpose**\\nThe engine shall deliver the thrust required to lift the LV.\\n- [2024_C_SE_PR_ENGINE_REQ_02](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_ENGINE_REQ_02) **Required thrust**\\nThe motor shall provide a peak thrust of [5000][+2500/-0]N.\\n- [2024_C_SE_PR_ENGINE_REQ_03](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_ENGINE_REQ_03) **Mechanical interface**\\nThe motor loads shall be transmitted to the rest of the LV using the thrust plate, designed by ST.\\n- [2024_C_SE_PR_ENGINE_REQ_04](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_ENGINE_REQ_04) **Engine bay module mass**\\nThe engine bay module shall have a maximum weight of [9000]g.\\n- [2024_C_SE_PR_ENGINE_REQ_05](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_ENGINE_REQ_05) **Engine bay PR length**\\nThe engine module shall fit within the engine bay which is composed of a bay of length [700][+/-20]mm and a boattail of length [250][+/-20]mm.\\n{.links-list}\\n\\n\\n## Fluid System\\n- [2024_C_SE_PR_FLUID-SYSTEM_REQ_01](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_FLUID-SYSTEM_REQ_01) **Declaration of purpose**\\nThe fluid system shall ensure safe transport of all propulsion fluids accross the different pressure vessels.\\n- [2024_C_SE_PR_FLUID-SYSTEM_REQ_02](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_FLUID-SYSTEM_REQ_02) **Burst disks**\\nEach tank shall incoporate a replaceable burst disk.\\n- [2024_C_SE_PR_FLUID-SYSTEM_REQ_12](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_FLUID-SYSTEM_REQ_12) **Burst disks orrifice diameter**\\nThe burst disks shall have a diaphragm orifice diameter greater than [6]mm.\\n- [2024_C_SE_PR_FLUID-SYSTEM_REQ_13](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_FLUID-SYSTEM_REQ_13) **Burst disk rupture pressure**\\nThe burst disks shall be calibrated to rupture at [70][-0/+2]bars.\\n- [2024_C_SE_PR_FLUID-SYSTEM_REQ_14](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_FLUID-SYSTEM_REQ_14) **Burst disk burst pressure**\\nThe burst disks shall be designed to burst at no less than [120] bars.\\n- [2024_C_SE_PR_FLUID-SYSTEM_REQ_15](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_FLUID-SYSTEM_REQ_15) **Valve safety switch**\\nThe fuel-side propellant valve shall feature a valve disabling safety switch.\\n- [2024_C_SE_PR_FLUID-SYSTEM_REQ_16](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_FLUID-SYSTEM_REQ_16) **Valve safety switch 2 **\\nThe oxidizer-side propellant valve shall feature a valve disabling safety switch.\\n- [2024_C_SE_PR_FLUID-SYSTEM_REQ_17](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_FLUID-SYSTEM_REQ_17) **Mechanical fixation**\\nAll elements of the fluid system shall remain fixed to the LV under axial accelerations of [600]m/s^2.\\n- [2024_C_SE_PR_FLUID-SYSTEM_REQ_05](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_FLUID-SYSTEM_REQ_05) **Pressurant bay module dry mass**\\nThe pressurant module dry mass shall not exceed [6500]g.\\n- [2024_C_SE_PR_FLUID-SYSTEM_REQ_06](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_FLUID-SYSTEM_REQ_06) **Pressurant bay module wet mass**\\nThe pressurant module wet mass shall not exceed [8000]g.\\n- [2024_C_SE_PR_FLUID-SYSTEM_REQ_07](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_FLUID-SYSTEM_REQ_07) **Pressurant bay module dimensions**\\nThe PR pressurant module shall fit within a cylinder of diameter of [190]mm and length [650]mm\\n- [2024_C_SE_PR_FLUID-SYSTEM_REQ_08](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_FLUID-SYSTEM_REQ_08) **Mid bay module mass**\\nThe mid bay module mass shall not exceed [2800]g.\\n- [2024_C_SE_PR_FLUID-SYSTEM_REQ_09](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_FLUID-SYSTEM_REQ_09) **Mid bay module length**\\nThe mid bay module shall have a maximum length of [280]mm.\\n- [2024_C_SE_PR_FLUID-SYSTEM_REQ_10](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_FLUID-SYSTEM_REQ_10) **Pressure relief valves**\\nAll isolated sections of pressurized lines (including pressure vessels) shall incorporate a passive pressure relief device (PRD) with an opening set point below the maximum tested pressure of the line section.\\n- [2024_C_SE_PR_FLUID-SYSTEM_REQ_11](/competition/firehorn/systems_engineering/requirements/2024_C_SE_DRL/2024_C_SE_PR_DRL/2024_C_SE_PR_FLUID-SYSTEM_REQ_11) **Pressure relief discharge coefficient**\\nAll pressure relief devices shall have a discharge coefficient equal to or higher than any other fluid interface on the respective pressurized section in which they are installed.\\n{.links-list}"


START_TEST(test_syncDrlToSheet_1) {

    initializeApiTokenVariables();

    command cmd;
    cmd.function = NULL;
    cmd.argument = "UT_DRL_1";

    syncDrlToSheet(cmd);

    pageList* drlPage = NULL;
    drlPage = addPageToList(&drlPage, "1995", NULL, NULL, NULL, NULL, NULL);

    getPage(&drlPage);

    ck_assert_str_eq(drlPage->content, UT_DRL);

    freePageList(&drlPage);
}
END_TEST

START_TEST(test_syncDrlToSheet_2) {

    initializeApiTokenVariables();

    command cmd;
    cmd.function = NULL;
    cmd.argument = "UT_DRL_2";

    syncDrlToSheet(cmd);

    pageList* drlPage = NULL;
    drlPage = addPageToList(&drlPage, "1995", NULL, NULL, NULL, NULL, NULL);

    getPage(&drlPage);

    ck_assert_str_eq(drlPage->content, UT_DRL);

    freePageList(&drlPage);
}
END_TEST

START_TEST(test_syncDrlToSheet_3) {

    initializeApiTokenVariables();

    command cmd;
    cmd.function = NULL;
    cmd.argument = "UT_DRL_3";

    syncDrlToSheet(cmd);

    pageList* drlPage = NULL;
    drlPage = addPageToList(&drlPage, "1995", NULL, NULL, NULL, NULL, NULL);

    getPage(&drlPage);

    ck_assert_str_eq(drlPage->content, "You are missing an, id, description or title value.{.links-list}");

    freePageList(&drlPage);
}
END_TEST

START_TEST(test_syncDrlToSheet_4) {

    initializeApiTokenVariables();

    command cmd;
    cmd.function = NULL;
    cmd.argument = "UT_DRL_4";

    syncDrlToSheet(cmd);

    pageList* drlPage = NULL;
    drlPage = addPageToList(&drlPage, "1995", NULL, NULL, NULL, NULL, NULL);

    getPage(&drlPage);

    ck_assert_str_eq(drlPage->content, UT_DRL);

    freePageList(&drlPage);
}
END_TEST

START_TEST(test_syncDrlToSheet_5) {

    initializeApiTokenVariables();

    command cmd;
    cmd.function = NULL;
    cmd.argument = "UT_DRL_5";

    syncDrlToSheet(cmd);

    pageList* drlPage = NULL;
    drlPage = addPageToList(&drlPage, "1995", NULL, NULL, NULL, NULL, NULL);

    getPage(&drlPage);

    ck_assert_str_eq(drlPage->content, "There was an error when parsing the requirements, you might be missing a header value.");

    freePageList(&drlPage);
}
END_TEST

// Test suite setup
Suite *syncDrlToSheet_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("syncDrlToSheet");

    // Core test case
    tc_core = tcase_create("Core");
    tcase_set_timeout(tc_core, 45.0);

    tcase_add_test(tc_core, test_syncDrlToSheet_1);
    tcase_add_test(tc_core, test_syncDrlToSheet_2);
    tcase_add_test(tc_core, test_syncDrlToSheet_3);
    tcase_add_test(tc_core, test_syncDrlToSheet_4);
    tcase_add_test(tc_core, test_syncDrlToSheet_5);
    suite_add_tcase(s, tc_core);

    return s;
}
