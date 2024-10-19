#include <stdarg.h>
#include <stdio.h>

#include "dry_bison.h"

#include "vinumc.h"

struct ctx ctx;

struct ctx ctx_new() {
	struct ctx ret = {
		.ast = ast_new(),
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

int main() {
	ctx = ctx_new();
	yyparse();
	ast_print(&ctx.ast);
}
