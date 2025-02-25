#include <stdarg.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
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

	ctx = ctx_new();
	FILE *out = stdout;
	for (int i = 1; i < argc ; i++) {
		char *arg = argv[i];
		if (!strcasecmp("--output", arg)) {
			out = fopen(argv[++i], "w");
		} else if (!strcasecmp("--with", arg)) {
			int c = 0;
			while (c + i + 1 < argc && argv[i+c+1][0] != '-'){
				c++;
			}
			ctx.libraries.names = malloc(sizeof(char*) * (c+1));
			
			for (int j = 0; j < c; j++) {
        ctx.libraries.names[j] = strdup(argv[i + 1 + j]);
			}
			ctx.libraries.names[c] = NULL;

			ctx.libraries.counter = c;
			i += c;
		} else {
			yyin = fopen(arg, "r");
		}
	}

	yyparse();

	eval(&ctx.eval_ctx, &ctx.ast, out, &ctx.libraries);
}
