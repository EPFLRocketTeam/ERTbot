/**
 * @file markdownToPDF.c
 * @author Ryan Svoboda (ryan.svoboda@epfl.ch)
 * @brief File which contains all of the helper functions which are only used during the conversion of wiki pages to latex files
 * 
 * 
 * @warning The code is extremely messy and needs to be refactored
 * @todo refactor all of the code, redocument, add functionality to scale image automatically, add functionality to compile the
 *       latex document locally, remove folder after sending it to slack
 */
#include "../include/struct.h"
#include "../include/api.h"
#include "../include/config.h"
#include "../include/features.h"
#include "../include/githubAPI.h"
#include "../include/helperFunctions.h"
#include "../include/markdownToPDF.h"
#include "../include/slackAPI.h"
#include "../include/stringTools.h"
#include "../include/wikiAPI.h"
#include "../include/sheetAPI.h"
#include "../include/command.h"
#include "../include/log.h"

#define MAX_COMMAND_LENGTH 5000

/**
 * @brief Converts a Markdown file to a LaTeX file using Pandoc.
 * 
 * This function constructs and executes a system command to use Pandoc for converting a Markdown file to LaTeX.
 * 
 * @param inputMarkdownFile The path to the Markdown file to be converted.
 * @param outputLatexFile The path where the resulting LaTeX file will be saved.
 * 
 * @details
 * - Constructs a command string for Pandoc to convert the input Markdown file to the specified LaTeX file.
 * - Executes the command using the `system` function.
 * - Outputs a success message if the conversion is successful, otherwise an error message.
 */
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

/**
 * @brief Fixes tabset headers in a string by adjusting the level of Markdown headers before and after a tabset.
 * 
 * This function processes a string containing Markdown content to handle tabsets by:
 * - Identifying occurrences of the `{.tabset}` tag.
 * - Adjusting the level of headers before and after the tabset by recalculating and removing excess hash symbols (`#`).
 * 
 * @param str The input string containing Markdown content with potential tabset headers.
 * 
 * @return char* A modified version of the input string with fixed tabsets.
 * 
 * @details
 * - The function searches for `{.tabset}` in the string.
 * - For each occurrence, it counts the number of headers (hash symbols) preceding and following the tabset.
 * - Adjustments are made to realign the headers as necessary by shifting characters and removing extra hashes.
 */
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

/**
 * @brief Removes image scaling parameters from a string.
 * 
 * This function searches for occurrences of a specified substring in the input string and removes the part of the string 
 * following each occurrence of the substring until it reaches a closing parenthesis `)` or square bracket `]`. 
 * If no closing character is found, it truncates the string at the end of the substring.
 * 
 * @param input The input string containing image references or other content.
 * @param sub The substring to search for and remove scaling parameters following it.
 * 
 * @return char* The modified version of the input string with specified image scaling parameters removed.
 * 
 * @details
 * - The function uses `strstr` to locate occurrences of the specified substring.
 * - It checks for closing parenthesis or square bracket and adjusts the string accordingly.
 * - The function handles cases where these characters might not be present by truncating the string after the substring.
 */
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

/**
 * @brief Modifies LaTeX strings to close figure environments properly.
 * 
 * This function searches for image file extensions (e.g., `.jpeg}`, `.png}`, `.jpg}`, `.gif}`) in the given LaTeX string 
 * and checks if the subsequent content is not a caption. If the image file extension is found and is not followed by a caption, 
 * it appends `\n\end{figure}` to close the figure environment properly.
 * 
 * @param str The LaTeX string to be modified.
 * 
 * @details
 * - The function processes multiple image file extensions including `.jpeg}`, `.png}`, `.PNG}`, `.jpg}`, and `.gif}`.
 * - For each image type, it appends `\n\end{figure}` to properly close any open figure environments.
 * - The function uses `strstr` to find occurrences of the image extensions and `memmove` and `memcpy` to insert the closing tags.
 */
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

/**
 * @brief Stylizes LaTeX text by modifying quote environments and special markers.
 * 
 * This function searches for occurrences of the `\begin{quote}` and `\end{quote}` tags in the input LaTeX string. Within 
 * these quote environments, it looks for the `\{.is-success\}` marker. If found, it replaces the content as follows:
 * 
 * - Replaces `\begin{quote}\n` with `\noindent Main advantages:\n\begin{itemize}\n\tightlist\n\color{issuccess}\n\item [\bf+]`
 * - Replaces double newlines `\n\n` with `\n\item [\bf+]`
 * - Removes the `\{.is-success\}` marker
 * - Replaces `\end{quote}` with `\end{itemize}`
 * 
 * The function iterates through the string and applies these changes until all relevant quote environments are processed.
 * 
 * @param str The LaTeX string to be stylized.
 * 
 * @details
 * - The function uses `strstr` to locate the beginning and end of quote environments and the `\{.is-success\}` marker.
 * - The function uses `extractParagraphWithPointerDelimiters`, `replaceWord`, and `replaceParagraph` to perform the necessary text replacements.
 */
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

