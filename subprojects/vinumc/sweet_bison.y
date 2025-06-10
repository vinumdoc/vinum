%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sweet_vinumc.h"

// Forward declaration
int yylex();

// Utility function declarations
char* concat(const char* a, const char* b);
char* format_block(const char* name, const char* content);
char* format_call(const char* outer, const char* inner);
char* put_in_chain(const char* inner_block, const char* call_chain);

// Output file pointer
extern FILE* output_file;
%}

%union {
    char* str;
}

%token<str> NAME CONTENT
%type<str> program block call final_

%%

// Entry point of the parser; prints the fully transpiled result
final_: 
    program {
        $$ = strdup($1); 
        free($1);
        if (output_file) {
            fprintf(output_file, "%s", $$);
        }
    }
;

// Program: sequence of blocks or empty
program:
    /* empty */ { $$ = strdup(""); }
    | program block {
        $$ = concat($1, $2);
        free($1); 
        free($2);
    }
;

// Block constructs
block:
    // Parenthesized block
    '(' program ')' {
        size_t len = strlen($2) + 3;
        $$ = malloc(len);
        snprintf($$, len, "(%s)", $2);
        free($2);
    }
    // Parenthesized block with call chain
    | '(' program ')' '.' NAME call {
        char* first_block = format_block($5, $2);
        char* chained_block = put_in_chain(first_block, $6);
        $$ = format_block(chained_block, "");
        free($5); 
        free($2);
        free($6);
    }
    // Parenthesized block with single call
    | '(' program ')' call {
        $$ = format_block($4, $2);
        free($4); 
        free($2);
    }
    // Call without arguments block
    | call {
        $$ = format_block($1, "");
        free($1);
    }
    // Raw content
    | CONTENT { 
        $$ = strdup($1); 
        free($1); 
    }
;

// Chained function calls
call:
    '.' NAME {
        $$ = strdup($2); 
        free($2);
    }
    | call '.' NAME {
        $$ = format_call($3, $1);
        free($1); 
        free($3);
    }
;

%%

/**
 * Concatenates two strings. If either string is empty, returns a copy of the other.
 *
 * @param a First string
 * @param b Second string
 * @return Concatenated result
 */
char* concat(const char* a, const char* b) {
    if (strlen(a) == 0) return strdup(b);
    if (strlen(b) == 0) return strdup(a);

    size_t len = strlen(a) + strlen(b) + 2; // +1 for NULL and extra safety
    char* result = malloc(len);
    snprintf(result, len, "%s%s", a, b);
    return result;
}

/**
 * Formats a function block in the dry flavor: [name content] or [name] if content is empty.
 *
 * @param name Function name
 * @param content Block content
 * @return Formatted dry-style block
 */
char* format_block(const char* name, const char* content) {
    size_t len = strlen(name) + strlen(content) + 4; // [], space and NULL
    char* result = malloc(len);

    if (strlen(content) > 0)
        snprintf(result, len, "[%s %s]", name, content);
    else
        snprintf(result, len, "[%s]", name);

    return result;
}

/**
 * Formats a function call inside another: returns a string like `outer [inner]`.
 *
 * @param outer Outer function name
 * @param inner Inner block
 * @return Nested block string
 */
char* format_call(const char* outer, const char* inner) {
    size_t len = strlen(outer) + strlen(inner) + 5; // '[', ' ', ']', NULL
    char* result = malloc(len);
    snprintf(result, len, "%s [%s]", outer, inner);
    return result;
}

/**
 * Inserts an inner block into the last nested position of a call chain.
 *
 * For example:
 *   call_chain = "bar [baz [qux]]"
 *   inner_block = "[foo content]"
 *   result = "bar [baz [qux [foo content]]]"
 *
 * @param inner_block The block to be inserted (e.g., "[foo content]")
 * @param call_chain  The chain of nested calls (e.g., "bar [baz [qux]]")
 * @return A newly allocated string with inner_block inserted at the correct position.
 */
char* put_in_chain(const char* inner_block, const char* call_chain) {
    if (!call_chain || strlen(call_chain) == 0) {
        return strdup(inner_block);
    }

    size_t call_len = strlen(call_chain);
    size_t inner_len = strlen(inner_block);

    // Find the position where the final ']' sequence starts
    ssize_t i = call_len - 1;
    while (i >= 0 && call_chain[i] == ']') {
        i--;
    }
    i++;

    // Split the call_chain into prefix and suffix
    char* prefix = strndup(call_chain, i);
    char* suffix = strdup(&call_chain[i]);

    size_t result_len = strlen(prefix) + inner_len + strlen(suffix) + 2; // +1 for space +1 for '\0'
    char* result = malloc(result_len);

    // Format the final string
    snprintf(result, result_len, "%s %s%s", prefix, inner_block, suffix);

    // Clean up temporary strings
    free(prefix);
    free(suffix);

    return result;
}
