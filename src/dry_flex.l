%option noyywrap nodefault yylineno

%{
#include <string.h>

#include "dry_bison.h"

#include "vinumc.h"
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
