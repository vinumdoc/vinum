%option noyywrap nodefault yylineno

%{
#include <string.h>
#include <stdio.h>

#include "sweet_bison.h"
#include "sweet_vinumc.h"

#define YY_NO_INPUT
#define YY_NO_UNPUT
%}

%x READING_NAME

name [^ \t\r\n\(\)\.]
content [^\(\)\.] 
blank [ \t\r\n]


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
  BEGIN(READING_NAME); 
  return '.';
}

<READING_NAME>{name}+ {
  yylval.str = strdup(yytext); 
  printf("\n token NAME: %s", yytext);
  return NAME;
}

<READING_NAME>"." {
  yylval.str = strdup(yytext); 
  return '.';
}

<READING_NAME>{blank} {
  yylval.str = strdup(yytext); 
  BEGIN(INITIAL); 
  return CONTENT;
}

<READING_NAME>"(" {
  yylval.str = strdup(yytext); 
  BEGIN(INITIAL); 
  return '(';
}

<READING_NAME>")" {
  yylval.str = strdup(yytext); 
  BEGIN(INITIAL); 
  return ')';
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
