#include <stdarg.h>
#include <stdio.h>
#include <locale.h>
#include <strings.h>

#include "vinumc.h"

struct ctx ctx;

struct ctx ctx_new() {
	struct ctx ret = {
		.ast = ast_new(),
		.eval_ctx = eval_ctx_new(),
		.libraries = {0, NULL},
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

	FILE *out = stdout;
	for (int i = 1; i < argc ; i++) {
		char* arg = argv[i];
		if (!strcasecmp("--output", arg)) {
			out = fopen(argv[++i], "w");
		} else {
			yyin = fopen(arg, "r");
		}
	}

	ctx = ctx_new();
	yyparse();

	eval(&ctx.eval_ctx, &ctx.ast, out, &ctx.libraries);
}
