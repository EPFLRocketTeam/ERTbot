# Specify the minimum version of CMake
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(ERTbot)

# Add the compile definition globally
add_compile_definitions(DEBUG)

# Add the include directory
include_directories(
    include
    include/api
    include/helpers
)

# DOES NOT CONTAIN GITHUB API or MARKDOWN to PDF
# Add the source files
set(SOURCES
    src/main.c
    src/log.c
    src/command.c
    src/api/apiHelpers.c
    src/api/sheetAPI.c
    src/api/slackAPI.c
    src/api/wikiAPI.c
    src/features/appendMentionedIn.c
    src/features/buildAcronymsList.c
    src/features/buildImageTracker.c
    src/features/buildLinksTracker.c
    src/features/buildLocalGraph.c
    src/features/buildMap.c
    src/features/createMissingRequirementPages.c
    src/features/getPages.c
    src/features/movePage.c
    src/features/onPageUpdate.c
    src/features/replaceText.c
    src/features/syncDrlToSheet.c
    src/features/syncSheetToDrl.c
    src/features/updateLinksTracker.c
    src/features/updateRequirementPage.c
    src/features/updateStatsPage.c
    src/features/updateVcdPage.c
    src/helpers/pageListHelpers.c
    src/helpers/requirementsHelpers.c
    src/helpers/stringHelpers.c
    src/helpers/timeHelpers.c
    src/helpers/wikiLinksHelpers.c
)

# Set the output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Create the executable from the source files
add_executable(ERTbot ${SOURCES})

# Link the external libraries to the target
target_link_libraries(ERTbot PRIVATE curl cjson)