/**
 * @brief Stylizes LaTeX text by modifying quote environments and special markers for main disadvantages.
 * 
 * This function searches for occurrences of the `\begin{quote}` and `\end{quote}` tags in the input LaTeX string. Within 
 * these quote environments, it looks for the `\{.is-danger\}` marker. If found, it replaces the content as follows:
 * 
 * - Replaces `\begin{quote}\n` with `\noindent Main disadvantages:\n\begin{itemize}\n\tightlist\n\color{isdanger}\n\item [\bf-]`
 * - Replaces double newlines `\n\n` with `\n\item [\bf-]`
 * - Removes the `\{.is-danger\}` marker
 * - Replaces `\end{quote}` with `\end{itemize}`
 * 
 * The function iterates through the string and applies these changes until all relevant quote environments are processed.
 * 
 * @param str The LaTeX string to be stylized.
 * 
 * @details
 * - The function uses `strstr` to locate the beginning and end of quote environments and the `\{.is-danger\}` marker.
 * - The function uses `extractParagraphWithPointerDelimiters`, `replaceWord`, and `replaceParagraph` to perform the necessary text replacements.
 */
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

/**
 * @brief Transforms LaTeX text by inserting line breaks before markers indicating danger lists.
 * 
 * This function processes an input LaTeX string to find occurrences of the `{.is-danger}` marker. For each occurrence, 
 * it inserts a newline before the marker, ensuring that lists marked as "danger" are formatted correctly.
 * 
 * The function performs the following steps:
 * - Locates occurrences of `{.is-danger}` in the input string.
 * - Identifies the last occurrence of `\n\n` before each `{.is-danger}` marker.
 * - Inserts a newline before the `{.is-danger}` marker by replacing `>` with `>\n`.
 * - Updates the input string with the modified paragraphs.
 * 
 * @param input The LaTeX string to be transformed.
 * @return A pointer to the transformed LaTeX string.
 * 
 * @details
 * - The function uses `strstr` to find markers and newline patterns.
 * - `extractParagraphWithPointerDelimiters` is used to isolate sections of text.
 * - `replaceWord` and `replaceParagraph` are used to perform text replacements.
 */
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

/**
 * @brief Transforms LaTeX text by inserting line breaks before markers indicating success lists.
 * 
 * This function processes an input LaTeX string to find occurrences of the `{.is-success}` marker. For each occurrence, 
 * it inserts a newline before the marker to ensure correct formatting for success lists.
 * 
 * The function performs the following steps:
 * - Locates occurrences of `{.is-success}` in the input string.
 * - Identifies the last occurrence of `\n\n` before each `{.is-success}` marker.
 * - Inserts a newline before the `{.is-success}` marker by replacing `>` with `>\n`.
 * - Updates the input string with the modified paragraphs.
 * 
 * @param input The LaTeX string to be transformed.
 * @return A pointer to the transformed LaTeX string.
 * 
 * @details
 * - The function uses `strstr` to find markers and newline patterns.
 * - `extractParagraphWithPointerDelimiters` is used to isolate sections of text.
 * - `replaceWord` and `replaceParagraph` are used to perform text replacements.
 */
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

/**
 * @brief Processes a string to generate a document title by removing specific characters 2024_P_SS_ and _DJF _SDD _DDF
 *        from the docID to make the title of the document.
 * 
 * This function modifies an input string to create a document title by removing the first 10 characters and optionally 
 * the last 3 characters based on a predefined list. It also replaces certain characters with LaTeX formatting.
 * 
 * The function performs the following steps:
 * - Removes the first 10 characters from the input string.
 * - Checks if the last 3 characters of the remaining string match any in a predefined list (`"SDD"`, `"DJF"`, `"DDF"`).
 * - If a match is found, also removes the last 4 characters of the string.
 * - Replaces occurrences of `"-"` and `"_"` with LaTeX formatting for spacing.
 * 
 * @param input The input string from which the document title will be generated.
 * 
 * @return A pointer to the newly allocated string containing the processed document title. If the input string is too 
 * short or if memory allocation fails, the function returns `NULL`.
 * 
 * @details
 * The function assumes that the input string is valid and has at least 11 characters. It uses `strncpy` to copy parts of 
 * the string and `replaceWord` to perform the character replacements. Memory for the result is dynamically allocated, 
 * and the caller is responsible for freeing this memory. The function handles cases where the input string is too short 
 * or where memory allocation fails gracefully.
 */
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

