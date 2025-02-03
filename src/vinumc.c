#include <stdio.h>
#include <locale.h>
#include <strings.h>

#include "parser_common.h"

struct ctx ctx;

extern FILE *yyin;

int main(int argc, char **argv) {
	setlocale(LC_ALL, "");

	FILE *out = stdout;
	filename = "(stdin)";
	for (int i = 1; i < argc ; i++) {
		char* arg = argv[i];
		if (!strcasecmp("--output", arg)) {
			out = fopen(argv[++i], "w");
		} else {
			yyin = fopen(arg, "r");
			filename = arg;
		}
	}

	ctx = ctx_new();
	yyparse();

	eval(&ctx.eval_ctx, &ctx.ast, out);
}
