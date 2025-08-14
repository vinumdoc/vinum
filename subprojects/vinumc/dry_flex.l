%option noyywrap nodefault yylineno noyy_top_state
%option stack

%x DEFINE_CALL_NAME
%x HAS_CALL_NAME
%x CONTENT
%x IN_LITERAL

%{
#include <string.h>

#include "dry_bison.h"

#include "vinumc.h"
#include "str.h"

/*
 * Disables input() and unput() function definitions from the generated .c
 * file. This is done to remove the unused functions warnings. If some day we
 * need to use these functions, we need to remove these define.
 */
#define YY_NO_INPUT
#define YY_NO_UNPUT

static void replace_state(int new_state);
static int yield_text_node(void);

// used to handle character escaping inside the regular text blocks
// preventing having two consecutive TEXT nodes
// that could be just one node
static struct str text_buffer = {};
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

<CONTENT>"{#" { yy_push_state(IN_LITERAL); }
<IN_LITERAL>"#}" {
	struct ast_node node = ast_node_new(LITERAL, strndup(yytext, yyleng - 2));
	yylval = ast_add_node(&ctx.ast, node);

	yy_pop_state();
	return LITERAL;
}
<IN_LITERAL>(.|\n)/(.|\n) {
	yymore();
}
<IN_LITERAL>(.|\n) {
	// the next token is eof, so we yield the
	// contents buffered so far
	struct ast_node node = ast_node_new(LITERAL, strndup(yytext, yyleng));
	yylval = ast_add_node(&ctx.ast, node);
	yy_pop_state();
	return LITERAL;
}


<CONTENT>\\(.|\n)/({BEFORE_TEXT_END}) {
	VEC_PUT(&text_buffer, yytext[1]);
	return yield_text_node();
}
<CONTENT>\\(.|\n)/(.|\n) {
	VEC_PUT(&text_buffer, yytext[1]);
}
<CONTENT>\\(.|\n) {
	// the next token is eof, so we yield the
	// contents buffered so far
	VEC_PUT(&text_buffer, yytext[1]);
	return yield_text_node();
}

<CONTENT>([^\[\]])/({BEFORE_TEXT_END}) {
	VEC_PUT(&text_buffer, *yytext);
	return yield_text_node();
}
<CONTENT>([^\[\]])/(.|\n) {
	VEC_PUT(&text_buffer, *yytext);
}
<CONTENT>([^\[\]]) {
	// the next token is eof, so we yield the
	// contents buffered so far
	VEC_PUT(&text_buffer, *yytext);
	return yield_text_node();
}

<*>. { yyerror("[ERROR]  \"%c\" not covered", *yytext); }

%%

static void replace_state(int new_state) {
	yy_pop_state();
	yy_push_state(new_state);
}

static int yield_text_node(void) {
	VEC_PUT(&text_buffer, '\0');
	struct ast_node node = ast_node_new(TEXT, text_buffer.base);
	// transfers the string to the ast node
	text_buffer = (struct str){};
	yylval = ast_add_node(&ctx.ast, node);
	return TEXT;
}
