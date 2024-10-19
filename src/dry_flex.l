%option noyywrap nodefault yylineno

%{
#include <string.h>

#include "dry_bison.h"

#include "vinumc.h"

/*
 * Disables input() and unput() function definitions from the generated .c
 * file. This is done to remove the unused functions warnings. If some day we
 * need to use these functions, we need to remove these define.
 */
#define YY_NO_INPUT
#define YY_NO_UNPUT
%}

%%

"[" |
"]" |
":" { return *yytext; }

[ \t\n\r]+ { }

([^\[\]: \t\r\n])+ {
	struct ast_node node = ast_node_new(WORD, strdup(yytext));
	yylval = ast_add_node(&ctx.ast, node);

	return WORD;
}

. { yyerror("[ERROR]  \"%c\" not covered", *yytext); }

%%
