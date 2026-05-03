#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vutils/arena_allocator.h>
#include <vutils/str.h>
#include <vutils/system_allocator.h>
#include <vutils/vec.h>

#include "ast.h"
#include "eval.h"
#include "extern_library.h"
#include "library_loader.h"
#include "libvinumc.h"
#include "utils.h"
#include "v_lib.h"

enum do_calls_flag {
	REDUCE_BLANKS = 1 << 0,
	FIRST_CHILD = 1 << 1,
	LAST_CHILD = 1 << 2,
};

struct eval_ctx eval_ctx_new(struct vut_allocator allocator) {
	struct eval_ctx ret = {
		.allocator = allocator,
		.scopes = VUT_VEC_INIT(struct eval_ctx_scopes_t, allocator),
	};

	struct vut_arena *arena = vut_allocator_malloc(allocator, sizeof(*arena), 1);
	*arena = vut_arena_new(allocator, 10 * 1024);
	ret.scopes_childs_arena = vut_arena_to_vut_allocator(arena);

	arena = vut_allocator_malloc(allocator, sizeof(*arena), 1);
	*arena = vut_arena_new(allocator, 10 * 1024);
	ret.scopes_namespace_arena = vut_arena_to_vut_allocator(arena);

	return ret;
}

void eval_ctx_free(struct eval_ctx *ctx) {
	vut_arena_free_all(ctx->scopes_namespace_arena.base_allocator);
	vut_allocator_free(ctx->allocator, ctx->scopes_namespace_arena.base_allocator);

	vut_arena_free_all(ctx->scopes_childs_arena.base_allocator);
	vut_allocator_free(ctx->allocator, ctx->scopes_childs_arena.base_allocator);

	VUT_VEC_FREE(&ctx->scopes);

	*ctx = (struct eval_ctx){ 0 };
}

static struct scope scope_new(struct eval_ctx *ctx, ast_node_id_t node, int father) {
	struct scope new_scope = {
		.father = father,
		.node = node,
		.childs = VUT_VEC_INIT(struct scope_childs_t, ctx->scopes_childs_arena),
		.namespace = VUT_VEC_INIT(struct scope_namespace_t, ctx->scopes_namespace_arena),
	};
	return new_scope;
}

static size_t add_scope_child(struct eval_ctx *ctx, size_t father, ast_node_id_t node) {
	size_t new_scope_id = ctx->scopes.len;
	struct scope new_scope = scope_new(ctx, node, father);

	VUT_VEC_PUT(&ctx->scopes, new_scope);
	struct scope *scope = &ctx->scopes.base[father];
	VUT_VEC_PUT(&scope->childs, new_scope_id);

	return new_scope_id;
}

static struct namespace_entry *namespace_find_name(const struct scope_namespace_t *namespace,
						   const struct vut_sv name) {
	for (size_t i = 0; i < namespace->len; i++) {
		if (vut_sv_eq(name, VUT_VEC_AT(namespace, i).name))
			return &VUT_VEC_AT(namespace, i);
	}

	return NULL;
}

static struct namespace_entry *find_symbol_on_scopes(const struct eval_ctx_scopes_t *scope_array,
						     const struct scope *scope,
						     const struct vut_sv name) {
	while (scope != NULL) {
		struct namespace_entry *entry = namespace_find_name(&scope->namespace, name);

		if (entry != NULL)
			return entry;

		scope = scope->father == -1 ? NULL : &scope_array->base[scope->father];
	}

	return NULL;
}

static int find_scope_by_ast_node(const struct eval_ctx_scopes_t *scope_array,
				  ast_node_id_t ast_node) {
	for (size_t i = 0; i < scope_array->len; i++) {
		struct scope *s = &VUT_VEC_AT(scope_array, i);
		if (s->node == ast_node)
			return i;
	}

	return -1;
}

struct namespace_entry *eval_find_symbol_on_scopes(const struct eval_ctx *ctx, size_t curr_scope,
						   const struct vut_sv name) {
	struct scope *sp = &VUT_VEC_AT(&ctx->scopes, curr_scope);
	return find_symbol_on_scopes(&ctx->scopes, sp, name);
}

static int find_scope_child_by_node(const struct eval_ctx_scopes_t *scopes, size_t scope_id,
				    ast_node_id_t ast_node) {
	const struct scope *curr_scope = &VUT_VEC_AT(scopes, scope_id);
	int call_scope = -1;

	for (size_t i = 0; i < curr_scope->childs.len; i++) {
		size_t tmp_scope_id = VUT_VEC_AT(&curr_scope->childs, i);
		struct scope *tmp_scope = &VUT_VEC_AT(scopes, tmp_scope_id);

		if (tmp_scope->node == ast_node)
			call_scope = tmp_scope_id;
	}

	return call_scope;
}

