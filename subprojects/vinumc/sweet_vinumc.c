#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sweet_vinumc.h"

// External lexer input file pointer
extern FILE *yyin;

// Output file to write the dry result
FILE *output_file = NULL;

/**
 * Error reporting function for the Bison parser.
 *
 * Accepts a printf-style format and optional arguments.
 *
 * @param s Format string
 */
void yyerror(char *s, ...) {
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "[ERROR]: ");
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");

    va_end(ap);
}

/**
 * Creates a new string with the same base as the input filename,
 * but with the suffix "_dry.vin".
 *
 * For example: input "foo.vin" → output "foo_dry.vin"
 *
 * @param filename Original input filename
 * @return Newly allocated output filename with "_dry.vin" suffix
 */
char* append_dry_suffix(const char* filename) {
    const char* dot = strrchr(filename, '.');
    size_t base_len = dot ? (size_t)(dot - filename) : strlen(filename);
    const char* suffix = "_dry.vin";
    size_t result_len = base_len + strlen(suffix) + 1;

    char* result = malloc(result_len);
    if (!result) {
        fprintf(stderr, "Memory allocation error.\n");
        exit(1);
    }

    strncpy(result, filename, base_len);
    result[base_len] = '\0';
    strcat(result, suffix);
    return result;
}

/**
 * Entry point of the transpiler.
 *
 * Opens the sweet-style source file, parses it using Flex/Bison,
 * and writes the dry-style output to a new file.
 *
 * @param argc Number of command-line arguments
 * @param argv Array of command-line arguments
 * @return Exit code (0 on success, 1 on error)
 */
int main(int argc, char *argv[]) {
    // Require input file argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file sweet.vin>\n", argv[0]);
        return 1;
    }

    const char* input_filename = argv[1];
    FILE* input = fopen(input_filename, "r");
    if (!input) {
        perror("Error opening input file");
        return 1;
    }

    // Set Flex to read from input file
    yyin = input;

    // Generate output filename
    char* output_filename = append_dry_suffix(input_filename);
    output_file = fopen(output_filename, "w");
    if (!output_file) {
        perror("Error creating output file");
        fclose(input);
        free(output_filename);
        return 1;
    }

    // Parse input file
    int status = yyparse();

    // Cleanup
    fclose(input);
    fclose(output_file);
    free(output_filename);

    return status;
}
