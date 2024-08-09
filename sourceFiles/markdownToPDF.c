#include "../headerFiles/struct.h"
#include "../headerFiles/api.h"
#include "../headerFiles/config.h"
#include "../headerFiles/features.h"
#include "../headerFiles/githubAPI.h"
#include "../headerFiles/helperFunctions.h"
#include "../headerFiles/markdownToPDF.h"
#include "../headerFiles/slackAPI.h"
#include "../headerFiles/stringTools.h"
#include "../headerFiles/wikiAPI.h"
#include "../headerFiles/sheetAPI.h"


#define MAX_COMMAND_LENGTH 5000

/*Function which call pandoc on terminal. 
Inputs: inputMarkdownFile(full with markdown), outputLatexFile(empty)*/
void markdownToLatex(char* inputMarkdownFile, char* outputLatexFile) {
    char command[MAX_COMMAND_LENGTH]; // Assuming command won't exceed 500 characters

    // Construct the Pandoc command
    snprintf(command, sizeof(command), "pandoc -f markdown -t latex -o %s %s", outputLatexFile, inputMarkdownFile);

    // Execute the command using system
    int status = system(command);

    // Check if Pandoc command executed successfully
    if (status == 0) {
        printf("Markdown file converted to LaTeX successfully.\n");
    } else {
        fprintf(stderr, "Error converting Markdown to LaTeX.\n");
    }
}

//Function used to remove tabsets from the wiki.js markdown, replaces with appropriate headings
char* fixTabsets(char *str) {
    char* input = str;
    int count1 = 0;
    int count2 = 0;
    int count3 = 0;
    int found_tabset = 0;
    int j;
    int o=0;

    // Iterate through the input string
    for (int i = 0; input[i] != '\0'; i++) {
        
        // Check for '{.tabset}' substring
        if (strncmp(&input[i], "{.tabset}", 9) == 0) { //find tabset (i is at the { of tabset)
            found_tabset = 1;
            for (j = i; input[j-1] != 'n' && input[j-2] != '\\' ; j--) { //go backward and count the number of # until finding \n (j is at the character after \n)
                if (input[j] == '#') {
                    count1++;
                }
            }

            for(int k = j; input[k] != '\\' && input[j+1] != 'n'; k++){ //go forward until you find \n (count the number of characters on the line)
                count2++;
            }

            for(int l = 0; j+l+count2+1 < strlen(input); l++){ //reassign each character as of the start of the tabset line with what comes after the tabset line
                input[j+l] = input[j+l+count2];
            }

            for(int m = 0; o == 1 ; m++ ){ //go trough test to remove # when appropriate 

                if(input[j+m] != '\0'){
                    break;
                }

                if (input[j+m]=='#'){ // if I find a #
                    for(int n = 0; n<10; n++){ // count number of hashtags
                        if(count3 > count2){ //if there are more 
                            remove_char_at_index(input, j+m+n);
                            break;
                        }

                        else if(input[j+m+n]=='#'){
                            count3++;
                        }
                        else {
                            m = m + 10;
                            o = 0;
                            break;
                        }
                    }
                     
                }
            }


        }
        count1 = 0;
        count2 = 0;
        count3 = 0;
        j=0;
    }

    return input;
}

//Used to remove the image scales from the wiki markodwn's image links (such as =100x) 
char* removeImagesScale(char *input, char *sub) {
    char *found;
    char* str = input;
    size_t subLen = strlen(sub);

    // Find the first occurrence of the substring
    while ((found = strstr(str, sub)) != NULL) {
        char *endBracket = strchr(found, ')');
        char *endSquareBracket = strchr(found, ']');

        if (endBracket != NULL ) {
            // Remove characters after the substring until ')'
            if(endSquareBracket > endBracket){
            memmove(found + subLen, endBracket, strlen(endBracket) + 1);
            }
        } else {
            // If ')' not found, just remove characters after the substring
            found[subLen] = '\0';
        }
        // Move the search position after the replaced portion
        str = found + subLen;
    }

    return str;
}

