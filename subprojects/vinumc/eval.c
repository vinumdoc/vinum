#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eval.h"
#include "extern_library.h"
#include "library_loader.h"
#include "str.h"
#include "utils.h"
#include "v_lib.h"
#include "vec.h"

enum do_calls_flag {
	REDUCE_BLANKS = 1 << 0,
	FIRST_CHILD = 1 << 1,
	LAST_CHILD = 1 << 2,
};

struct eval_ctx eval_ctx_new() {
	struct eval_ctx ret = {};

	return ret;
}

static size_t add_scope_child(struct eval_ctx_scopes_t *scope_array, size_t scope_id,
			      ast_node_id_t node) {
	size_t new_scope_id = scope_array->len;

	VEC_PUT(scope_array, ((struct scope){ .father = scope_id, node = node }));
	struct scope *scope = &scope_array->base[scope_id];
	VEC_PUT(&scope->childs, new_scope_id);

	return new_scope_id;
}

static struct namespace_entry *namespace_find_name(const struct scope_namespace_t *namespace,
						   const char *name) {
	for (size_t i = 0; i < namespace->len; i++) {
		if (strcmp(name, VEC_AT(namespace, i).name) == 0)
			return &VEC_AT(namespace, i);
	}

	return NULL;
}

static struct namespace_entry *find_symbol_on_scopes(const struct eval_ctx_scopes_t *scope_array,
						     const struct scope *scope, const char *name) {
	while (scope != NULL) {
		struct namespace_entry *entry = namespace_find_name(&scope->namespace, name);

		if (entry != NULL)
			return entry;

		scope = scope->father == -1 ? NULL : &scope_array->base[scope->father];
	}

	return NULL;
}

static int find_scope_child_by_node(const struct eval_ctx_scopes_t *scopes, size_t scope_id,
				    size_t ast_node_id) {
	const struct scope *curr_scope = &VEC_AT(scopes, scope_id);
	int call_scope = -1;

	for (size_t i = 0; i < curr_scope->childs.len; i++) {
		size_t tmp_scope_id = VEC_AT(&curr_scope->childs, i);
		struct scope *tmp_scope = &VEC_AT(scopes, tmp_scope_id);

		if (tmp_scope->node == ast_node_id)
			call_scope = tmp_scope_id;
	}

	return call_scope;
}

#define RESOLVE_FUNC_SIGNATURE(func_name)                                                          \
	static void func_name(struct eval_ctx *ctx, struct ast *ast, size_t curr_scope_id,         \
			      size_t ast_node_id)

RESOLVE_FUNC_SIGNATURE(resolve_symbols);

RESOLVE_FUNC_SIGNATURE(resolve_symbols_descent) {
	struct ast_node ast_node = VEC_AT(&ast->nodes, ast_node_id);
	for (size_t i = 0; i < ast_node.childs.len; i++) {
		resolve_symbols(ctx, ast, curr_scope_id, VEC_AT(&ast_node.childs, i));
	}
}

RESOLVE_FUNC_SIGNATURE(resolve_symbols_assignment) {
	struct scope *curr_scope = &VEC_AT(&ctx->scopes, curr_scope_id);
	const struct ast_node *ast_node = &VEC_AT(&ast->nodes, ast_node_id);

	char *name = VEC_AT(&ast->nodes, VEC_AT(&ast_node->childs, 0)).text;
	struct namespace_entry entry = {
		.name = name,
		.type = ENTRY_INTERNAL,
		.as.ast_node_id = ast_node->childs.len > 1 ? (int)VEC_AT(&ast_node->childs, 1) : -1,
	};

	VEC_PUT(&curr_scope->namespace, entry);
}

RESOLVE_FUNC_SIGNATURE(resolve_symbols) {
	const struct ast_node *ast_node = &VEC_AT(&ast->nodes, ast_node_id);

	if (ast_node->type == ASSIGNMENT) {
		resolve_symbols_assignment(ctx, ast, curr_scope_id, ast_node_id);
	} else {
		if (ast_node->type == CALL)
			curr_scope_id = add_scope_child(&ctx->scopes, curr_scope_id, ast_node_id);
		resolve_symbols_descent(ctx, ast, curr_scope_id, ast_node_id);
	}
}

RESOLVE_FUNC_SIGNATURE(resolve_calls);

RESOLVE_FUNC_SIGNATURE(resolve_calls_descent) {
	const struct ast_node ast_node = VEC_AT(&ast->nodes, ast_node_id);
	for (size_t i = 0; i < ast_node.childs.len; i++) {
		resolve_calls(ctx, ast, curr_scope_id, VEC_AT(&ast_node.childs, i));
	}
}

