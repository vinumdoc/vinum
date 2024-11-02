#include <string.h>

#include "eval.h"

struct eval_ctx eval_ctx_new() {
	struct eval_ctx ret = { };

	return ret;
}

static struct namespace_entry* namespace_find_name(struct program_ctx_namespace_t *namespace, const char *name) {
	for (size_t i = 0; i < namespace->len; i++) {
		if (strcmp(name, namespace->base[i].name) == 0)
			return &namespace->base[i];
	}

	return NULL;
}

#define RESOLVE_CALLS_FUNC_SIGNATURE(func_name) \
	static void func_name (struct eval_ctx *ctx, struct ast *ast, size_t curr_ctx_id, \
			size_t ast_node_id)

RESOLVE_CALLS_FUNC_SIGNATURE(resolve_calls);

RESOLVE_CALLS_FUNC_SIGNATURE(resolve_calls_program) {
	const struct ast_node *ast_node = &ast->nodes[ast_node_id];
	for (size_t i = 0; i < ast_node->num_childs; i++) {
		resolve_calls(ctx, ast, curr_ctx_id, ast_node->childs[i]);
	}
}

RESOLVE_CALLS_FUNC_SIGNATURE(resolve_calls_assignment) {
	struct program_ctx *curr_ctx = &ctx->program_ctxs.base[curr_ctx_id];
	const struct ast_node *ast_node = &ast->nodes[ast_node_id];

	char *name = ast->nodes[ast_node->childs[0]].text;
	struct namespace_entry entry = {
		.name = name,
		.ast_node_id = ast_node->num_childs > 1 ? (int)ast_node->childs[1] : -1,
	};

	VEC_PUT(&curr_ctx->namespace, entry);
}

RESOLVE_CALLS_FUNC_SIGNATURE(resolve_calls_call) {
	struct program_ctx *curr_ctx = &ctx->program_ctxs.base[curr_ctx_id];
	struct ast_node *ast_node = &ast->nodes[ast_node_id];

	if (ast_node->num_childs > 1) {
		const struct ast_node *args_node = &ast->nodes[ast_node->childs[1]];

		for (size_t i = 0; i < args_node->num_childs; i++) {
			resolve_calls(ctx, ast, curr_ctx_id, args_node->childs[i]);
		}
	}

	char *call_name = ast->nodes[ast_node->childs[0]].text;
	if (call_name == NULL) {
		fprintf(stderr, "ERROR: Symbol with no name\n");
		return;
	}

	struct namespace_entry *symbol_info = namespace_find_name(&curr_ctx->namespace, call_name);

	if (symbol_info != NULL) {
		if (ast_node->num_childs > 1) {
			if (symbol_info->ast_node_id < 0) {
				ast_node->num_childs--;
				return;
			}

			size_t symbol_args_node_id = ast_copy_node(ast, symbol_info->ast_node_id);
			struct ast_node *symbol_args_node = &ast->nodes[symbol_args_node_id];

			for (size_t i = 0; i < symbol_args_node->num_childs; i++) {
				struct ast_node *child = &ast->nodes[symbol_args_node->childs[i]];

				if (child->type == ARG_REF_ALL_ARGS) {
					symbol_args_node->childs[i] = ast_node->childs[1];
				}
			}

			ast_node->childs[1] = symbol_args_node_id;
		} else {
			if (symbol_info->ast_node_id >= 0) {
				ast_node_add_child(ast_node, symbol_info->ast_node_id);
			}
		}
	} else {
		fprintf(stderr, "ERROR: No symbol with name \"%s\" exist\n", call_name);
	}
}

RESOLVE_CALLS_FUNC_SIGNATURE(resolve_calls) {
	const struct ast_node *ast_node = &ast->nodes[ast_node_id];

	switch (ast_node->type) {
		case ARGS:
		case PROGRAM:
			resolve_calls_program(ctx, ast, curr_ctx_id, ast_node_id);
			break;
		case ASSIGNMENT:
			resolve_calls_assignment(ctx, ast, curr_ctx_id, ast_node_id);
			break;
		case CALL:
			resolve_calls_call(ctx, ast, curr_ctx_id, ast_node_id);
			break;
		default:
			break;
	}
}

#define DO_CALLS_FUNC_SIGNATURE(func_name) \
static void func_name (const struct ast *ast, FILE *out, size_t ast_node_id)

DO_CALLS_FUNC_SIGNATURE(do_calls);

DO_CALLS_FUNC_SIGNATURE(do_calls_program) {
	const struct ast_node *ast_node = &ast->nodes[ast_node_id];
	for (size_t i = 0; i < ast_node->num_childs; i++) {
		do_calls(ast, out, ast_node->childs[i]);
	}
}

DO_CALLS_FUNC_SIGNATURE(do_calls_call) {
	const struct ast_node *ast_node = &ast->nodes[ast_node_id];

	if (ast_node->num_childs <= 1) {
		return;
	}

	const struct ast_node *args_node = &ast->nodes[ast_node->childs[1]];

	for (size_t i = 0; i < args_node->num_childs; i++) {
		do_calls(ast, out, args_node->childs[i]);
	}
}

DO_CALLS_FUNC_SIGNATURE(do_calls_text) {
	const struct ast_node *ast_node = &ast->nodes[ast_node_id];

	for (size_t i = 0; i < ast_node->num_childs; i++) {
		const struct ast_node *child = &ast->nodes[ast_node->childs[i]];
		if (child->type != WORD) {
			fprintf(stderr, "ERROR: TEXT node must hold only WORDS\n");
			return;
		}

		fprintf(out, child->text);
		if (i != ast_node->num_childs - 1)
			fprintf(out, " ");
	}
	fprintf(out, "\n");
}

DO_CALLS_FUNC_SIGNATURE(do_calls) {
	const struct ast_node *ast_node = &ast->nodes[ast_node_id];

	switch (ast_node->type) {
		case PROGRAM:
		case ARGS:
			do_calls_program(ast, out, ast_node_id);
			break;
		case CALL:
			do_calls_call(ast, out, ast_node_id);
			break;
		case TEXT:
			do_calls_text(ast, out, ast_node_id);
			break;
		default:
			break;
	}
}

void eval(struct eval_ctx *ctx, struct ast *ast, FILE *out) {
	VEC_PUT(&ctx->program_ctxs, (struct program_ctx){});
	resolve_calls(ctx, ast, 0, 0);
	do_calls(ast, out, 0);
}
