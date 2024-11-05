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
    tests
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

# Set the output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Create the executable from the source files
add_executable(ERTbot ${SOURCES})

# Conditionally configure for macOS
# Set paths for cJSON on macOS
if(APPLE)
    # Set paths for cJSON on macOS
    set(CJSON_INCLUDE_DIR /usr/local/opt/cjson/include)
    set(CJSON_LIBRARY /usr/local/opt/cjson/lib/libcjson.dylib)

    # Add the include directory and library path for ERTbot
    target_link_libraries(ERTbot PRIVATE ${CJSON_LIBRARY} curl)
    target_include_directories(ERTbot PRIVATE ${CJSON_INCLUDE_DIR})

# Configure for Linux
elseif(UNIX AND NOT APPLE)
    # Directly link with cjson on Linux, assuming it’s installed and in the linker path
    target_link_libraries(ERTbot PRIVATE curl cjson)
endif()

# Unit Testing Section
# Find pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(CHECK REQUIRED check)

# Include Check directories conditionally on macOS
if(APPLE)
    # Assuming Homebrew installed check at this path
    set(CHECK_INCLUDE_DIRS /usr/local/opt/check/include)
    set(CHECK_LIBRARIES /usr/local/opt/check/lib/libcheck.dylib)
endif()

# Include Check directories
include_directories(${CHECK_INCLUDE_DIRS})

# Test source files
set(TEST_SOURCES
    tests/features/test_createMissingRequirementPages.c
    tests/features/test_syncDrlToSheet.c
    tests/features/test_updateRequirementPages.c
    tests/helpers/test_stringHelpers.c
    tests/test_main.c
    tests/api/test_wikiAPI.c
    tests/features/test_createMissingRequirementPages.c
    tests/helpers/test_requirementHelpers.c
)

# Test executable
add_executable(ERTbot_tests ${SOURCES} ${TEST_SOURCES})
target_compile_definitions(ERTbot_tests PRIVATE TESTING)

# Add debugging symbols for the test executable
target_compile_options(ERTbot_tests PRIVATE -g)

if(APPLE)
    # Add the include directory and library path for ERTbot
    target_include_directories(ERTbot_tests PRIVATE ${CJSON_INCLUDE_DIR})
    target_link_libraries(ERTbot_tests PRIVATE ${CHECK_LIBRARIES} ${CJSON_LIBRARY} curl)

# Configure for Linux
elseif(UNIX AND NOT APPLE)
    # Directly link with cjson on Linux, assuming it’s installed and in the linker path
    target_link_libraries(ERTbot_tests PRIVATE ${CHECK_LIBRARIES} curl cjson)
endif()

# Enable CTest
enable_testing()
add_test(NAME ERTbotTests COMMAND ERTbot_tests)