/**
 * @brief Applies default image scaling and positioning to a LaTeX file.
 * 
 * This function reads the contents of a LaTeX file, modifies image inclusion commands and centering, and updates 
 * the file with the changes. It also applies specific stylizations to sections of the document.
 * 
 * The function performs the following steps:
 * - Opens the specified file for reading and determines its size.
 * - Allocates memory to read the file contents into a buffer.
 * - Replaces LaTeX commands for centering and image inclusion with default settings for image scaling and positioning.
 * - Applies additional stylizations for advantages and disadvantages sections.
 * - Writes the modified contents back to the file.
 * 
 * @param filename The name of the LaTeX file to be modified.
 *  
 * @details
 * The function uses `fopen`, `fread`, `fwrite`, and `fclose` to handle file operations and `malloc` to allocate memory for 
 * file contents. It performs text replacements and stylistic changes using helper functions like `replaceWord`, `endCenteringLatex`, 
 * `stylizeMainAdvantages`, and `stylizeMainDisadvantages`. The function assumes the file is a valid LaTeX file and that 
 * sufficient memory is available. Error handling is included for file operations and memory allocation.
 */
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

/**
 * @brief Populates the title and header of a document based on its type and ID.
 * 
 * This function modifies a LaTeX file to include the appropriate document ID, title, and document type header based 
 * on information retrieved from a document path. The function reads the file, performs replacements based on the document 
 * type, and writes the modified content back to the file.
 * 
 * The function performs the following steps:
 * - Opens the specified file and determines its size.
 * - Allocates memory to read the file contents into a buffer.
 * - Retrieves the document ID, document type, and title.
 * - Replaces placeholders for document ID, title, and document type header with actual values.
 * - Appends a closing `\end{document}` command to the buffer.
 * - Writes the modified contents back to the file.
 * 
 * @param[in] filename The name of the LaTeX file to be modified.
 * @param[in] docPath The path used to retrieve the document ID and type.
 *  
 * @details
 * The function uses `fopen`, `fread`, `fwrite`, and `fclose` for file operations and `malloc` for memory allocation. It assumes 
 * the existence of helper functions `getDocId`, `getDocType`, and `makeDocTitle` for retrieving document details, and 
 * `replaceWord` for performing text replacements. Error handling is included for file operations and memory allocation.
 */
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

/**
 * @brief Removes Draw.io diagrams from a given string.
 * 
 * This function searches for and removes embedded Draw.io diagrams from the input string. Draw.io diagrams are identified 
 * by specific markers and are removed, along with their placeholders. The function modifies the string in place and 
 * returns the modified string.
 * 
 * The function performs the following steps:
 * - Searches for the start marker of a Draw.io diagram in the input string.
 * - If the start marker is found, it searches for the corresponding end marker.
 * - If both markers are found, it removes the section between them, including the markers, and replaces it with a placeholder.
 * - Continues searching for additional diagrams in the string.
 * 
 * @param input The input string containing potential Draw.io diagrams.
 * 
 * @return A modified string with Draw.io diagrams removed and placeholders inserted.
 * 
 * @details
 * The function uses `strstr` to find markers in the input string and `replaceParagraph` to remove and replace sections of the 
 * string. It assumes the existence of `replaceParagraph` for modifying parts of the string and processes the string in-place. 
 * Error handling for `replaceParagraph` is not included.
 */
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

/**
 * @brief Removes image captions from a given string if the image caption contains a full stop character".".
 * 
 * This function searches for and removes image captions from the input string. Image captions are identified by specific 
 * markers and are replaced with a placeholder. The function modifies the string in place and returns the modified string.
 * 
 * The function performs the following steps:
 * - Searches for the start marker of an image caption.
 * - Identifies the section of the string that includes the caption and image URL.
 * - Replaces this section with a placeholder that only includes the image URL.
 * - Continues searching for additional image captions in the string.
 * 
 * @param input The input string containing potential image captions.
 * 
 * @return char* A modified string with image captions removed and placeholders inserted.
 * 
 * @details
 * The function uses `strstr` to locate the start and end markers of image captions in the input string and `replaceParagraph` 
 * to replace these sections. It processes the string in-place, assuming the existence of `replaceParagraph` for modifying 
 * parts of the string. Error handling for `replaceParagraph` is not included.
 */
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

/**
 * @brief Compiles PlantUML code into a PNG image file.
 * 
 * This function writes the provided PlantUML code to a temporary file, compiles it into a PNG image using the `plantuml` 
 * command-line tool, and then cleans up the temporary file.
 * 
 * The function performs the following steps:
 * - Extracts the file name from the provided image path.
 * - Writes the PlantUML code to a temporary file with a `.uml` extension.
 * - Uses the `plantuml` command to compile the `.uml` file into a PNG image and saves it to the specified image path.
 * - Deletes the temporary `.uml` file after compilation.
 * 
 * @param plantUMLCode The PlantUML code to be compiled.
 * @param imagePath The path where the compiled PNG image should be saved.
 *   
 * @details
 * - The function assumes that the `plantuml` command is available and properly configured in the system's PATH.
 * - The length of the command string is assumed to be within safe limits.
 * - Error handling is provided for file operations and the `plantuml` command execution.
 */
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

