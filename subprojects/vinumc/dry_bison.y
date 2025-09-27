%{
#include <stdio.h>
#include <ctype.h>
#include <wctype.h>
#include <stdlib.h>
#include <string.h>

#include <vutils/system_allocator.h>

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
	struct ast_node *node = &VUT_VEC_AT(&ctx.ast.nodes, $1);

	ast_node_add_child(node, $2);
	$$ = $1;
       }
       ;

block:
     '[' symbol ':' args ']'  {
	struct ast_node node = ast_node_new_nvl(ASSIGNMENT, ctx.ast.allocator);

	ast_node_add_child(&node, $2);
	ast_node_add_child(&node, $4);

	$$ = ast_add_node(&ctx.ast, node);
   }
   | '[' symbol args ']'  {
	struct ast_node node = ast_node_new_nvl(CALL, ctx.ast.allocator);

	ast_node_add_child(&node, $2);
	ast_node_add_child(&node, $3);

	$$ = ast_add_node(&ctx.ast, node);
   }
   | '[' symbol ']'  {
	struct ast_node node = ast_node_new_nvl(CALL, ctx.ast.allocator);

	ast_node_add_child(&node, $2);

	$$ = ast_add_node(&ctx.ast, node);
   }
   ;

args:
	args_child {
		struct ast_node node = ast_node_new_nvl(ARGS, ctx.ast.allocator);
		ast_node_add_child(&node, $1);

		$$ = ast_add_node(&ctx.ast, node);
	}
	| args args_child {
		struct ast_node *node = &VUT_VEC_AT(&ctx.ast.nodes, $1);

		ast_node_add_child(node, $2);

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
	struct ast_node node = ast_node_new_nvl(SYMBOL, ctx.ast.allocator);

	ast_node_add_child(&node, $1);

	$$ = ast_add_node(&ctx.ast, node);
      }
      ;
%%
