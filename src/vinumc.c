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
	if (argc > 1) {
		yyin = fopen(argv[1], "r");
	}

	ctx = ctx_new();
	yyparse();
	ast_print(&ctx.ast);

	FILE *out = fopen("out.txt", "w");
	eval(&ctx.eval_ctx, &ctx.ast, out);
	ast_print(&ctx.ast);
}
