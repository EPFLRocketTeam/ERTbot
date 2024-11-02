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
if [ -d "$BUILD_DIR" ]; then
    rm -rf "$BUILD_DIR"  # Remove existing build directory if it exists
fi
mkdir "$BUILD_DIR"
cd "$BUILD_DIR" || exit 1

# Run CMake to configure the build
echo "Configuring the project with CMake..."
cmake .. || { echo "CMake configuration failed"; exit 1; }

# Build the project
echo "Building the project..."
make || { echo "Build failed"; exit 1; }

# Build the project
echo "Returning to project directory"
cd ..

# Optional: Cleanup build artifacts
# Uncomment the following line if you want to clean up after running the tests
# rm -rf "$BUILD_DIR"

# Run tests
# echo "Running tests..."
#./ERTbot_tests || { echo "Tests failed"; exit 1; }

# Run Program
echo "Running Program"
./ERTbot || { echo "Run failed"; exit 1; }
