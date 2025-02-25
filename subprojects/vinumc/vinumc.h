#ifndef __VINUMC_H__
#define __VINUMC_H__

#include "ast.h"
#include "eval.h"

#include <stdbool.h>

struct ctx {
	char *input_path;
	char *output_path;
	bool show_help;

	struct ast ast;
	struct eval_ctx eval_ctx;
	struct str_vec libraries;
};

extern struct ctx ctx;

struct ctx ctx_new();

void yyerror(char *s, ...);
int yyparse();

#endif // __VINUMC_H__
