%option noyywrap nodefault yylineno

%{
#include <string.h>
#include <stdio.h>

#include "sweet_bison.h"

#include "sweet_vinumc.h"

/*
 * Disables input() and unput() function definitions from the generated .c
 * file. This is done to remove the unused functions warnings. If some day we
 * need to use these functions, we need to remove these define.
 */
#define YY_NO_INPUT
#define YY_NO_UNPUT
%}

alpha [a-zA-Z]

numeric [0-9]

blank [ \r\n\t]

alphanum {alpha}|{numeric}

block (\(([^()])*\))?(\.[^ \n\r\t]+)+

%s inside_block outside_block

%%


"(" {
  ECHO;
  BEGIN inside_block;
  printf("entrei   num bloco\n");
}

<inside_block>.*?/\)\.[^ \n\r\t]+ {
  ECHO;

  BEGIN outside_block;
  printf("sai dum bloco\n");
}

<inside_block>\)/\.[^ \n\r\t]+ {
  ECHO;
}

<<EOF>> {printf("\nAcabou!\n"); return 0;}

.|\n {ECHO; printf("aqui no content teoricamente\n");}

{blank}+ { }


. { yyerror("[ERROR]  \"%c\" not covered", *yytext); }

%%
