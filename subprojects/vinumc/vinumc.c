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
		if (!strcasecmp("--output", arg)) {
			char *file_path = argv[++i];
			ctx.output_path = file_path;
		} else {
			ctx.input_path = arg;
		}
	}

	FILE *out = stdout;
	if (ctx.output_path != NULL)
		out = fopen(ctx.output_path, "w");

	if (ctx.input_path != NULL)
		yyin = fopen(ctx.input_path, "r");

	ctx = ctx_new();
	yyparse();

	eval(&ctx.eval_ctx, &ctx.ast, out);
}