#define RESOLVE_FUNC_SIGNATURE(func_name)                                                          \
	static void func_name(struct compiler_ctx *cctx, size_t curr_scope_id,                     \
			      ast_node_id_t ast_node)

RESOLVE_FUNC_SIGNATURE(resolve_symbols);

RESOLVE_FUNC_SIGNATURE(resolve_symbols_descent) {
	for (size_t i = 0; i < ast_get_num_child(&cctx->ast, ast_node); i++) {
		resolve_symbols(cctx, curr_scope_id, ast_get_nth_child(&cctx->ast, ast_node, i));
	}
}

RESOLVE_FUNC_SIGNATURE(resolve_symbols_assignment) {
	struct scope *curr_scope = &VUT_VEC_AT(&cctx->eval_ctx.scopes, curr_scope_id);

	struct ast *ast = &cctx->ast;
	struct vut_sv name = ast_get_text(ast, ast_get_nth_child(ast, ast_node, 0));
	struct namespace_entry entry = {
		.name = name,
		.type = ENTRY_INTERNAL,
		.as.ast_node_id = ast_get_num_child(ast, ast_node) > 1
					  ? (int)ast_get_nth_child(ast, ast_node, 1)
					  : -1,
	};

	VUT_VEC_PUT(&curr_scope->namespace, entry);
}

RESOLVE_FUNC_SIGNATURE(resolve_symbols) {
	struct ast *ast = &cctx->ast;
	struct eval_ctx *ctx = &cctx->eval_ctx;
	if (ast_get_type(ast, ast_node) == ASSIGNMENT) {
		resolve_symbols_assignment(cctx, curr_scope_id, ast_node);
	} else {
		if (ast_get_type(ast, ast_node) == CALL)
			curr_scope_id = add_scope_child(ctx, curr_scope_id, ast_node);

		resolve_symbols_descent(cctx, curr_scope_id, ast_node);
	}
}

RESOLVE_FUNC_SIGNATURE(resolve_calls);

RESOLVE_FUNC_SIGNATURE(resolve_calls_descent) {
	struct ast *ast = &cctx->ast;
	for (size_t i = 0; i < ast_get_num_child(ast, ast_node); i++) {
		resolve_calls(cctx, curr_scope_id, ast_get_nth_child(ast, ast_node, i));
	}
}

#define DO_CALLS_FUNC_SIGNATURE(func_name)                                                         \
	static void func_name(struct compiler_ctx *cctx, struct vut_str *out,                      \
			      ast_node_id_t ast_node, int flags)

DO_CALLS_FUNC_SIGNATURE(do_calls);

