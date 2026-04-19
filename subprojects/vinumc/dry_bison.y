%{
#include <stdio.h>
#include <ctype.h>
#include <wctype.h>
#include <stdlib.h>
#include <string.h>

#include "vinumc.h"

int yylex();
%}

%token ASSIGNMENT
%token CALL
%token ARGS
%token ARG_REF_ALL_ARGS
%token PROGRAM
%token SYMBOL
// TODO: FUNCTION is not used in bison but in eval
%token FUNCTION
%token TEXT
%token LITERAL

%%

program:
       {
	struct ast_node node = ast_node_new_nvl(PROGRAM, ctx.ast.allocator);
	$$ = ast_add_node(&ctx.ast, node);
       }
       | program block {
	ast_add_child(&ctx.ast, $1, $2);
	$$ = $1;
       }
       ;

block:
     '[' symbol ':' args ']'  {
	ast_node_id_t node = ast_add_node(&ctx.ast, ast_node_new_nvl(ASSIGNMENT, ctx.ast.allocator));

	ast_add_child(&ctx.ast, node, $2);
	ast_add_child(&ctx.ast, node, $4);

	$$ = node;
   }
   | '[' symbol args ']'  {
	ast_node_id_t node = ast_add_node(&ctx.ast, ast_node_new_nvl(CALL, ctx.ast.allocator));

	ast_add_child(&ctx.ast, node, $2);
	ast_add_child(&ctx.ast, node, $3);

	$$ = node;
   }
   | '[' symbol ']'  {
	ast_node_id_t node = ast_add_node(&ctx.ast, ast_node_new_nvl(CALL, ctx.ast.allocator));

	ast_add_child(&ctx.ast, node, $2);

	$$ = node;
   }
   ;

args:
	args_child {
		ast_node_id_t node = ast_add_node(&ctx.ast, ast_node_new_nvl(ARGS, ctx.ast.allocator));
		ast_add_child(&ctx.ast, node, $1);

		$$ = node;
	}
	| args args_child {
		ast_add_child(&ctx.ast, $1, $2);

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
	char *text = VUT_VEC_AT(&ctx.ast.nodes, $1).text;
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
      | block {
	ast_node_id_t node = ast_add_node(&ctx.ast, ast_node_new_nvl(SYMBOL, ctx.ast.allocator));
	ast_add_child(&ctx.ast, node, $1);

	$$ = node;
      }
      ;
%%
