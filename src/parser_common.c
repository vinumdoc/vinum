#include "parser_common.h"
#include <stdarg.h>

void yyerror(char *s, ...) {
	va_list	ap;
	va_start(ap, s);

	fprintf(stderr, "[ERROR]:");
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");

	va_end(ap);
}

void lyyerror(YYLTYPE t, char *s, ...) {
	// TODO: handle file
	va_list ap;
	va_start(ap, s);
	fprintf(stderr, "%s:%d:%d: ", t.filename, t.first_line, t.first_column);
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");

	va_end(ap);
}

struct ctx ctx_new() {
	struct ctx ret = {
		.ast = ast_new(),
		.eval_ctx = eval_ctx_new(),
	};

	return ret;
}

