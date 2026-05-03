%option noyywrap nodefault yylineno noyy_top_state
%option stack

%option reentrant bison-bridge

%option extra-type="struct compiler_ctx *"

%x DEFINE_CALL_NAME
%x HAS_CALL_NAME
%x CONTENT
%x IN_LITERAL

%{
#include <string.h>

#include <vutils/str.h>
#include <vutils/system_allocator.h>

#include "dry_bison.h"
#include "libvinumc.h"

/*
 * Disables input() and unput() function definitions from the generated .c
 * file. This is done to remove the unused functions warnings. If some day we
 * need to use these functions, we need to remove these define.
 */
#define YY_NO_INPUT
#define YY_NO_UNPUT

static void replace_state(int new_state, yyscan_t yyscanner);
static int yield_text_node(yyscan_t yyscanner);

void yyerror(yyscan_t scanner, struct compiler_ctx *ctx, const char *fmt, ...);
%}

BEFORE_TEXT_END ("["|"]"|"$*"|"{#")
%%


<INITIAL>[ \t\r\n]+ ;
<INITIAL>[^\[\] \t\r\n]+ {
	struct ast_node node = ast_node_new(TEXT, vut_sv_from_cstr(yytext), yyextra->ast.allocator);
	*yylval = ast_add_node(&yyextra->ast, node);

	return TEXT;
}

<INITIAL,CONTENT>"[" { yy_push_state(DEFINE_CALL_NAME, yyscanner); return *yytext; }
<INITIAL>"]" { return *yytext; }
<CONTENT>"]" { yy_pop_state(yyscanner); return *yytext; }

<DEFINE_CALL_NAME>"[" {
	replace_state(HAS_CALL_NAME, yyscanner);
	yy_push_state(DEFINE_CALL_NAME, yyscanner);
	return *yytext;
}
<DEFINE_CALL_NAME,HAS_CALL_NAME>"]" { yy_pop_state(yyscanner); return *yytext; }
<DEFINE_CALL_NAME,HAS_CALL_NAME>[ \t]*: { replace_state(CONTENT, yyscanner); return ':'; }
<DEFINE_CALL_NAME>[ \t\r\n]+ ;
<DEFINE_CALL_NAME>[^\[\]: \t\r\n]+ {
	struct ast_node node = ast_node_new(SYMBOL, vut_sv_from_cstr(yytext), yyextra->ast.allocator);
	*yylval = ast_add_node(&yyextra->ast, node);

	replace_state(HAS_CALL_NAME, yyscanner);

	return SYMBOL;
}
<HAS_CALL_NAME>[ \t\r\n] { replace_state(CONTENT, yyscanner); }
<HAS_CALL_NAME>. { replace_state(CONTENT, yyscanner); yyless(0); }

<CONTENT>"$*" {
	struct ast_node node = ast_node_new_nvl(ARG_REF_ALL_ARGS, yyextra->ast.allocator);
	*yylval = ast_add_node(&yyextra->ast, node);

	return ARG_REF_ALL_ARGS;
}

<CONTENT>"{#" { yy_push_state(IN_LITERAL, yyscanner); }
<IN_LITERAL>"#}" {
	struct ast_node node = ast_node_new(LITERAL, vut_sv_from_str(yytext, yyleng - 2), yyextra->ast.allocator);
	*yylval = ast_add_node(&yyextra->ast, node);

	yy_pop_state(yyscanner);
	return LITERAL;
}
<IN_LITERAL>(.|\n)/(.|\n) {
	yymore();
}
<IN_LITERAL>(.|\n) {
	// the next token is eof, so we yield the
	// contents buffered so far
	struct ast_node node = ast_node_new(LITERAL, vut_sv_from_str(yytext, yyleng),
	yyextra->ast.allocator);
	*yylval = ast_add_node(&yyextra->ast, node);
	yy_pop_state(yyscanner);
	return LITERAL;
}


<CONTENT>\\(.|\n)/({BEFORE_TEXT_END}) {
	VUT_VEC_PUT(&yyextra->dry_flex_text_buffer, yytext[1]);
	return yield_text_node(yyscanner);
}
<CONTENT>\\(.|\n)/(.|\n) {
	VUT_VEC_PUT(&yyextra->dry_flex_text_buffer, yytext[1]);
}
<CONTENT>\\(.|\n) {
	// the next token is eof, so we yield the
	// contents buffered so far
	VUT_VEC_PUT(&yyextra->dry_flex_text_buffer, yytext[1]);
	return yield_text_node(yyscanner);
}

<CONTENT>([^\[\]])/({BEFORE_TEXT_END}) {
	VUT_VEC_PUT(&yyextra->dry_flex_text_buffer, *yytext);
	return yield_text_node(yyscanner);
}
<CONTENT>([^\[\]])/(.|\n) {
	VUT_VEC_PUT(&yyextra->dry_flex_text_buffer, *yytext);
}
<CONTENT>([^\[\]]) {
	// the next token is eof, so we yield the
	// contents buffered so far
	VUT_VEC_PUT(&yyextra->dry_flex_text_buffer, *yytext);
	return yield_text_node(yyscanner);
}

<*>. { yyerror(yyscanner, yyextra, "[ERROR]  \"%c\" not covered", *yytext); }

%%

static void replace_state(int new_state, yyscan_t yyscanner) {
	yy_pop_state(yyscanner);
	yy_push_state(new_state, yyscanner);
}

static int yield_text_node(yyscan_t yyscanner) {

	/* Manually get yylval and context from the scanner */
	YYSTYPE * lval = yyget_lval(yyscanner);
	struct compiler_ctx * extra = yyget_extra(yyscanner);

	struct ast_node node = ast_node_new(
		TEXT,
		vut_sv_from_vut_str(&extra->dry_flex_text_buffer),
		extra->ast.allocator
	);
	// transfers the string to the ast node
	extra->dry_flex_text_buffer = vut_str_init(extra->ast.allocator);

	*lval = ast_add_node(&extra->ast, node);
	return TEXT;
}