RESOLVE_FUNC_SIGNATURE(resolve_calls_call) {
	struct ast *ast = &cctx->ast;
	struct eval_ctx *ctx = &cctx->eval_ctx;
	struct scope *curr_scope = &VUT_VEC_AT(&ctx->scopes, curr_scope_id);

	ast_node_id_t call_name_ast = ast_get_nth_child(ast, ast_node, 0);
	assert(ast_get_type(ast, call_name_ast) == SYMBOL);

	bool allocated = false;
	struct vut_sv call_name = ast_get_text(ast, call_name_ast);
	struct vut_str call_name_str = { 0 };

	if (call_name.base == NULL) {
		if (ast_get_num_child(ast, call_name_ast) == 0) {
			fprintf(stderr, "ERROR: Symbol with no name\n");
			return;
		} else {
			ast_node_id_t call = ast_get_nth_child(ast, call_name_ast, 0);
			assert(ast_get_type(ast, call) == CALL);
			resolve_calls(cctx, curr_scope_id, call);

			call_name_str = vut_str_init(ctx->allocator);

			do_calls(cctx, &call_name_str, call, REDUCE_BLANKS);

			call_name = vut_sv_from_vut_str(&call_name_str);
			allocated = true;
		}
	}
	struct namespace_entry *symbol_info =
		find_symbol_on_scopes(&ctx->scopes, curr_scope, call_name);

	if (symbol_info != NULL) {
		if (symbol_info->type == ENTRY_INTERNAL) {
			if (ast_get_num_child(ast, ast_node) > 1) {
				if (symbol_info->as.ast_node_id < 0) {
					struct ast_node *node = &VUT_VEC_AT(&ast->nodes, ast_node);
					node->childs.len--;
					goto exit;
				}

				ast_node_id_t symbol_args =
					ast_copy_node(ast, symbol_info->as.ast_node_id);

				ast_node_id_t all_args = ast_get_nth_child(ast, ast_node, 1);
				for (size_t i = 0; i < ast_get_num_child(ast, symbol_args); i++) {
					ast_node_id_t child =
						ast_get_nth_child(ast, symbol_args, i);

					if (ast_get_type(ast, child) == ARG_REF_ALL_ARGS) {
						ast_set_nth_child(ast, symbol_args, i, all_args);
					}
				}
				ast_set_nth_child(ast, ast_node, 1, symbol_args);
			} else {
				if (symbol_info->as.ast_node_id >= 0) {
					ast_add_child(ast, ast_node, symbol_info->as.ast_node_id);
				}
			}

			ast_node_id_t new_node = ast_get_nth_child(ast, ast_node, 1);

			resolve_symbols(cctx, curr_scope_id, new_node);
		} else if (symbol_info->type == ENTRY_EXTERNAL) {
			ast_node_id_t symbol = ast_get_nth_child(ast, ast_node, 0);
			ast_set_type(ast, symbol, FUNCTION);

			if (ast_get_num_child(ast, ast_node) <= 1) {
				// ensure that the call node has an ARGS node
				// to prevent it from being skipped during evaluation
				size_t args_node_id = ast_node_new_nvl(ast, ARGS);
				ast_add_child(ast, ast_node, args_node_id);
			}
		}
	} else {
		fprintf(stderr, "ERROR: No symbol with name \"" VUT_SV_FMT "\" exist\n",
			VUT_SV_ARG(call_name));
	}

	resolve_calls_descent(cctx, curr_scope_id, ast_node);

exit:
	if (allocated)
		vut_str_free(&call_name_str);
}

RESOLVE_FUNC_SIGNATURE(resolve_calls) {
	struct ast *ast = &cctx->ast;
	switch (ast_get_type(ast, ast_node)) {
	case ARGS:
	case PROGRAM:
		resolve_calls_descent(cctx, curr_scope_id, ast_node);
		break;
	case CALL:;
		int new_scope =
			find_scope_child_by_node(&cctx->eval_ctx.scopes, curr_scope_id, ast_node);
		if (new_scope > 0)
			curr_scope_id = new_scope;
		else
			fprintf(stderr, "ERROR: Could not find call scope for node %u\n", ast_node);
		resolve_calls_call(cctx, curr_scope_id, ast_node);
		break;
	default:
		break;
	}
}

DO_CALLS_FUNC_SIGNATURE(do_calls_program) {
	struct ast *ast = &cctx->ast;
	for (size_t i = 0; i < ast_get_num_child(ast, ast_node); i++) {
		size_t child_id = ast_get_nth_child(ast, ast_node, i);
		int sub_flags = flags & ~(FIRST_CHILD | LAST_CHILD);
		if (i == 0) {
			sub_flags |= FIRST_CHILD;
		}
		if (i == ast_get_num_child(ast, ast_node) - 1) {
			sub_flags |= LAST_CHILD;
		}
		do_calls(cctx, out, child_id, sub_flags);
	}
}

DO_CALLS_FUNC_SIGNATURE(do_calls_call) {
	struct ast *ast = &cctx->ast;
	struct eval_ctx *ctx = &cctx->eval_ctx;
	if (ast_get_num_child(ast, ast_node) <= 1) {
		return;
	}

	ast_node_id_t symbol = ast_get_nth_child(ast, ast_node, 0);
	ast_node_id_t args = ast_get_nth_child(ast, ast_node, 1);

	if (ast_get_type(ast, symbol) == FUNCTION) {
		// writes the returns of the arguments calls to a temporary buffer,
		// so any nested call will be resolved normally
		struct vut_str tmp_out = vut_str_init(ctx->allocator);
		do_calls(cctx, &tmp_out, args, flags);

		// find the extern function on the scope
		struct scope *curr_scope = &VUT_VEC_AT(&ctx->scopes, 0);
		struct namespace_entry *symbol_info =
			find_symbol_on_scopes(&ctx->scopes, curr_scope, ast_get_text(ast, symbol));
		int curr_scope_id = find_scope_by_ast_node(&ctx->scopes, ast_node);

		assert(curr_scope_id != -1);

		// expose context to external function
		struct _call_ctx call_ctx = {
			.arg_text = vut_str_move_to_cstr(&tmp_out),
			.scope_id = curr_scope_id,
			.ast_node = ast_node,
			.compiler_ctx = cctx,
		};

		struct return_value call_return = symbol_info->as.func(&call_ctx);

		// put the extern function call return on the out str
		if ((flags & REDUCE_BLANKS) != 0) {
			vut_str_put_blank_reduced_cstr(out, call_return.ptr, true, true);
		} else {
			vut_str_put_cstr(out, call_return.ptr);
		}

		if (call_return.free) {
			free(call_return.ptr);
		}
	} else {
		do_calls(cctx, out, args, flags);
	}
}

