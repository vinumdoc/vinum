#ifndef __PARSER_UTILS_H__
#define __PARSER_UTILS_H__

#include "eval.h"

struct ctx {
	struct ast ast;
	struct eval_ctx eval_ctx;
};

extern struct ctx ctx;

struct ctx ctx_new();

void yyerror(char *s, ...);
void lyyerror(YYLTYPE t, char *s, ...);
int yyparse();

#endif // __PARSER_UTILS_H__
