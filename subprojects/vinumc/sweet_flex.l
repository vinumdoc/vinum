%option noyywrap nodefault yylineno

%{
#include <string.h>
#include <stdio.h>

#include "sweet_bison.h"
#include "sweet_vinumc.h"

#define YY_NO_INPUT
#define YY_NO_UNPUT
%}

name [^ \t\r\n\(\)\.]
content [^\(\)\.] 

%%

"(" { 
  yylval.str = strdup(yytext); 
  printf("\nBEGIN BLOCK\n");
  return '('; 
}

")" { 
  printf("\nEND BLOCK\n");
  yylval.str = strdup(yytext); 
  return ')'; 
}

"." { 
  printf("\npoint\n");
  yylval.str = strdup(yytext); 
  return '.'; 
}

{name}+ {
  yylval.str = strdup(yytext);
  printf("\nToken NAME: %s\n", yytext);
  return NAME;
}

{content}+ {
  yylval.str = strdup(yytext);
  printf("\nToken CONTENT: %s\n", yytext);
  return CONTENT;
}

\n+ { 
  yylval.str = strdup("\n");
  return NEW_LINE; 
}

<<EOF>> { 
  printf("\nFim da entrada\n"); 
  return 0; 
}