/**
 * @brief Extracts PlantUML diagrams from a given text, compiles them into PNG images, and replaces the diagrams in the text with image references.
 * 
 * This function searches for PlantUML diagrams embedded in a text, compiles each diagram into a PNG image using the `compilePlantUML` function, 
 * and replaces the PlantUML diagram blocks in the text with references to the generated PNG images.
 * 
 * The function performs the following steps:
 * - Searches for PlantUML code blocks marked with ```````plantuml`` and ```````end``.
 * - Extracts each PlantUML diagram code, compiles it into a PNG image, and saves it using the `compilePlantUML` function.
 * - Replaces the PlantUML code block in the text with a Markdown image reference to the compiled PNG file.
 * 
 * @param input The text containing embedded PlantUML diagrams.
 * @param docID The document identifier used to name the PNG files.
 * 
 * @return A pointer to the modified text with PlantUML diagrams replaced by image references.
 * 
 * @details
 * - The function generates unique names for each PlantUML diagram and uses these names to save the PNG files.
 * - The generated PNG file names are based on a combination of the document ID and a sequential number.
 * - The function assumes that the `LOCAL_FILE_PATH` macro is defined and points to the directory where PNG files should be saved.
 * - Error handling for the `compilePlantUML` function and file operations is assumed to be handled elsewhere.
 */
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

/**
 * @brief Downloads images specified in a linked list of pages.
 * 
 * This function iterates through a linked list of pages, and for each page, it attempts to download the images specified by the page's path 
 * to a given directory. If an image fails to download, the function will attempt to download it again.
 * 
 * @param head A pointer to the head of the linked list of `pageList` nodes, where each node contains a path to an image.
 * @param pagePath The path to the directory where images should be downloaded.
 * 
 * @details
 * - The function uses the `fetchImage` function to perform the image download.
 * - If an error occurs during the image download (indicated by a non-zero return value from `fetchImage`), the function will print an error message 
 *   and attempt to download the image again.
 * - The function processes each node in the linked list, downloading images as specified by the node's `path`.
 * - Error handling for specific issues in `fetchImage` and memory management for the linked list are assumed to be handled elsewhere.
 */
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

/**
 * @brief Downloads images from the content of a page.
 * 
 * This function processes the content of a page to find image links, filters those links, and then downloads the images to a directory 
 * specified by the document ID extracted from the page's path.
 * 
 * @param head A pointer to the head of the linked list of `pageList` nodes, where each node contains the content and path of a page.
 * 
 * @details
 * - `findImageLinks` is used to extract image links from the content of the page pointed to by `(*head)`.
 * - `filterLinks` is used to filter the extracted image links.
 * - `getDocId` retrieves the document ID from the path of the page.
 * - `getImages` downloads the images based on the filtered links and the document ID.
 * - `freePageList` cleans up and deallocates the memory used by the list of image links.
 */
void downloadImages(pageList** head){
    char *docID;
    pageList* imageLinks;
    imageLinks = findImageLinks((*head)->content, &imageLinks);
    filterLinks(&imageLinks);
    docID = getDocId((*head)->path);
    getImages(&imageLinks, docID);
    freePageList(&imageLinks);
}

/**
 * @brief Processes and transforms the content of a page for further handling.
 * 
 * This function performs various preprocessing steps on the page content, including removing or modifying image scales, adjusting newline
 * characters, replacing specific patterns with others, and handling PlantUML diagrams. The function aims to clean and format the content
 * according to predefined rules.
 * 
 * @param pageContent The original content of the page to be processed.
 * @param docID The document ID used for identifying and processing specific elements within the content.
 * 
 * @return A modified version of the page content with applied transformations.
 * 
 * @details
 * - `fixTabsets` adjusts tab settings in the content.
 * - `removeImagesScale` removes or adjusts image scales for various image formats (e.g., PNG, JPEG, JPG, GIF).
 * - `reformatNewLineCharacter` reformats newline characters.
 * - `replaceWord` replaces specific placeholders or patterns in the content with predefined text.
 * - `makeDangerLists` and `makeSuccessLists` handle list formatting for danger and success items.
 * - `removeDrawio` removes or adjusts Draw.io diagram entries.
 * - `removeImageCaptions` removes captions from image entries.
 * - `extractPlantUML` extracts and processes PlantUML diagrams from the content.
 */
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