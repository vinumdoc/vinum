%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sweet_vinumc.h"

int yylex();
%}

%union {
  char* str;
}

%type<str> program block content name
%token<str> CONTENT NAME '(' ')' '.' NEW_LINE

%%

program:
    // Programa vazio 
    { $$ = strdup(""); }
    | program block {
        // Concatena as linhas do programa
        size_t len = strlen($1) + strlen($2) + 1;
        $$ = malloc(len + 1);
        snprintf($$, len + 1, "%s%s", $1, $2);
        free($1);
        free($2);
        printf("\nprogram: %s", $$);
    };

block:
    '(' block ')' '.' name {
        // Transforma blocos Sweet em Dry
        size_t len = strlen($2) + strlen($5) + 4; // "[name content]"
        $$ = malloc(len);
        snprintf($$, len, "[%s %s]", $5, $2);
        free($2);
        free($5);
        printf("\no bloco lido foi: %s", $$);
    }
    | '.' name {
        size_t len = strlen($2) + 3; // "[name]"
        $$ = malloc(len);
        snprintf($$, len, "[%s]", $2);
        free($2);
        printf("\no bloco lido foi: %s", $$);
    }
    | content {
        printf("\nÉ um bloco no formato CONTENT\n");
        $$ = $1;
    }
    | content block content {
        printf("\nÉ um bloco no formato CONTENT BLOCK CONTENT\n");
        size_t len = strlen($1) + strlen($2) + strlen($3) + 3; 
        $$ = malloc(len);
        snprintf($$, len, "%s %s %s", $1, $2, $3);
        free($1);
        free($2);
        free($3);
        printf("\no bloco lido foi: %s", $$);
    };
    // | content NEW_LINE NEW_LINE {
    //     // É um parágrafo
    //     size_t len = strlen($2) + 7; // "[par content]"
    //     $$ = malloc(len);
    //     snprintf($$, len, "[par %s]", $2);
    //     free($2);
    // }

content: //content pode ter (, ) e . -> ainda nao vi isso 
    CONTENT { 
        $$ = strdup($1); 
    }
    | name {
        $$ = strdup("");
    }
    | /* ε */ {
        $$ = strdup("");
    };


name:
    NAME { $$ = strdup($1); };

%%


