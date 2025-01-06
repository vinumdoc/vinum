#include <stdarg.h>
#include <stdio.h>
#include <locale.h>

#include "vinumc.h"

struct ctx ctx;

struct ctx ctx_new() {
	struct ctx ret = {
		.ast = ast_new(),
		.eval_ctx = eval_ctx_new(),
	};

	return ret;
}

void yyerror(char *s, ...) {
	va_list	ap;
	va_start(ap, s);

	fprintf(stderr, "[ERROR]:");
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");

	va_end(ap);
}

extern FILE *yyin;

int main(int argc, char **argv) {
	setlocale(LC_ALL, "");

	for (int i = 1; i < argc ; i++) {
		char* arg = argv[i];
		yyin = fopen(arg, "r");
	}

	ctx = ctx_new();
	yyparse();

	eval(&ctx.eval_ctx, &ctx.ast, stdout);
}
