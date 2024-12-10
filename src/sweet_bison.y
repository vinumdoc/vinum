%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sweet_vinumc.h"

int yylex();

char *include_content(char *content, char *block){
    int block_size = strlen(block);
    int content_size = strlen(content);
    int len = block_size + content_size + 2;
    char *complete_block = malloc(sizeof(char) * (len));
    int pos;

    for(pos = block_size; pos >= 0; pos--)
        if(block[pos-1] != ']') break;
    
    int i = 0;
    for(int new_i = 0; new_i < len; new_i++){
        if(new_i == pos){
            complete_block[new_i] = ' ';
            new_i++;

            for(int j = 0; j <= content_size; j++)
                complete_block[new_i+j] = content[j];
            
            new_i += content_size;
        }
        complete_block[new_i] = block[i];
        i++;
    }

    return complete_block;
}

%}

%union {
  char* str;
}

%type<str> program block content name call
%token<str> CONTENT NAME '(' ')' '.'

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
    '(' block ')' call {
        // Transforma blocos Sweet em Dry
        $$ = include_content($2, $4);
        free($2);
        free($4);
    }
    | call {
        $$ = $1;
    }
    | content {
        $$ = $1;
    }
    | block block {
        size_t len = strlen($1) + strlen($2) + 1; 
        $$ = malloc(len);
        snprintf($$, len, "%s%s", $1, $2);
        free($1);
        free($2);
    };
    // | content NEW_LINE NEW_LINE {
    //     // É um parágrafo
    //     size_t len = strlen($2) + 7; // "[par content]"
    //     $$ = malloc(len);
    //     snprintf($$, len, "[par %s]", $2);
    //     free($2);
    // }

call:
    '.' name {
        size_t len = strlen($2) + 3; // "[name]"
        $$ = malloc(len);
        snprintf($$, len, "[%s]", $2);
        free($2);
    }
    | call '.' name {
        size_t len = strlen($1) + strlen($3) + 4; // "[name call]"
        $$ = malloc(len);
        snprintf($$, len, "[%s %s]", $3, $1);
        free($3);
        free($1);
    };

content: //content pode ter (, ) e . -> ainda nao vi isso 
    name content{ 
        size_t len = strlen($1) + strlen($2) + 1; 
        $$ = malloc(len);
        snprintf($$, len, "%s%s", $1, $2);
        free($1);
        free($2); 
    }
    | CONTENT {
        $$ = strdup($1);
    }
    | /* ε */ {
        $$ = strdup("");
    };


name:
    NAME { $$ = strdup($1); };

%%


