# ERTbot
The ERTbot is a program which can automate certain tasks across several platforms. It is mainly used for syncing requirements from a google sheet into different forms on our wiki.js instance.

# Installing
## Requiremets
To set up the project, ensure you have the following dependencies installed:

1. libcurl: For calling the Slack, Google Sheets, Wiki.js, and GitHub APIs.
Linux:
```bash
sudo apt-get update
sudo apt-get install libcurl4-openssl-dev
```

macOS:
```bash
brew install curl
```

2. cJSON: For handling cJSON formatted data.
Linux:
```bash
sudo apt-get install libcjson-dev
```

macOS:
```bash
brew install cjson
```

3. Check: For unit testing.
Linux:
```bash
sudo apt-get install check
sudo apt-get install pkg-config
```

macOS:
```bash
brew install check
```


4. CMake: For building the project.
Linux:
```bash
sudo apt-get install cmake
```

macOS:
```bash
brew install cmake
```

## Download Project

## Config

## Create script for running
1. Create a file called ```build_and_run.sh``` which will be the script you will call to build and run the program.

2. Paste this into the contents of the file
```
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

# Run tests
echo "Running Program"
./ERTbot || { echo "Tests failed"; exit 1; }

# Optional: Cleanup build artifacts
# Uncomment the following line if you want to clean up after running the tests
# rm -rf "$BUILD_DIR"
```

3. Add you API tokens
Replace all of the ```YOUR API TOKEN``` with your API tokens.

4. Make the script
```bash
chmod +x build_and_run.sh
```

5. Add log files

6. Run the program with your new script

```bash
./build_and_run
```

# Running Tests
```
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

# Run tests
echo "Running tests..."
./ERTbot_tests || { echo "Tests failed"; exit 1; }

# Optional: Cleanup build artifacts
# Uncomment the following line if you want to clean up after running the tests
# rm -rf "$BUILD_DIR"

echo "Build and tests completed successfully."
```