//(Honnestly I'm not sure, it's something that's got to do with image formatting in Latex)
void endCenteringLatex(char *str) {
    char *jpeg = ".jpeg}";
    char *png = ".png}";
    char *PNG = ".PNG}";
    char *jpg = ".jpg}";
    char *gif = ".gif}";
    char *caption = "\n\\caption{";
    char *figureEnd = "\n\\end{figure}";

    char *pos = str;
    while ((pos = strstr(pos, jpeg)) != NULL) {
        // Check if the next characters after ".jpeg}" are not "\caption{"
        if (strncmp(pos + strlen(jpeg), caption, strlen(caption)) != 0) {
            // Append "\n\end{center}"
            memmove(pos + strlen(jpeg) + strlen(figureEnd), pos + strlen(jpeg), strlen(pos + strlen(jpeg)) + 1);
            memcpy(pos + strlen(jpeg), figureEnd, strlen(figureEnd));
            pos += strlen(jpeg) + strlen(figureEnd);
        } else {
            pos += strlen(jpeg);
        }
    }

    pos = str;
    while ((pos = strstr(pos, png)) != NULL) {
        // Check if the next characters after ".png}" are not "\caption{"
        if (strncmp(pos + strlen(png), caption, strlen(caption)) != 0) {
            // Append "\n\end{center}"
            memmove(pos + strlen(png) + strlen(figureEnd), pos + strlen(png), strlen(pos + strlen(png)) + 1);
            memcpy(pos + strlen(png), figureEnd, strlen(figureEnd));
            pos += strlen(png) + strlen(figureEnd);
        } else {
            pos += strlen(png);
        }
    }

    pos = str;
    while ((pos = strstr(pos, PNG)) != NULL) {
        // Check if the next characters after ".PNG}" are not "\caption{"
        if (strncmp(pos + strlen(PNG), caption, strlen(caption)) != 0) {
            // Append "\n\end{center}"
            memmove(pos + strlen(PNG) + strlen(figureEnd), pos + strlen(PNG), strlen(pos + strlen(PNG)) + 1);
            memcpy(pos + strlen(PNG), figureEnd, strlen(figureEnd));
            pos += strlen(PNG) + strlen(figureEnd);
        } else {
            pos += strlen(PNG);
        }
    }

    pos = str;
    while ((pos = strstr(pos, jpg)) != NULL) {
        // Check if the next characters after ".jpg}" are not "\caption{"
        if (strncmp(pos + strlen(jpg), caption, strlen(caption)) != 0) {
            // Append "\n\end{center}"
            memmove(pos + strlen(jpg) + strlen(figureEnd), pos + strlen(jpg), strlen(pos + strlen(jpg)) + 1);
            memcpy(pos + strlen(jpg), figureEnd, strlen(figureEnd));
            pos += strlen(jpg) + strlen(figureEnd);
        } else {
            pos += strlen(jpg);
        }
    }

    pos = str;
    while ((pos = strstr(pos, gif)) != NULL) {
        // Check if the next characters after ".gif}" are not "\caption{"
        if (strncmp(pos + strlen(gif), caption, strlen(caption)) != 0) {
            // Append "\n\end{center}"
            memmove(pos + strlen(gif) + strlen(figureEnd), pos + strlen(gif), strlen(pos + strlen(gif)) + 1);
            memcpy(pos + strlen(gif), figureEnd, strlen(figureEnd));
            pos += strlen(gif) + strlen(figureEnd);
        } else {
            pos += strlen(gif);
        }
    }
}

/*Finds occurences of {.is-success\\} in latex and stylizese it as
     a green list starting with "+" for each lines*/
