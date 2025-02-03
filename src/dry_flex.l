%option noyywrap nodefault yylineno

%{
#include <string.h>

#include "dry_bison.h"

#include "parser_common.h"

/*
 * Disables input() and unput() function definitions from the generated .c
 * file. This is done to remove the unused functions warnings. If some day we
 * need to use these functions, we need to remove these define.
 */
#define YY_NO_INPUT
#define YY_NO_UNPUT

/* handle locations */
int yycolumn = 1;
#define YY_USER_ACTION yylloc.filename = filename; \
	yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn; yylloc.last_column = yycolumn+yyleng-1; \
	yycolumn += yyleng;
%}

%%

"[" |
"]" |
":" { return *yytext; }

\n {
	yycolumn = 1;
}

[ \t\r]+ { }

"$*" {
	struct ast_node node = ast_node_new_nvl(ARG_REF_ALL_ARGS);
	yylval = ast_add_node(&ctx.ast, node);

	return ARG_REF_ALL_ARGS;
}

([^\[\]: \t\r\n])+ {
	struct ast_node node = ast_node_new(WORD, strdup(yytext));
	yylval = ast_add_node(&ctx.ast, node);

	return WORD;
}

. { yyerror("[ERROR]  \"%c\" not covered", *yytext); }

%%
