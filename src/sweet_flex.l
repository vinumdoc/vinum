%option noyywrap nodefault yylineno

%{
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sweet_bison.h"
#include "sweet_vinumc.h"

#define YY_NO_INPUT
%}

/* Define custom lexer state */
%s AFTER_DOT

%%

"(" { return '('; }
")" { return ')'; }

"."[a-zA-Z_] {
/* Dot followed by identifier: return dot and leave identifier for next token */
  unput(yytext[1]);
/* Switch to AFTER_DOT mode */
  BEGIN(AFTER_DOT); 
  return '.'; 
}

<AFTER_DOT>[a-zA-Z_][a-zA-Z0-9_]* {
/* In AFTER_DOT state: match NAME */
  yylval.str = strdup(yytext);
  BEGIN(INITIAL); // back to default state
  return NAME;
}

<AFTER_DOT>. {
/* In AFTER_DOT state: fallback to CONTENT and revert state */
  // Not a valid NAME after dot, treat it as content
  char* s = malloc(strlen(yytext) + 2);
  snprintf(s, strlen(yytext) + 2, "%s", yytext);
  yylval.str = s;
  BEGIN(INITIAL);
  return CONTENT;
}

[ \t\n]+ {
/* Whitespace or newline: CONTENT */
  yylval.str = strdup(yytext);
  return CONTENT;
}

. {
/* Any other character: also CONTENT */
  yylval.str = strdup(yytext);
  return CONTENT;
}
%%
