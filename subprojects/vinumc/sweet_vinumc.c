#include <stdarg.h>
#include <stdio.h>

#include "sweet_vinumc.h"


void yyerror(char *s, ...) {
	va_list	ap;
	va_start(ap, s);

	fprintf(stderr, "[ERROR]:");
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");

	va_end(ap);
}

int main() {
	yyparse();
}
