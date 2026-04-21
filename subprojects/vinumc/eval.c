#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vutils/str.h>
#include <vutils/system_allocator.h>
#include <vutils/vec.h>

#include "eval.h"
#include "extern_library.h"
#include "library_loader.h"
#include "utils.h"
#include "v_lib.h"

enum do_calls_flag {
	REDUCE_BLANKS = 1 << 0,
	FIRST_CHILD = 1 << 1,
	LAST_CHILD = 1 << 2,
};

struct eval_ctx eval_ctx_new(struct vut_allocator *allocator) {
	struct eval_ctx ret = {
		.allocator = allocator,
		.scopes = VUT_VEC_INIT(struct eval_ctx_scopes_t, allocator),
	};

	return ret;
}

static struct scope scope_new(ast_node_id_t node, int father, struct vut_allocator *allocator) {
	struct scope new_scope = {
		.father = father,
		.node = node,
		.childs = VUT_VEC_INIT(struct scope_childs_t, allocator),
		.namespace = VUT_VEC_INIT(struct scope_namespace_t, allocator),
	};
	return new_scope;
}

static size_t add_scope_child(struct eval_ctx_scopes_t *scope_array, size_t scope_id,
			      ast_node_id_t node, struct vut_allocator *allocator) {
	size_t new_scope_id = scope_array->len;
	struct scope new_scope = scope_new(node, scope_id, allocator);

	VUT_VEC_PUT(scope_array, new_scope);
	struct scope *scope = &scope_array->base[scope_id];
	VUT_VEC_PUT(&scope->childs, new_scope_id);

	return new_scope_id;
}

