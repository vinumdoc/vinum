#include "parser_common.h"
#include <stdarg.h>
#include <string.h>

void yyerror(char *s, ...) {
	va_list	ap;
	va_start(ap, s);

	// commented to not print the default "syntax error"
	// fprintf(stderr, "[ERROR]: ");
	// vfprintf(stderr, s, ap);
	// fprintf(stderr, "\n");

	va_end(ap);
}

char* file_get_line(char* filename, int line_number){
	FILE* f = fopen(filename, "r");
	if (f == NULL) {
		return NULL;
	}
	char* buffer = malloc(1024);
	int curr_line = 1;
	while (fgets(buffer, 1024, f) != NULL){
		if (curr_line == line_number){
			fclose(f);
			return buffer;
		}
		curr_line++;
	}

	return NULL;
}

void lyyerror(YYLTYPE t, char *s, ...) {
	va_list ap;
	va_start(ap, s);
	fprintf(stderr, "%s:%d:%d: ", t.filename, t.first_line, t.first_column);
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");

	if (strcmp("(stdin)", filename)){
		char* line = file_get_line(filename, t.first_line);
		if (line != NULL){
			fprintf(stderr, " | %s | ", line);
			for (int i = 1; i <= t.last_column; i++){
				if (i < t.first_column){
					fprintf(stderr, ".");
				} else {
					fprintf(stderr, "^");
				}
			}
			fprintf(stderr, "\n");
			free(line);
		}
	}

	va_end(ap);
}

struct ctx ctx_new() {
	struct ctx ret = {
		.ast = ast_new(),
		.eval_ctx = eval_ctx_new(),
	};

	return ret;
}

