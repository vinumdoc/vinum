#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

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

enum command {
	CMD_AST,
	CMD_AST_AFTER,
	CMD_SCOPES,
};

static const char* pop_argv(int *argc, char **argv[]) {
	if (argc == 0)
		return NULL;

	const char *ret = **argv;

	(*argc)--;
	(*argv)++;

	return ret;
}

static void show_usage(const char *prgname) {
	fprintf(stderr, "usage: %s <COMMAND>\n\n", prgname);
	fprintf(stderr, "Commands:\n");
	fprintf(stderr, "  ast\tShow the AST in the dot format\n");
	fprintf(stderr, "  ast-after\tShow the AST after evaluation in the dot format\n");
	fprintf(stderr, "  scopes\tShow the scope tree in the dot format\n");
}
 
int main(int argc, char *argv[]) {
	const char *prgname = pop_argv(&argc, &argv);
	const char *cmd_str = pop_argv(&argc, &argv);

	if (cmd_str == NULL) {
		fprintf(stderr, "ERROR: No command provided\n\n");
		show_usage(prgname);
		return EXIT_FAILURE;
	}

	enum command cmd;

	if (!strcmp(cmd_str, "ast")) {
		cmd = CMD_AST;
	} else if (!strcmp(cmd_str, "ast-after")) {
		cmd = CMD_AST_AFTER;
	} else if (!strcmp(cmd_str, "scopes")) {
		cmd = CMD_SCOPES;
	} else {
		fprintf(stderr, "ERROR: Command \"%s\" is not recognized\n\n", cmd_str);
		show_usage(prgname);
		return EXIT_FAILURE;
	}

	ctx = ctx_new();
	yyparse();

	switch (cmd) {
		case CMD_AST:
			ast_dot(&ctx.ast, stdout);
			return EXIT_SUCCESS;
		case CMD_AST_AFTER:
			eval(&ctx.eval_ctx, &ctx.ast, stderr);
			ast_dot(&ctx.ast, stdout);
			return EXIT_SUCCESS;
		case CMD_SCOPES:
			eval(&ctx.eval_ctx, &ctx.ast, stderr);
			eval_dot(&ctx.eval_ctx, stdout);
			return EXIT_SUCCESS;
		default:
			assert(0 && "unreachable"); // This should never happen
	}
}