static struct namespace_entry *namespace_find_name(const struct scope_namespace_t *namespace,
						   const char *name) {
	for (size_t i = 0; i < namespace->len; i++) {
		if (strcmp(name, VUT_VEC_AT(namespace, i).name) == 0)
			return &VUT_VEC_AT(namespace, i);
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
	const struct scope *curr_scope = &VUT_VEC_AT(scopes, scope_id);
	int call_scope = -1;

	for (size_t i = 0; i < curr_scope->childs.len; i++) {
		size_t tmp_scope_id = VUT_VEC_AT(&curr_scope->childs, i);
		struct scope *tmp_scope = &VUT_VEC_AT(scopes, tmp_scope_id);

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
	for (size_t i = 0; i < ast_get_num_child(ast, ast_node_id); i++) {
		resolve_symbols(ctx, ast, curr_scope_id, ast_get_nth_child(ast, ast_node_id, i));
	}
}

RESOLVE_FUNC_SIGNATURE(resolve_symbols_assignment) {
	struct scope *curr_scope = &VUT_VEC_AT(&ctx->scopes, curr_scope_id);

	char *name = ast_get_text(ast, ast_get_nth_child(ast, ast_node_id, 0));
	struct namespace_entry entry = {
		.name = name,
		.type = ENTRY_INTERNAL,
		.as.ast_node_id = ast_get_num_child(ast, ast_node_id) > 1
					  ? (int)ast_get_nth_child(ast, ast_node_id, 1)
					  : -1,
	};

	VUT_VEC_PUT(&curr_scope->namespace, entry);
}

RESOLVE_FUNC_SIGNATURE(resolve_symbols) {
	if (ast_get_type(ast, ast_node_id) == ASSIGNMENT) {
		resolve_symbols_assignment(ctx, ast, curr_scope_id, ast_node_id);
	} else {
		if (ast_get_type(ast, ast_node_id) == CALL)
			curr_scope_id = add_scope_child(&ctx->scopes, curr_scope_id, ast_node_id,
							ctx->allocator);
		resolve_symbols_descent(ctx, ast, curr_scope_id, ast_node_id);
	}
}

RESOLVE_FUNC_SIGNATURE(resolve_calls);

RESOLVE_FUNC_SIGNATURE(resolve_calls_descent) {
	for (size_t i = 0; i < ast_get_num_child(ast, ast_node_id); i++) {
		resolve_calls(ctx, ast, curr_scope_id, ast_get_nth_child(ast, ast_node_id, i));
	}
}

RESOLVE_FUNC_SIGNATURE(resolve_calls_call) {
	struct scope *curr_scope = &VUT_VEC_AT(&ctx->scopes, curr_scope_id);

	char *call_name = ast_get_text(ast, ast_get_nth_child(ast, ast_node_id, 0));
	if (call_name == NULL) {
		fprintf(stderr, "ERROR: Symbol with no name\n");
		return;
	}

	struct namespace_entry *symbol_info =
		find_symbol_on_scopes(&ctx->scopes, curr_scope, call_name);

	if (symbol_info != NULL) {
		if (symbol_info->type == ENTRY_INTERNAL) {
			if (ast_get_num_child(ast, ast_node_id) > 1) {
				if (symbol_info->as.ast_node_id < 0) {
					struct ast_node *node =
						&VUT_VEC_AT(&ast->nodes, ast_node_id);
					node->childs.len--;
					return;
				}

				ast_node_id_t symbol_args =
					ast_copy_node(ast, symbol_info->as.ast_node_id);

				ast_node_id_t all_args = ast_get_nth_child(ast, ast_node_id, 1);
				for (size_t i = 0; i < ast_get_num_child(ast, symbol_args); i++) {
					ast_node_id_t child =
						ast_get_nth_child(ast, symbol_args, i);

					if (ast_get_type(ast, child) == ARG_REF_ALL_ARGS) {
						ast_set_nth_child(ast, symbol_args, i, all_args);
					}
				}
				ast_set_nth_child(ast, ast_node_id, 1, symbol_args);
			} else {
				if (symbol_info->as.ast_node_id >= 0) {
					ast_add_child(ast, ast_node_id,
						      symbol_info->as.ast_node_id);
				}
			}

			ast_node_id_t new_node = ast_get_nth_child(ast, ast_node_id, 1);

			resolve_symbols(ctx, ast, curr_scope_id, new_node);
		} else if (symbol_info->type == ENTRY_EXTERNAL) {
			ast_node_id_t symbol = ast_get_nth_child(ast, ast_node_id, 0);
			ast_set_type(ast, symbol, FUNCTION);

			if (ast_get_num_child(ast, ast_node_id) <= 1) {
				// ensure that the call node has an ARGS node
				// to prevent it from being skipped during evaluation
				size_t args_node_id =
					ast_add_node(ast, ast_node_new_nvl(ARGS, ast->allocator));
				ast_add_child(ast, ast_node_id, args_node_id);
			}
		}
	} else {
		fprintf(stderr, "ERROR: No symbol with name \"%s\" exist\n", call_name);
	}

	resolve_calls_descent(ctx, ast, curr_scope_id, ast_node_id);
}

RESOLVE_FUNC_SIGNATURE(resolve_calls) {
	switch (ast_get_type(ast, ast_node_id)) {
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
	static void func_name(struct eval_ctx *ctx, const struct ast *ast, struct vut_str *out,    \
			      size_t ast_node_id, int flags)

DO_CALLS_FUNC_SIGNATURE(do_calls);

DO_CALLS_FUNC_SIGNATURE(do_calls_program) {
	for (size_t i = 0; i < ast_get_num_child(ast, ast_node_id); i++) {
		size_t child_id = ast_get_nth_child(ast, ast_node_id, i);
		int sub_flags = flags & ~(FIRST_CHILD | LAST_CHILD);
		if (i == 0) {
			sub_flags |= FIRST_CHILD;
		}
		if (i == ast_get_num_child(ast, ast_node_id) - 1) {
			sub_flags |= LAST_CHILD;
		}
		do_calls(ctx, ast, out, child_id, sub_flags);
	}
}

DO_CALLS_FUNC_SIGNATURE(do_calls_call) {
	if (ast_get_num_child(ast, ast_node_id) <= 1) {
		return;
	}

	ast_node_id_t symbol = ast_get_nth_child(ast, ast_node_id, 0);
	ast_node_id_t args = ast_get_nth_child(ast, ast_node_id, 1);

	if (ast_get_type(ast, symbol) == FUNCTION) {
		// writes the returns of the arguments calls to a temporary buffer,
		// so any nested call will be resolved normally
		struct vut_str tmp_out = vut_str_init(ctx->allocator);
		VUT_VEC_PUT(&tmp_out, '\0');
		do_calls(ctx, ast, &tmp_out, args, flags);

		// find the extern function on the scope
		struct scope *curr_scope = &VUT_VEC_AT(&ctx->scopes, 0);
		struct namespace_entry *symbol_info =
			find_symbol_on_scopes(&ctx->scopes, curr_scope, ast_get_text(ast, symbol));

		// expose context to external function
		struct _call_ctx cctx = { .text = tmp_out.base };
		struct return_value call_return = symbol_info->as.func(&cctx);

		// put the extern function call return on the out str

		if ((flags & REDUCE_BLANKS) != 0) {
			vut_put_blank_reduced_str(out, call_return.ptr, true, true);
		} else {
			vut_put_str(out, call_return.ptr);
		}

		if (call_return.free) {
			free(call_return.ptr);
		}
		VUT_VEC_FREE(&tmp_out);
	} else {
		do_calls(ctx, ast, out, args, flags);
	}
}

DO_CALLS_FUNC_SIGNATURE(do_calls_text) {
	UNUSED(ctx);
	char *text = ast_get_text(ast, ast_node_id);

	if ((flags & REDUCE_BLANKS) != 0) {
		bool trim_left = (flags & FIRST_CHILD) != 0;
		bool trim_right = (flags & LAST_CHILD) != 0;
		vut_put_blank_reduced_str(out, text, trim_left, trim_right);
	} else {
		vut_put_str(out, text);
	}
}

DO_CALLS_FUNC_SIGNATURE(do_calls) {
	switch (ast_get_type(ast, ast_node_id)) {
	case PROGRAM:
	case ARGS:
		do_calls_program(ctx, ast, out, ast_node_id, flags);
		break;
	case CALL:
		do_calls_call(ctx, ast, out, ast_node_id, flags);
		break;
	case TEXT:
	case LITERAL:
		do_calls_text(ctx, ast, out, ast_node_id, flags);
		break;
	default:
		break;
	}
}

void resolve_extern_functions(struct eval_ctx *ctx, struct loaded_lib lib) {
	struct scope *curr_scope = &VUT_VEC_AT(&ctx->scopes, 0);

	int i = 0;
	struct extern_function f = lib.functions[i];
	while (f.name != 0) {
		struct namespace_entry entry = {
			.name = f.name,
			.type = ENTRY_EXTERNAL,
			.as.func = f.fp,
		};

		VUT_VEC_PUT(&curr_scope->namespace, entry);
		f = lib.functions[++i];
	}
}

struct loaded_lib *load_libs(struct eval_ctx *ctx, struct str_vec *libraries) {
	size_t len = libraries->len;
	if (len == 0)
		return NULL;
	struct loaded_lib *loaded_libs =
		vut_allocator_calloc(ctx->allocator, sizeof(struct loaded_lib), len + 1);
	for (size_t i = 0; i < len; i++) {
		loaded_libs[i] = load_lib(VUT_VEC_AT(libraries, i));
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
	struct scope base_scope = scope_new(0, -1, ctx->allocator);
	VUT_VEC_PUT(&ctx->scopes, base_scope);

	struct loaded_lib *loaded_libs = load_libs(ctx, libraries);
	resolve_symbols(ctx, ast, 0, 0);
	resolve_calls(ctx, ast, 0, 0);

	struct vut_str str_out = vut_str_init(ctx->allocator);
	VUT_VEC_PUT(&str_out, '\0');

	do_calls(ctx, ast, &str_out, 0, REDUCE_BLANKS);
	fprintf(out, "%s", str_out.base);
	unload_libs(loaded_libs);
}

void eval_dot(const struct eval_ctx *ctx, FILE *stream) {
	fprintf(stream, "digraph {\n");
	fprintf(stream, "\tnode [shape=record];\n");
	for (size_t i = 0; i < ctx->scopes.len; i++) {
		const struct scope *sc = &VUT_VEC_AT(&ctx->scopes, i);
		fprintf(stream, "\t%zu [label=\" %zu |", i, i);
		for (size_t j = 0; j < sc->namespace.len; j++) {
			fprintf(stream, "%s", VUT_VEC_AT(&sc->namespace, j).name);
			if (j < sc->namespace.len - 1)
				fprintf(stream, " |");
		}
		fprintf(stream, "\"]\n");

		for (size_t j = 0; j < sc->childs.len; j++) {
			fprintf(stream, "\t%zu -> %zu\n", i, VUT_VEC_AT(&sc->childs, j));
		}
	}
	fprintf(stream, "}\n");
}
