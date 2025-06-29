%option noyywrap nodefault yylineno noyy_top_state
%option stack

%x DEFINE_CALL_NAME
%x HAS_CALL_NAME
%x CONTENT

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

static void replace_state(int new_state);
static int yield_text_node(void);
%}

BEFORE_TEXT_END ("["|"]"|"$*"|"{#")
%%


<INITIAL>[ \t\r\n]+ ;
<INITIAL>[^\[\] \t\r\n]+ {
	struct ast_node node = ast_node_new(TEXT, strdup(yytext));
	yylval = ast_add_node(&ctx.ast, node);

	return TEXT;
}

<INITIAL,CONTENT>"[" { yy_push_state(DEFINE_CALL_NAME); return *yytext; }
<INITIAL>"]" { return *yytext; }
<CONTENT>"]" { yy_pop_state(); return *yytext; }

<DEFINE_CALL_NAME>"[" {
	replace_state(HAS_CALL_NAME);
	yy_push_state(DEFINE_CALL_NAME);
	return *yytext;
}
<DEFINE_CALL_NAME,HAS_CALL_NAME>"]" { yy_pop_state(); return *yytext; }
<DEFINE_CALL_NAME,HAS_CALL_NAME>[ \t]*: { replace_state(CONTENT); return ':'; }
<DEFINE_CALL_NAME>[ \t\r\n]+ ;
<DEFINE_CALL_NAME>[^\[\]: \t\r\n]+ {
	struct ast_node node = ast_node_new(SYMBOL, strdup(yytext));
	yylval = ast_add_node(&ctx.ast, node);

	replace_state(HAS_CALL_NAME);

	return SYMBOL;
}
<HAS_CALL_NAME>[ \t\r\n] { replace_state(CONTENT); }
<HAS_CALL_NAME>. { replace_state(CONTENT); yyless(0); }

<CONTENT>"$*" {
	struct ast_node node = ast_node_new_nvl(ARG_REF_ALL_ARGS);
	yylval = ast_add_node(&ctx.ast, node);

	return ARG_REF_ALL_ARGS;
}

<CONTENT>([^\[\]])/({BEFORE_TEXT_END}) {
	return yield_text_node();
}
<CONTENT>([^\[\]])/(.|\n) {
	yymore();
}
<CONTENT>([^\[\]]) {
	// the next token is eof, so we yield the
	// contents buffered so far
	return yield_text_node();
}

<*>. { yyerror("[ERROR]  \"%c\" not covered", *yytext); }

%%

static void replace_state(int new_state) {
	yy_pop_state();
	yy_push_state(new_state);
}

static int yield_text_node(void) {
	struct ast_node node = ast_node_new(TEXT, strdup(yytext));
	yylval = ast_add_node(&ctx.ast, node);
	return TEXT;
}