void stylizeMainAdvantages(char *str) {
    char *begin_quote = "\\begin{quote}";
    char *danger = "\\{.is-success\\}";
    char *end_quote = "\\end{quote}";
    char * paragraph;
    
    // Initialize pointers to search positions
    char *begin_ptr = strstr(str, begin_quote);
    char *danger_ptr = NULL;
    char *end_ptr = NULL;
    char *end_of_end_ptr = NULL;
    
    // Keep searching until we reach the end of the string
    while (begin_ptr != NULL) {
        danger_ptr = strstr(begin_ptr, danger);
        end_ptr = strstr(begin_ptr, end_quote);
        if (danger_ptr != NULL && end_ptr != NULL && danger_ptr < end_ptr ) {

            end_of_end_ptr = end_ptr + 10;

            paragraph = extractParagraphWithPointerDelimiters(str, begin_ptr, end_of_end_ptr);

            paragraph = replaceWord(paragraph, "\\begin{quote}\n", "\\noindent Main advantages:\n\\begin{itemize}\n\\tightlist\n\\color{issuccess}\n\\item [\\bf+]");

            paragraph = replaceWord(paragraph, "\n\n", "\n\\item [\\bf+]");

            paragraph = replaceWord(paragraph, "\\{.is-success\\}", "");

            paragraph = replaceWord(paragraph, "\\end{quote", "\\end{itemize");

            str = replaceParagraph(str, paragraph, begin_ptr, end_of_end_ptr);

            begin_ptr = strstr(end_ptr, begin_quote);

        } else {
            // Move pointers to search for next occurrence
            begin_ptr = strstr(end_ptr, begin_quote);
            
        }
    }
}

/*Finds occurences of {.is-danger\\} in latex and stylizese it as
     a red list starting with "-" for each lines*/
void stylizeMainDisadvantages(char *str) {
    char *begin_quote = "\\begin{quote}";
    char *danger = "\\{.is-danger\\}";
    char *end_quote = "\\end{quote}";
    char * paragraph;
    
    // Initialize pointers to search positions
    char *begin_ptr = strstr(str, begin_quote);
    char *danger_ptr = NULL;
    char *end_ptr = NULL;
    char *end_of_end_ptr = NULL;
    
    // Keep searching until we reach the end of the string
    while (begin_ptr != NULL) {
        danger_ptr = strstr(begin_ptr, danger);
        end_ptr = strstr(begin_ptr, end_quote);
        if (danger_ptr != NULL && end_ptr != NULL && danger_ptr < end_ptr ) {

            end_of_end_ptr = end_ptr + 10;

            paragraph = extractParagraphWithPointerDelimiters(str, begin_ptr, end_of_end_ptr);

            paragraph = replaceWord(paragraph, "\\begin{quote}\n", "\\noindent Main disadvantages:\n\\begin{itemize}\n\\tightlist\n\\color{isdanger}\n\\item [\\bf-]");

            paragraph = replaceWord(paragraph, "\n\n", "\n\\item [\\bf-]");

            paragraph = replaceWord(paragraph, "\\{.is-danger\\}", "");

            paragraph = replaceWord(paragraph, "\\end{quote", "\\end{itemize");

            str = replaceParagraph(str, paragraph, begin_ptr, end_of_end_ptr);

            printf("found!");

            begin_ptr = strstr(end_ptr, begin_quote);

        } else {
            // Move pointers to search for next occurrence
            begin_ptr = strstr(end_ptr, begin_quote);
            
        }
    }
}

//Makes {.is-danger} lists in the markdown
char* makeDangerLists(char* input) {

    char* is_danger = "{.is-danger}";
    char* newline_pattern = "\n\n";
    char* tempParagraph;

    // Pointer to the beginning of the input
    char* current_ptr = input;
    
    // Loop until no more occurrences of "{.is-danger}" are found
    while (1) {
        // Find the next occurrence of "{.is-danger}"
        char* is_danger_ptr = strstr(current_ptr, is_danger);


        if (is_danger_ptr == NULL) {
            // No more occurrences of "{.is-danger}"
            break;
        }

        // Find the last occurrence of "/n/n" before "{.is-danger}"
        char* last_newline_before_is_danger = input;
        char* newline_ptr = strstr(input, newline_pattern);
        
        while (newline_ptr != NULL && newline_ptr < is_danger_ptr) {
            last_newline_before_is_danger = newline_ptr;
            newline_ptr = strstr(newline_ptr + 1, newline_pattern);
        }

        if (last_newline_before_is_danger == input) {
            printf("No '/n/n' found before '{.is-danger}'.\n");
        }

        tempParagraph = extractParagraphWithPointerDelimiters(input, last_newline_before_is_danger, is_danger_ptr);
        tempParagraph = replaceWord(tempParagraph, ">", ">\n");

        input = replaceParagraph(input, tempParagraph, last_newline_before_is_danger, is_danger_ptr);


        // Move the current pointer after the last "{.is-danger}" found
        current_ptr = is_danger_ptr + strlen(is_danger);
    }

    return input;
}

