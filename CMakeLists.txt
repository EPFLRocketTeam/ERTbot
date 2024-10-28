# Specify the minimum version of CMake
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(ERTbot)

# Add the compile definition globally
add_compile_definitions(DEBUG)

# Allows clangd (lsp) to see include files
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Add the include directory
include_directories(
    include
    include/api
    include/helpers
)

# Add the source files
set(SOURCES
    src/main.c
    src/log.c
    src/command.c
    src/api/apiHelpers.c
    src/api/sheetAPI.c
    src/api/slackAPI.c
    src/api/wikiAPI.c
    src/features/createMissingRequirementPages.c
    src/features/syncDrlToSheet.c
    src/features/updateRequirementPage.c
    src/features/updateVcdPage.c
    src/helpers/pageListHelpers.c
    src/helpers/requirementsHelpers.c
    src/helpers/stringHelpers.c
    src/helpers/timeHelpers.c
    src/helpers/wikiLinksHelpers.c
)

# Unit Testing Section
# Find pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(CHECK REQUIRED check)

# Include Check directories
include_directories(${CHECK_INCLUDE_DIRS})

# Test source files
set(TEST_SOURCES
    tests/test_stringHelpers.c  # Example test source file
)

# Test executable
add_executable(ERTbot_tests ${SOURCES} ${TEST_SOURCES})
target_compile_definitions(ERTbot_tests PRIVATE TESTING)
target_link_libraries(ERTbot_tests PRIVATE ${CHECK_LIBRARIES} curl cjson)

# Enable CTest
enable_testing()
add_test(NAME ERTbotTests COMMAND ERTbot_tests)