%{
#include <stdio.h>

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