//Makes {.is-success} lists in the markdown
char* makeSuccessLists(char* input) {

    char* is_danger = "{.is-success}";
    char* newline_pattern = "\n\n";
    char* tempParagraph;

    // Pointer to the beginning of the input
    char* current_ptr = input;
    
    // Loop until no more occurrences of "{.is-danger}" are found
    while (1) {
        // Find the next occurrence of "{.is-danger}"
        char* is_danger_ptr = strstr(current_ptr, is_danger);


        if (is_danger_ptr == NULL) {
            // No more occurrences of "{.is-danger}"
            break;
        }

        // Find the last occurrence of "/n/n" before "{.is-danger}"
        char* last_newline_before_is_danger = input;
        char* newline_ptr = strstr(input, newline_pattern);
        
        while (newline_ptr != NULL && newline_ptr < is_danger_ptr) {
            last_newline_before_is_danger = newline_ptr;
            newline_ptr = strstr(newline_ptr + 1, newline_pattern);
        }

        if (last_newline_before_is_danger == input) {
        }

        tempParagraph = extractParagraphWithPointerDelimiters(input, last_newline_before_is_danger, is_danger_ptr);
        tempParagraph = replaceWord(tempParagraph, ">", ">\n");

        input = replaceParagraph(input, tempParagraph, last_newline_before_is_danger, is_danger_ptr);


        // Move the current pointer after the last "{.is-danger}" found
        current_ptr = is_danger_ptr + strlen(is_danger);
    }

    return input;
}

//Removes the 2024_P_SS_ and _DJF _SDD _DDF from the docID to make the title of the document
char* makeDocTitle(char *input) {
    // Remove the first 10 characters
    size_t len = strlen(input);
    if (len <= 10) {
        return NULL; // String is too short, cannot process
    }

    char *predetermined[] = {"SDD", "DJF", "DDF"};

    char *lastThree = input + len - 3;
    int found = 0;
    for (size_t i = 0; i < sizeof(predetermined) / sizeof(predetermined[0]); ++i) {
        if (strcmp(lastThree, predetermined[i]) == 0) {
            found = 1;
            break;
        }
    }

    char *result = (char*)malloc(len - 9 + 1); // Allocating memory for result
    if (result == NULL) {
        return NULL; // Memory allocation failed
    }

    if (found) {
        strncpy(result, input + 10, len - 14); // Copying characters without first 10 and last 4
        result[len - 14] = '\0'; // Null-terminating the string
    } else {
        strncpy(result, input + 10, len - 10); // Copying characters without first 10
        result[len - 10] = '\0'; // Null-terminating the string
    }

    // Replace "_" and "-" with " "

    result = replaceWord(result, "-", "$\\hspace{0.3cm}$");
    result = replaceWord(result, "_", "$\\hspace{0.3cm}$");

    return result;
}

/*replaces "\\includegraphics{/" with "\\begin{center}\n\\includegraphics[width=0.5\\textwidth]{"
    and calls stylizeMainAdvantages and stylizeMainDisadvantages*/
void applyDefaultImageScaleAndPosition(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Unable to open file %s\n", filename);
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // Allocate memory for file contents
    char *buffer = (char *)malloc(fileSize + 1000);
    if (buffer == NULL) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return;
    }

    // Read file contents into buffer
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    buffer[bytesRead] = '\0';  // Null-terminate the buffer
    fclose(file);

    buffer = replaceWord(buffer, "\\centering", "\n");

    buffer = replaceWord(buffer, "\\includegraphics{/", "\\begin{figure}[H]\n\\centering\n\\includegraphics[width=0.75\\textwidth]{");
    buffer = replaceWord(buffer, "\\begin{figure}\n", "");
    endCenteringLatex(buffer);
    
    

    stylizeMainAdvantages(buffer);
    stylizeMainDisadvantages(buffer);


    bytesRead = strlen(buffer);


    // Write modified buffer back to the file
    FILE *newFile = fopen(filename, "w");
    if (newFile == NULL) {
        printf("Unable to open file %s for writing\n", filename);
        free(buffer);
        return;
    }

    size_t bytesWritten = fwrite(buffer, 1, bytesRead, newFile);
    fclose(newFile);

    if (bytesWritten != bytesRead) {
        printf("Error writing to file.\n");
    } else {
        printf("\n\nFile %s successfully modified and saved.\n\n", filename);
    }

    free(buffer);
}