RESOLVE_FUNC_SIGNATURE(resolve_calls_call) {
	struct scope *curr_scope = &VEC_AT(&ctx->scopes, curr_scope_id);
	struct ast_node *ast_node = &VEC_AT(&ast->nodes, ast_node_id);

	char *call_name = VEC_AT(&ast->nodes, VEC_AT(&ast_node->childs, 0)).text;
	if (call_name == NULL) {
		fprintf(stderr, "ERROR: Symbol with no name\n");
		return;
	}

	struct namespace_entry *symbol_info =
		find_symbol_on_scopes(&ctx->scopes, curr_scope, call_name);

	if (symbol_info != NULL) {
		if (symbol_info->type == ENTRY_INTERNAL) {
			if (ast_node->childs.len > 1) {
				if (symbol_info->as.ast_node_id < 0) {
					ast_node->childs.len--;
					return;
				}

				size_t symbol_args_node_id =
					ast_copy_node(ast, symbol_info->as.ast_node_id);
				struct ast_node *symbol_args_node =
					&VEC_AT(&ast->nodes, symbol_args_node_id);

				for (size_t i = 0; i < symbol_args_node->childs.len; i++) {
					struct ast_node *child = &VEC_AT(
						&ast->nodes, VEC_AT(&symbol_args_node->childs, i));

					if (child->type == ARG_REF_ALL_ARGS) {
						VEC_AT(&symbol_args_node->childs, i) =
							VEC_AT(&ast_node->childs, 1);
					}
				}
				VEC_AT(&ast_node->childs, 1) = symbol_args_node_id;
			} else {
				if (symbol_info->as.ast_node_id >= 0) {
					ast_node_add_child(ast_node, symbol_info->as.ast_node_id);
				}
			}

			size_t new_node = VEC_AT(&ast_node->childs, 1);

			resolve_symbols(ctx, ast, curr_scope_id, new_node);

		} else if (symbol_info->type == ENTRY_EXTERNAL) {
			struct ast_node *symbol_node =
				&VEC_AT(&ast->nodes, VEC_AT(&ast_node->childs, 0));
			symbol_node->type = FUNCTION;

			if (ast_node->childs.len <= 1) {
				// ensure that the call node has an ARGS node
				// to prevent it from being skipped during evaluation
				size_t args_node_id = ast_add_node(ast, ast_node_new_nvl(ARGS));
				ast_node_add_child(ast_node, args_node_id);
			}
		}
	} else {
		fprintf(stderr, "ERROR: No symbol with name \"%s\" exist\n", call_name);
	}

	resolve_calls_descent(ctx, ast, curr_scope_id, ast_node_id);
}

RESOLVE_FUNC_SIGNATURE(resolve_calls) {
	const struct ast_node *ast_node = &VEC_AT(&ast->nodes, ast_node_id);

	switch (ast_node->type) {
	case ARGS:
	case PROGRAM:
		resolve_calls_descent(ctx, ast, curr_scope_id, ast_node_id);
		break;
	case CALL:;
		int new_scope = find_scope_child_by_node(&ctx->scopes, curr_scope_id, ast_node_id);
		if (new_scope > 0)
			curr_scope_id = new_scope;
		else
			fprintf(stderr, "ERROR: Could not find call scope for node %zu\n",
				ast_node_id);
		resolve_calls_call(ctx, ast, curr_scope_id, ast_node_id);
		break;
	default:
		break;
	}
}

#define DO_CALLS_FUNC_SIGNATURE(func_name)                                                         \
	static void func_name(struct eval_ctx *ctx, const struct ast *ast, struct str *out,        \
			      size_t ast_node_id, int flags)

DO_CALLS_FUNC_SIGNATURE(do_calls);

DO_CALLS_FUNC_SIGNATURE(do_calls_program) {
	const struct ast_node *ast_node = &VEC_AT(&ast->nodes, ast_node_id);
	for (size_t i = 0; i < ast_node->childs.len; i++) {
		size_t child_id = VEC_AT(&ast_node->childs, i);
		int sub_flags = flags & ~(FIRST_CHILD | LAST_CHILD);
		if (i == 0) {
			sub_flags |= FIRST_CHILD;
		}
		if (i == ast_node->childs.len - 1) {
			sub_flags |= LAST_CHILD;
		}
		do_calls(ctx, ast, out, child_id, sub_flags);
	}
}

