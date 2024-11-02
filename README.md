# ERTbot
The ERTbot is a program which can automate certain tasks across several platforms. It is mainly used for syncing requirements from a google sheet into different forms on our wiki.js instance.

# Installing
## Requiremets
To set up the project, ensure you have the following dependencies installed:

### libcurl
For calling the Slack, Google Sheets, Wiki.js, and GitHub APIs.

**Linux**
```bash
sudo apt-get install libcurl4-openssl-dev
```
**macOS**
```bash
brew install curl
```

### cJSON
For handling cJSON formatted data.

**Linux**
```bash
sudo apt-get install libcjson-dev
```
**macOS**
```bash
brew install cjson
```

### Check
For unit testing.

**Linux**
```bash
sudo apt-get install check
sudo apt-get install pkg-config
```
**macOS**
```bash
brew install check
```


### CMake
For building the project.

**Linux**
```bash
sudo apt-get install cmake
```
**macOS**
```bash
brew install cmake
```

## Create script for running
1. Duplicate ```script_template.sh``` into a new file called ```build_and_run.sh``` which will be the script you will call to build and run the program.
**Linux**
```bash
cp script_template.sh build_and_run.sh
```

2. Add you API tokens
Replace all of the ```YOUR API TOKEN``` place holder text with your API tokens.

3. Make the script
```bash
chmod +x build_and_run.sh
```

4. Run the program with your new script

```bash
./build_and_run
```

# Running Tests
1. Uncomment Run test section in script

```
echo "Running tests..."
./ERTbot_tests || { echo "Tests failed"; exit 1; }
```
