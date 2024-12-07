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
name_stop [ \t\r\n\(\)\.]


%%

"(" { 
  yylval.str = strdup(yytext); 
  return '('; 
}

")" { 
  yylval.str = strdup(yytext); 
  return ')'; 
}

"." { 
  yylval.str = strdup(yytext); 
  return '.'; 
}

{name}+/{name_stop} {
  yylval.str = strdup(yytext);
  printf("\n token NAME: %s", yytext);
  return NAME;
}

{content}+ {
  yylval.str = strdup(yytext);
  printf("\n token CONTENT: %s", yytext);

  return CONTENT;
}

<<EOF>> { 
  printf("\nFim da entrada\n"); 
  return 0; 
}
