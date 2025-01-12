%{
#include <stdio.h>
#include <ctype.h>
#include <wctype.h>
#include <stdlib.h>
#include <string.h>

#include "ast_bison_helpers.h"
#include "vinumc.h"

int yylex();
%}

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
   ;

args:
	 text {
		struct ast_node node = ast_node_new_nvl(ARGS);

		ast_node_add_child(&node, $1);

		size_t arg_node_id = ast_add_node(&ctx.ast, node);

		args_handle_arg_refs(&ctx.ast, arg_node_id);

		$$ = arg_node_id;
	 }
	 | block {
		struct ast_node node = ast_node_new_nvl(ARGS);

		ast_node_add_child(&node, $1);

		$$ = ast_add_node(&ctx.ast, node);
	 }
	 | args text {
		struct ast_node *node = &VEC_AT(&ctx.ast.nodes, $1);

		ast_node_add_child(node, $2);

		args_handle_arg_refs(&ctx.ast, $1);

		$$ = $1;
	 }
	 | args block {
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
      | block {
	struct ast_node node = ast_node_new_nvl(SYMBOL);

	ast_node_add_child(&node, $1);

	$$ = ast_add_node(&ctx.ast, node);
      }
      ;

text:
     word_text {
	struct ast_node node = ast_node_new_nvl(TEXT);

	ast_node_add_child(&node, $1);

	$$ = ast_add_node(&ctx.ast, node);
    }
    | text word_text {
	struct ast_node *node = &VEC_AT(&ctx.ast.nodes, $1);

	ast_node_add_child(node, $2);

	$$ = $1;
    }
    ;

word_text: WORD { $$ = $1;}
	 | ':'{ $$ = $1;}
	 ;

%%
