%code requires {
	extern char *filename; /* current filename here for the lexer */
	typedef struct YYLTYPE {
		int first_line;
		int first_column;
		int last_line;
		int last_column;
		char *filename;
	} YYLTYPE;
	# define YYLTYPE_IS_DECLARED 1 /* alert the parser that we have our own definition */
	# define YYLLOC_DEFAULT(Current, Rhs, N) \
		do \
			if (N) \
				{ \
					(Current).first_line = YYRHSLOC (Rhs, 1).first_line; \
					(Current).first_column = YYRHSLOC (Rhs, 1).first_column; \
					(Current).last_line = YYRHSLOC (Rhs, N).last_line; \
					(Current).last_column = YYRHSLOC (Rhs, N).last_column; \
					(Current).filename = YYRHSLOC (Rhs, 1).filename; \
				} \
			else \
				{ /* empty RHS */ \
					(Current).first_line = (Current).last_line = \
					YYRHSLOC (Rhs, 0).last_line; \
					(Current).first_column = (Current).last_column = \
					YYRHSLOC (Rhs, 0).last_column; \
					(Current).filename = NULL; /* new */ \
				} \
		while (0)
}

%{
#include <stdio.h>
#include <ctype.h>
#include <wctype.h>
#include <stdlib.h>
#include <string.h>

#include "parser_common.h"

int yylex();

char* filename;
%}
%locations

%token ASSIGNMENT
%token CALL
%token ARGS
%token ARG_REF_ALL_ARGS
%token PROGRAM
%token SYMBOL
%token TEXT
%token WORD

%%

program:
	{
		struct ast_node node = ast_node_new_nvl(PROGRAM);
		$$ = ast_add_node(&ctx.ast, node);
	}
	| program block {
		struct ast_node *node = &VEC_AT(&ctx.ast.nodes, $1);

		ast_node_add_child(node, $2);
		$$ = $1;
	}
	;

block:
	'[' symbol ':' args ']'  {
		struct ast_node node = ast_node_new_nvl(ASSIGNMENT);

		ast_node_add_child(&node, $2);
		ast_node_add_child(&node, $4);

		$$ = ast_add_node(&ctx.ast, node);
	}
	| '[' symbol args ']'  {
		struct ast_node node = ast_node_new_nvl(CALL);

		ast_node_add_child(&node, $2);
		ast_node_add_child(&node, $3);

		$$ = ast_add_node(&ctx.ast, node);
	}
	| '[' symbol ']'  {
		struct ast_node node = ast_node_new_nvl(CALL);

		ast_node_add_child(&node, $2);

		$$ = ast_add_node(&ctx.ast, node);
	}
	| '[' block ']' {
		// TODO: just moved the code, need to check if this is correct
		struct ast_node node = ast_node_new_nvl(SYMBOL);

		ast_node_add_child(&node, $1);

		$$ = ast_add_node(&ctx.ast, node);
	}
	| error ']' {
		lyyerror(@2, "error: extra ']'");	
	}
	| '[' error {
		lyyerror(@$, "error: no matching for '['");	
	}
	;

args:
	text {
		struct ast_node node = ast_node_new_nvl(ARGS);

		ast_node_add_child(&node, $1);

		size_t arg_node_id = ast_add_node(&ctx.ast, node);

		$$ = arg_node_id;
	}
	| block {
		struct ast_node node = ast_node_new_nvl(ARGS);

		ast_node_add_child(&node, $1);

		$$ = ast_add_node(&ctx.ast, node);
	}
	| ARG_REF_ALL_ARGS {
		struct ast_node node = ast_node_new_nvl(ARGS);

		ast_node_add_child(&node, $1);

		$$ = ast_add_node(&ctx.ast, node);
	}
	| args text {
		struct ast_node *node = &VEC_AT(&ctx.ast.nodes, $1);

		ast_node_add_child(node, $2);

		$$ = $1;
	}
	| args block {
		struct ast_node *node = &VEC_AT(&ctx.ast.nodes, $1);

		ast_node_add_child(node, $2);

		$$ = $1;
	}
	| args ARG_REF_ALL_ARGS {
		struct ast_node *node = &VEC_AT(&ctx.ast.nodes, $1);

		ast_node_add_child(node, $2);

		$$ = $1;
	}
	;

symbol: WORD {
	VEC_AT(&ctx.ast.nodes, $1).type = SYMBOL;

	// making so our symbols are case insensitive by making the whole string lowercase
	char *text = VEC_AT(&ctx.ast.nodes, $1).text;
	size_t len = strlen(text);

	// we need to convert from multi-byte to wide-character string
	wchar_t *wtext = (wchar_t*)malloc(len * sizeof(wchar_t));
	// TODO: handle the function return value
	mbstowcs(wtext, text, len);
	
	for(size_t i = 0; i < len; i++) {
		wtext[i] = towlower(wtext[i]);
	}
	// converting back to multi-byte
	// TODO: handle the function return value
	wcstombs(text, wtext, len);
	free(wtext);

	$$ = $1;
	}
	;

text:
	WORD {
		struct ast_node node = ast_node_new_nvl(TEXT);

		ast_node_add_child(&node, $1);

		$$ = ast_add_node(&ctx.ast, node);
	}
	| text WORD {
		struct ast_node *node = &VEC_AT(&ctx.ast.nodes, $1);

		ast_node_add_child(node, $2);

		$$ = $1;
	}
	;

%%
