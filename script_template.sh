#!/bin/bash

# Set environment variables if necessary
export GITHUB_API_TOKEN="YOUR API TOKEN"
export SLACK_API_TOKEN="YOUR API TOKEN"
export GOOGLE_CLIENT_ID="YOUR API TOKEN"
export GOOGLE_CLIENT_SECRET="YOUR API TOKEN"
export GOOGLE_REFRESH_TOKEN="YOUR API TOKEN"
export WIKI_API_TOKEN="YOUR API TOKEN"
export SHEET_API_TOKEN="YOUR API TOKEN"


# Directory setup
BUILD_DIR="build"
LOG_DIR="logs"
LOG_FILES=("debug.log" "error.log" "info.log")

# Parse command-line arguments
REBUILD=false
RUN_TESTS=false
RUN_PROGRAM=false
EXECUTION_OPTION_SPECIFIED=false

while [[ "$#" -gt 0 ]]; do
    case $1 in
        --rebuild) REBUILD=true ;;               # Flag to rebuild with CMake
        --run-tests) RUN_TESTS=true; EXECUTION_OPTION_SPECIFIED=true ;;  # Flag to run tests
        --run-program) RUN_PROGRAM=true; EXECUTION_OPTION_SPECIFIED=true ;;  # Flag to run the main program
        *) echo "Unknown option: $1" ; exit 1 ;;
    esac
    shift
done

# If no execution option is specified, default to running the main program
if ! $EXECUTION_OPTION_SPECIFIED; then
    RUN_PROGRAM=true
    REBUILD=true
fi

# Rebuild the build directory if specified
if $REBUILD; then
    echo "Rebuilding the project..."
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
    mkdir "$BUILD_DIR"
    cd "$BUILD_DIR" || exit 1
    echo "Configuring the project with CMake..."
    cmake .. || { echo "CMake configuration failed"; exit 1; }
else
    echo "Using existing build directory..."
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR" || exit 1
fi

# Build the project
echo "Building the project with make..."
make || { echo "Build failed"; exit 1; }

# Return to project directory
cd ..

# Log directory and file setup
if [ ! -d "$LOG_DIR" ]; then
    echo "Creating logs directory..."
    mkdir "$LOG_DIR"
fi

for LOG_FILE in "${LOG_FILES[@]}"; do
    if [ ! -f "$LOG_DIR/$LOG_FILE" ]; then
        echo "Creating $LOG_FILE in logs directory..."
        touch "$LOG_DIR/$LOG_FILE"
    fi
done

# Run tests if specified
if $RUN_TESTS; then
    echo "Running tests..."
    ./ERTbot_tests || { echo "Tests failed"; exit 1; }
fi

# Run the main program if specified
if $RUN_PROGRAM; then
    echo "Running Program..."
    ./ERTbot || { echo "Run failed"; exit 1; }
fi
