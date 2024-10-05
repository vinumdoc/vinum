#ifndef __VINUMC_H__
#define __VINUMC_H__

#include "ast.h"

struct ctx {
	struct ast ast;
};

extern struct ctx ctx;

struct ctx ctx_new();

void yyerror(char *s, ...);
int yyparse();

#endif // __VINUMC_H__