//Populates the Title Page and Header with the appropriate information
void populateTitleAndHeader( char *filename, char *docPath) {
    char *docID;
    char *docType;
    char *docTitle;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Unable to open file %s\n", filename);
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // Allocate memory for file contents
    char *buffer = (char *)malloc(fileSize + 2000);
    if (buffer == NULL) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return;
    }

    // Read file contents into buffer
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    buffer[bytesRead] = '\0';  // Null-terminate the buffer
    fclose(file);

    docID = getDocId(docPath);
    docType = getDocType(docID);
    docTitle = makeDocTitle(docID);
    docID = replaceWord(docID, "_", "\\_");
    buffer = replaceWord(buffer, "DEFAULTDOCID", docID);
    buffer = replaceWord(buffer, "DEFAULTTITLE", docTitle);

    if(!strcmp("DJF",docType)){
        buffer = replaceWord(buffer, "DEFAULTDOCTYPE", "\\bf{ \\fontsize{28}{2} \\selectfont \\an{DESIGN}}\n    \\newline\n    \\newline\n    \\bf{ \\fontsize{28}{2} \\selectfont \\an{JUSTIFICATION}}\n    \\newline\n    \\newline\n    \\bf{ \\fontsize{28}{2} \\selectfont \\an{FILE}}\n");
    }
    else if(!strcmp("SDD",docType)){
        buffer = replaceWord(buffer, "DEFAULTDOCTYPE", "\\bf{ \\fontsize{28}{2} \\selectfont \\an{SOFTWARE}}\n    \\newline\n    \\newline\n    \\bf{ \\fontsize{28}{2} \\selectfont \\an{DESIGN}}\n    \\newline\n    \\newline\n    \\bf{ \\fontsize{28}{2} \\selectfont \\an{DOCUMENT}}\n");
    }
    else{
        buffer = replaceWord(buffer, "DEFAULTDOCTYPE", "");

    }

    appendStrings(buffer, "\\end{document}");

    bytesRead = strlen(buffer);


    // Write modified buffer back to the file
    FILE *newFile = fopen(filename, "w");
    if (newFile == NULL) {
        printf("Unable to open file %s for writing\n", filename);
        free(buffer);
        return;
    }

    size_t bytesWritten = fwrite(buffer, 1, bytesRead, newFile);
    fclose(newFile);

    if (bytesWritten != bytesRead) {
        printf("Error writing to file.\n");
    } else {
        printf("\n\nFile %s successfully modified and saved.\n\n", filename);
    }

    free(buffer);
}

/* Removes Drawio diagrams from markdown and replaces with a link to an image*/
char* removeDrawio(char *input) {
    char *begin_quote = "```diagram\nPHN2ZyB4";
    char *end_quote = "=\n```";
    char *str = input;

    // Initialize pointers to search positions
    char *begin_ptr = strstr(str, begin_quote);
    char *end_ptr = NULL;
    
    // Keep searching until we reach the end of the string or find nothing
    while (begin_ptr != NULL) {
        end_ptr = strstr(begin_ptr, end_quote);
        if (end_ptr != NULL) {
            // Calculate end_of_end_ptr if end_quote is found
            char *end_of_end_ptr = end_ptr + strlen(end_quote);

            // Now you can manipulate the string or pointers as needed
            str = replaceParagraph(str, "\n![](/drawio.png)\n", begin_ptr, end_of_end_ptr);

            // Move begin_ptr to the next occurrence
            begin_ptr = strstr(end_ptr, begin_quote);
        } else {
            // If end_quote is not found, break the loop
            break;
        }
    }

    return str;
}