DO_CALLS_FUNC_SIGNATURE(do_calls_call) {
	const struct ast_node *ast_node = &VEC_AT(&ast->nodes, ast_node_id);

	if (ast_node->childs.len <= 1) {
		return;
	}

	const struct ast_node *symbol_node = &VEC_AT(&ast->nodes, VEC_AT(&ast_node->childs, 0));
	size_t args_id = VEC_AT(&ast_node->childs, 1);

	if (symbol_node->type == FUNCTION) {
		// writes the returns of the arguments calls to a temporary buffer,
		// so any nested call will be resolved normally
		struct str tmp_out = {};
		VEC_PUT(&tmp_out, '\0');
		do_calls(ctx, ast, &tmp_out, args_id, flags);

		// find the extern function on the scope
		struct scope *curr_scope = &VEC_AT(&ctx->scopes, 0);
		struct namespace_entry *symbol_info =
			find_symbol_on_scopes(&ctx->scopes, curr_scope, symbol_node->text);

		// expose context to external function
		struct _call_ctx cctx = { .text = tmp_out.base };
		struct return_value call_return = symbol_info->as.func(&cctx);

		// put the extern function call return on the out str

		if ((flags & REDUCE_BLANKS) != 0) {
			put_blank_reduced_str(out, call_return.ptr, true, true);
		} else {
			put_str(out, call_return.ptr);
		}

		if (call_return.free) {
			free(call_return.ptr);
		}
		VEC_FREE(&tmp_out);
	} else {
		do_calls(ctx, ast, out, args_id, flags);
	}
}

DO_CALLS_FUNC_SIGNATURE(do_calls_text) {
	UNUSED(ctx);
	const struct ast_node *ast_node = &VEC_AT(&ast->nodes, ast_node_id);

	if ((flags & REDUCE_BLANKS) != 0) {
		bool trim_left = (flags & FIRST_CHILD) != 0;
		bool trim_right = (flags & LAST_CHILD) != 0;
		put_blank_reduced_str(out, ast_node->text, trim_left, trim_right);
	} else {
		put_str(out, ast_node->text);
	}
}

DO_CALLS_FUNC_SIGNATURE(do_calls) {
	const struct ast_node *ast_node = &VEC_AT(&ast->nodes, ast_node_id);

	switch (ast_node->type) {
	case PROGRAM:
	case ARGS:
		do_calls_program(ctx, ast, out, ast_node_id, flags);
		break;
	case CALL:
		do_calls_call(ctx, ast, out, ast_node_id, flags);
		break;
	case TEXT:
		do_calls_text(ctx, ast, out, ast_node_id, flags);
		break;
	default:
		break;
	}
}

void resolve_extern_functions(struct eval_ctx *ctx, struct loaded_lib lib) {
	struct scope *curr_scope = &VEC_AT(&ctx->scopes, 0);

	int i = 0;
	struct extern_function f = lib.functions[i];
	while (f.name != 0) {
		struct namespace_entry entry = {
			.name = f.name,
			.type = ENTRY_EXTERNAL,
			.as.func = f.fp,
		};

		VEC_PUT(&curr_scope->namespace, entry);
		f = lib.functions[++i];
	}
}

struct loaded_lib *load_libs(struct eval_ctx *ctx, struct str_vec *libraries) {
	size_t len = libraries->len;
	struct loaded_lib *loaded_libs = calloc(len, sizeof(struct loaded_lib));
	for (size_t i = 0; i < len; i++) {
		loaded_libs[i] = load_lib(VEC_AT(libraries, i));
		resolve_extern_functions(ctx, loaded_libs[i]);
	}
	loaded_libs[len].dl_handle = NULL;
	return loaded_libs;
}

void unload_libs(struct loaded_lib *loaded_libs) {
	if (loaded_libs == NULL) {
		return;
	}
	int i = 0;
	while (loaded_libs[i].dl_handle != NULL) {
		unload_lib(loaded_libs[i]);
		i++;
	}
	free(loaded_libs);
}

void eval(struct eval_ctx *ctx, struct ast *ast, FILE *out, struct str_vec *libraries) {
	struct scope base_scope = {
		.father = -1,
		.node = 0,
	};
	VEC_PUT(&ctx->scopes, base_scope);

	struct loaded_lib *loaded_libs = load_libs(ctx, libraries);
	resolve_symbols(ctx, ast, 0, 0);
	resolve_calls(ctx, ast, 0, 0);
	struct str str_out = {};
	VEC_PUT(&str_out, '\0');

	do_calls(ctx, ast, &str_out, 0, REDUCE_BLANKS);
	fprintf(out, "%s", str_out.base);
	unload_libs(loaded_libs);
}

void eval_dot(const struct eval_ctx *ctx, FILE *stream) {
	fprintf(stream, "digraph {\n");
	fprintf(stream, "\tnode [shape=record];\n");
	for (size_t i = 0; i < ctx->scopes.len; i++) {
		const struct scope *sc = &VEC_AT(&ctx->scopes, i);
		fprintf(stream, "\t%zu [label=\" %zu |", i, i);
		for (size_t j = 0; j < sc->namespace.len; j++) {
			fprintf(stream, "%s", VEC_AT(&sc->namespace, j).name);
			if (j < sc->namespace.len - 1)
				fprintf(stream, " |");
		}
		fprintf(stream, "\"]\n");

		for (size_t j = 0; j < sc->childs.len; j++) {
			fprintf(stream, "\t%zu -> %zu\n", i, VEC_AT(&sc->childs, j));
		}
	}
	fprintf(stream, "}\n");
}
