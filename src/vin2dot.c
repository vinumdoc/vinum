#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "parser_common.h"

struct ctx ctx;

enum command {
	CMD_AST,
	CMD_AST_AFTER,
	CMD_AST_AND_SCOPES,
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
	fprintf(stderr, "  ast-scopes\tShow the AST linked with the scope tree in the dot format\n");
	fprintf(stderr, "  scopes\tShow the scope tree in the dot format\n");
}

static void dot_ast_and_scopes(struct ast_nodes_t *ast, struct eval_ctx_scopes_t *scopes,
			       FILE *stream) {
	fprintf(stream, "digraph {\n");

	for (size_t i = 0; i < ast->len; i++) {
		const struct ast_node *node = &VEC_AT(ast, i);

		fprintf(stream, "\tan_%zu [label = \"", i);

		if (node->type < 256) {
			fprintf(stream, "\"%c\"", (char)node->type);
		} else {
			fprintf(stream, "%s", token_to_str(node->type));
		}

		if (node->text != NULL) {
			fprintf(stream, "(%s)", node->text);
		}

		fprintf(stream, "\"]\n");

		for (size_t j = 0; j < node->childs.len; j++) {
			size_t child_id = VEC_AT(&node->childs, j);
			fprintf(stream, "\tan_%zu -> an_%zu\n", i, child_id);
		}
	}

	for (size_t i = 0; i < scopes->len; i++) {
		const struct scope *sc = &VEC_AT(scopes, i);

		fprintf(stream, "\ts_%zu [shape=record, label=\" %zu |", i, i);
		for (size_t j = 0; j < sc->namespace.len; j++) {
			struct namespace_entry *entry = &VEC_AT(&sc->namespace, j);
			fprintf(stream, "<%d> %s", entry->ast_node_id, entry->name);
			if (j < sc->namespace.len - 1)
				fprintf(stream, " |");
		}
		fprintf(stream, "\"]\n");

		for (size_t j = 0; j < sc->namespace.len; j++) {
			struct namespace_entry *entry = &VEC_AT(&sc->namespace, j);
			fprintf(stream, "s_%zu:<%d> -> an_%d [style=dotted]", i,
					entry->ast_node_id,
					entry->ast_node_id);
		}

		for (size_t j = 0; j < sc->childs.len; j++) {
			fprintf(stream, "\ts_%zu -> s_%zu\n", i, VEC_AT(&sc->childs, j));
		}

		fprintf(stream, "\tan_%zu -> s_%zu  [style=dashed]\n", (size_t)sc->node, i);
	}

	fprintf(stream, "}\n");
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
 	} else if (!strcmp(cmd_str, "ast-scopes")) {
 		cmd = CMD_AST_AND_SCOPES;
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
		case CMD_AST_AND_SCOPES:
			eval(&ctx.eval_ctx, &ctx.ast, stderr);
			dot_ast_and_scopes(&ctx.ast.nodes, &ctx.eval_ctx.scopes, stdout);
			return EXIT_SUCCESS;
		case CMD_SCOPES:
			eval(&ctx.eval_ctx, &ctx.ast, stderr);
			eval_dot(&ctx.eval_ctx, stdout);
			return EXIT_SUCCESS;
		default:
			assert(0 && "unreachable"); // This should never happen
	}
}