/*Removes image caption form markdown if the caption contains a "." */
char* removeImageCaptions(char *input) {
    char *begin_quote = "![";
    char *danger = ".";
    char *end_quote = "](";
    char *paragraph;
    char *str = input;

    // Initialize pointers to search positions
    char *begin_ptr = strstr(str, begin_quote);
    char *danger_ptr = NULL;
    char *end_ptr = NULL;
    char *end_of_end_ptr = NULL;
    
    // Keep searching until we reach the end of the string
    while (begin_ptr != NULL) {
        danger_ptr = strstr(begin_ptr, danger);
        end_ptr = strstr(begin_ptr, end_quote);
        if (danger_ptr != NULL && end_ptr != NULL && danger_ptr < end_ptr ) {

            end_of_end_ptr = end_ptr + strlen(end_quote);

            paragraph = "![](/";

            str = replaceParagraph(str, paragraph, begin_ptr, end_of_end_ptr);

            begin_ptr = strstr(end_ptr, begin_quote);

        } else {
            // Move pointers to search for next occurrence
            begin_ptr = strstr(end_ptr, begin_quote);
            
        }
    }

    return str;
}

//Compiles PlantUML code (a string) into an image saved at iamgePath
void compilePlantUML( char *plantUMLCode,  char *imagePath) {
    // Extract the file name from the provided imagePath
     char *lastSlash = strrchr(imagePath, '/');
     char *fileName = (lastSlash != NULL) ? lastSlash + 1 : imagePath;

    // Write the PlantUML code to a temporary file with extracted file name
    char tempUML[1000];
    snprintf(tempUML, sizeof(tempUML), "%s.uml", fileName);
    FILE *file = fopen(tempUML, "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }
    fwrite(plantUMLCode, 1, strlen(plantUMLCode), file);
    fclose(file);

    // Compile the PlantUML file to PNG using plantuml command
    char command[5024]; // Assuming command length won't exceed 1000 characters
    snprintf(command, sizeof(command), "plantuml -tpng -o \"%s\" %s", imagePath, tempUML);

    int status = system(command);
    if (status != 0) {
        printf("Error compiling PlantUML to PNG.\n");
        return;
    }

    // Clean up temporary .uml file
    remove(tempUML);

    printf("PlantUML compiled to %s\n", imagePath);
}

/*Extracts PlantUML diagrams and makes them images*/
char* extractPlantUML(char *input, char * docID) {
    char *str = input;
    char *begin_quote = "\n```plantuml\n";
    char *danger = "@end";
    char *end_quote = "```\n";
    char *diagramCode;
    int diagramCount = 0;
    
    // Initialize pointers to search positions
    char *begin_ptr = strstr(str, begin_quote);
    char *danger_ptr = NULL;
    char *end_ptr = NULL;
    char *end_of_end_ptr = NULL;
    
    // Keep searching until we reach the end of the string
    while (begin_ptr != NULL) {
        danger_ptr = strstr(begin_ptr, danger);
        end_ptr = strstr(begin_ptr, end_quote);
        if (danger_ptr != NULL && end_ptr != NULL && danger_ptr < end_ptr ) {

            diagramCount++; 

            end_of_end_ptr = end_ptr + strlen(end_quote);

            diagramCode = extractParagraphWithPointerDelimiters(str, begin_ptr, end_of_end_ptr);


            char diagramName[50];
            snprintf(diagramName, sizeof(diagramName), "PlantUMLDiagram%d", diagramCount);

            char pathPlantUML[300];
            snprintf(pathPlantUML, sizeof(pathPlantUML), "%s%s%s", LOCAL_FILE_PATH, docID, diagramName);

            compilePlantUML(diagramCode, pathPlantUML);

            snprintf(diagramName, sizeof(diagramName), "![](PlantUMLDiagram%d.png)\n\n", diagramCount);

            str = replaceParagraph(str, diagramName, begin_ptr, end_of_end_ptr);

            begin_ptr = strstr(end_ptr, begin_quote);

        } else {
            // Move pointers to search for next occurrence
            begin_ptr = strstr(end_ptr, begin_quote);
            
        }
    }

    return str;
}

//Goes through link list which contains all of the links to the images and calls fetch image for each image
void getImages(pageList** head, char* pagePath) {
    pageList* current = *head;
    int error;
    while (current != NULL) {
        error = fetchImage(current->path, pagePath);
        if(!error){
            printf("an image did not download properly");
            fetchImage(current->path, pagePath);
        }
        current = current->next;
    }
}

//download the images linked in a text from github
void downloadImages(pageList** head){
    char *docID;
    pageList* imageLinks;
    imageLinks = findImageLinks((*head)->content, &imageLinks);
    filterLinks(&imageLinks);
    docID = getDocId((*head)->path);
    getImages(&imageLinks, docID);
    freePageList(&imageLinks);
}

//text preprocessing before the markdown-latex conversion
char* preProcessing(char* pageContent, char *docID){

    char *tempPageContent = pageContent;

    tempPageContent = fixTabsets(tempPageContent);
    tempPageContent = removeImagesScale(tempPageContent, ".png");
    tempPageContent = removeImagesScale(tempPageContent, ".jpeg");
    tempPageContent = removeImagesScale(tempPageContent, ".jpg");
    tempPageContent = removeImagesScale(tempPageContent, ".gif");
    tempPageContent = removeImagesScale(tempPageContent, ".PNG");
    tempPageContent = reformatNewLineCharacter(tempPageContent);
    tempPageContent = replaceWord(tempPageContent, "{.links-list}", "\n\n");
    tempPageContent = replaceWord(tempPageContent, "{.grid-list}", "\n\n");
    tempPageContent = replaceWord(tempPageContent, "{.align-center}", "");
    tempPageContent = replaceWord(tempPageContent, "{.center}", "");
    tempPageContent = replaceWord(tempPageContent, "{.is-info}", "");
    tempPageContent = replaceWord(tempPageContent, "{.is-warning}", "");
    tempPageContent = replaceWord(tempPageContent, "\\t", "");
    tempPageContent = makeDangerLists(tempPageContent);
    tempPageContent = makeSuccessLists(tempPageContent);
    tempPageContent = removeDrawio(tempPageContent);
    tempPageContent = removeImageCaptions(tempPageContent);
    tempPageContent = extractPlantUML(tempPageContent, docID);

    return tempPageContent;
}

void getZip(command cmd){
    pageList* tempPage;
    char *docID;
    int error;
    tempPage->id = cmd.argument_1;


    tempPage = getPage(&tempPage);
    
    downloadImages(&tempPage);
    
    docID = getDocId(tempPage->path);
    char pathTXT[525];
    snprintf(pathTXT, sizeof(LOCAL_FILE_PATH)+200, "%s%s/%s.txt", LOCAL_FILE_PATH, docID, docID);
    char pathTEX[525];
    snprintf(pathTEX, sizeof(LOCAL_FILE_PATH)+200, "%s%s/%s.tex", LOCAL_FILE_PATH, docID, docID);
    char pathFolder[525];
    snprintf(pathFolder, sizeof(LOCAL_FILE_PATH)+200, "%s%s", LOCAL_FILE_PATH, docID);
    char pathZip[525];
    snprintf(pathZip, sizeof(LOCAL_FILE_PATH)+200, "%s%s.zip", LOCAL_FILE_PATH, docID);
    FILE *contentFile;
    // Open file for writing
    contentFile = fopen(pathTXT, "w");
    // Check if file opened successfully
    if (contentFile == NULL) {
        printf("Error opening file!\n\n");
        exit(1);
    }

    tempPage->content = preProcessing(tempPage->content, docID);
    

    fprintf(contentFile, "%s\n", tempPage->content);
    fclose(contentFile);
    markdownToLatex(pathTXT, pathTEX);

    applyDefaultImageScaleAndPosition(pathTEX);
    prepend_file("/Users/ryan/Desktop/Tidy/EPFL/Mirror/EPFL/ERT/ERTWikiToolbox-Test-Branch/test/2024_C_SE_DOCUMENTATION_TEMPLATE.txt", pathTEX);
    printf("template added");
    populateTitleAndHeader(pathTEX, tempPage->path);
    error = zipFolder(pathFolder);

     char *message = "Here is your file:"; // Message you want to send

        if (sendMessageToSlack(message) == 0) {
            printf("Message sent successfully to Slack!\n");
        } else {
            printf("Message sending failed.\n");
        }


        /*if (uploadFileToSlack(pathZip) == 0) {
            printf("File uploaded successfully to Slack!\n");
        } else {
            printf("File upload failed.\n");
        }*/
}