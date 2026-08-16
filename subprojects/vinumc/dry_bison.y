%{
#include <stdio.h>
#include <ctype.h>
#include <wctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "libvinumc.h"
#include "utils.h"

typedef void* yyscan_t;
int yylex(YYSTYPE *yylval_param, yyscan_t yyscanner);

void yyerror(yyscan_t scanner, struct compiler_ctx *ctx, const char *fmt, ...);
%}

%code requires {
	/* This code goes into the header file (dry_bison.h) */
	typedef void* yyscan_t;
	struct compiler_ctx;
}

/* Generate a thread-safe (pure) parser */
%define api.pure full

/* Pass the scanner to yylex() */
%lex-param   { yyscan_t scanner }

/* Pass the scanner and your context to yyparse() */
%parse-param { yyscan_t scanner }
%parse-param { struct compiler_ctx *ctx }

%token ASSIGNMENT
%token CALL
%token ARGS
%token ARG_REF_ALL_ARGS
%token PROGRAM
%token SYMBOL
// TODO: FUNCTION is not used in bison but in eval
%token FUNCTION
// TODO: UNDEFINED is not used in bison but in eval
%token UNDEFINED
%token TEXT
%token LITERAL

%%

program:
       {
       UNUSED(yynerrs);
	$$ = ast_node_new_nvl(&ctx->ast, PROGRAM);
       }
       | program args_child {
	ast_add_child(&ctx->ast, $1, $2);
	$$ = $1;
       }
       ;

block:
     '[' symbol ':' args ']'  {
	ast_node_id_t node = ast_node_new_nvl(&ctx->ast, ASSIGNMENT);

	ast_add_child(&ctx->ast, node, $2);
	ast_add_child(&ctx->ast, node, $4);

	$$ = node;
   }
   | '[' symbol args ']'  {
	ast_node_id_t node = ast_node_new_nvl(&ctx->ast, CALL);

	ast_add_child(&ctx->ast, node, $2);
	ast_add_child(&ctx->ast, node, $3);

	$$ = node;
   }
   | '[' symbol ']'  {
	ast_node_id_t node = ast_node_new_nvl(&ctx->ast, CALL);

	ast_add_child(&ctx->ast, node, $2);

	$$ = node;
   }
   ;

args:
	args_child {
		ast_node_id_t node = ast_node_new_nvl(&ctx->ast, ARGS);
		ast_add_child(&ctx->ast, node, $1);

		$$ = node;
	}
	| args args_child {
		ast_add_child(&ctx->ast, $1, $2);

		$$ = $1;
	}
	;

args_child:
	TEXT { $$ = $1; }
	| block { $$ = $1; }
	| ARG_REF_ALL_ARGS { $$ = $1; }
	| LITERAL { $$ = $1; }
	;

symbol: SYMBOL {
	// making so our symbols are case insensitive by making the whole string lowercase
	struct vut_sv text = ast_get_text(&ctx->ast, $1);
	struct vut_str tmp_str = vut_str_from_vut_sv(text, ctx->dry_arena);

	// we need to convert from multi-byte to wide-character string
	wchar_t *wtext = (wchar_t*)malloc(text.len * sizeof(wchar_t));
	// TODO: handle the function return value
	mbstowcs(wtext, text.base, text.len);

	for(size_t i = 0; i < text.len; i++) {
		wtext[i] = towlower(wtext[i]);
	}
	// converting back to multi-byte
	// TODO: handle the function return value
	wcstombs(tmp_str.base, wtext, text.len);
	free(wtext);

	// TODO: Find a way to free this tmp_str
	ast_set_text(&ctx->ast, $1, vut_sv_from_vut_str(&tmp_str));

	$$ = $1;
      }
      | block {
	ast_node_id_t node = ast_node_new_nvl(&ctx->ast, SYMBOL);
	ast_add_child(&ctx->ast, node, $1);

	$$ = node;
      }
      ;
%%


void yyerror(yyscan_t scanner, struct compiler_ctx *ctx, const char *fmt, ...) {
	UNUSED(scanner);
	UNUSED(ctx);
	va_list ap;
	va_start(ap, fmt);

	fprintf(stderr, "[ERROR]:");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");

	va_end(ap);
}