DO_CALLS_FUNC_SIGNATURE(do_calls_text) {
	struct vut_sv text = ast_get_text(&cctx->ast, ast_node);

	if ((flags & REDUCE_BLANKS) != 0) {
		bool trim_left = (flags & FIRST_CHILD) != 0;
		bool trim_right = (flags & LAST_CHILD) != 0;
		vut_str_put_blank_reduced_sv(out, text, trim_left, trim_right);
	} else {
		vut_str_put_sv(out, text);
	}
}

DO_CALLS_FUNC_SIGNATURE(do_calls) {
	switch (ast_get_type(&cctx->ast, ast_node)) {
	case PROGRAM:
	case ARGS:
		do_calls_program(cctx, out, ast_node, flags);
		break;
	case CALL:
		do_calls_call(cctx, out, ast_node, flags);
		break;
	case TEXT:
	case LITERAL:
		do_calls_text(cctx, out, ast_node, flags);
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
			.name = vut_sv_from_cstr(f.name),
			.type = ENTRY_EXTERNAL,
			.as.func = f.fp,
		};

		VUT_VEC_PUT(&curr_scope->namespace, entry);
		f = lib.functions[++i];
	}
}

struct loaded_lib *load_libs(struct eval_ctx *ctx, struct sv_vec *libraries) {
	size_t len = libraries->len;
	if (len == 0)
		return NULL;
	struct loaded_lib *loaded_libs =
		vut_allocator_calloc(ctx->allocator, sizeof(struct loaded_lib), len + 1);
	for (size_t i = 0; i < len; i++) {
		loaded_libs[i] = load_lib(VUT_VEC_AT(libraries, i), ctx->allocator);
		resolve_extern_functions(ctx, loaded_libs[i]);
	}
	loaded_libs[len].dl_handle = NULL;
	return loaded_libs;
}

void unload_libs(struct loaded_lib *loaded_libs, struct vut_allocator alloc) {
	if (loaded_libs == NULL) {
		return;
	}
	int i = 0;
	while (loaded_libs[i].dl_handle != NULL) {
		unload_lib(loaded_libs[i]);
		i++;
	}
	vut_allocator_free(alloc, loaded_libs);
}

struct vut_str eval(struct compiler_ctx *cctx) {
	struct eval_ctx *ctx = &cctx->eval_ctx;
	struct scope base_scope = scope_new(ctx, 0, -1);
	VUT_VEC_PUT(&ctx->scopes, base_scope);

	struct loaded_lib *loaded_libs = load_libs(ctx, &cctx->libraries);

	struct vut_str str_out = vut_str_init(ctx->allocator);
	eval_node(cctx, 0, 0, &str_out);

	unload_libs(loaded_libs, ctx->allocator);

	return str_out;
}

void eval_node(struct compiler_ctx *cctx, ast_node_id_t ast_node, size_t scope_id,
	       struct vut_str *ret_str) {
	resolve_symbols(cctx, scope_id, ast_node);
	resolve_calls(cctx, scope_id, ast_node);

	do_calls(cctx, ret_str, ast_node, REDUCE_BLANKS);
}

void eval_dot(const struct eval_ctx *ctx, FILE *stream) {
	fprintf(stream, "digraph {\n");
	fprintf(stream, "\tnode [shape=record];\n");
	for (size_t i = 0; i < ctx->scopes.len; i++) {
		const struct scope *sc = &VUT_VEC_AT(&ctx->scopes, i);
		fprintf(stream, "\t%zu [label=\" %zu |", i, i);
		for (size_t j = 0; j < sc->namespace.len; j++) {
			fprintf(stream, VUT_SV_FMT, VUT_SV_ARG(VUT_VEC_AT(&sc->namespace, j).name));
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
