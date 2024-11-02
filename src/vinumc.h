#ifndef __VINUMC_H__
#define __VINUMC_H__

#include "ast.h"
#include "eval.h"

struct ctx {
	struct ast ast;
	struct eval_ctx eval_ctx;
};

extern struct ctx ctx;

struct ctx ctx_new();

void yyerror(char *s, ...);
int yyparse();

#endif // __VINUMC_H__
