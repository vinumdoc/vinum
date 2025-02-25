#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eval.h"

#define EXTERN_CALL_BUFFER_SIZE 1024

struct eval_ctx eval_ctx_new() {
	struct eval_ctx ret = { };

	return ret;
}

static size_t add_scope_child(struct eval_ctx_scopes_t *scope_array, size_t scope_id,
			      ast_node_id_t node) {
	size_t new_scope_id = scope_array->len;

	VEC_PUT(scope_array, ((struct scope){.father = scope_id, node = node}));
	struct scope *scope = &scope_array->base[scope_id];
	VEC_PUT(&scope->childs, new_scope_id);

	return new_scope_id;
}

static struct namespace_entry* namespace_find_name(const struct scope_namespace_t *namespace,
						   const char *name) {
	for (size_t i = 0; i < namespace->len; i++) {
		if (strcmp(name, VEC_AT(namespace, i).name) == 0)
			return &VEC_AT(namespace, i);
	}

	return NULL;
}

static struct namespace_entry* find_symbol_on_scopes(const struct eval_ctx_scopes_t *scope_array,
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

#define RESOLVE_FUNC_SIGNATURE(func_name) \
	static void func_name (struct eval_ctx *ctx, struct ast *ast, size_t curr_scope_id, \
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
		.ast_node_id = ast_node->childs.len > 1 ? (int)VEC_AT(&ast_node->childs, 1) : -1,
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
	struct ast_node ast_node = VEC_AT(&ast->nodes, ast_node_id);

	char *call_name = VEC_AT(&ast->nodes, VEC_AT(&ast_node.childs, 0)).text;
	if (call_name == NULL) {
		fprintf(stderr, "ERROR: Symbol with no name\n");
		return;
	}

	struct namespace_entry *symbol_info = find_symbol_on_scopes(&ctx->scopes, curr_scope,
								    call_name);

	if (symbol_info != NULL) {
		if (symbol_info->type == ENTRY_INTERNAL){
			if (ast_node.childs.len > 1) {
				if (symbol_info->ast_node_id < 0) {
					ast_node.childs.len--;
					return;
				}

				size_t symbol_args_node_id = ast_copy_node(ast, symbol_info->ast_node_id);
				struct ast_node *symbol_args_node = &VEC_AT(&ast->nodes, symbol_args_node_id);

				for (size_t i = 0; i < symbol_args_node->childs.len; i++) {
					struct ast_node *child = &VEC_AT(&ast->nodes,
									 VEC_AT(&symbol_args_node->childs,
										i));

					if (child->type == ARG_REF_ALL_ARGS) {
						VEC_AT(&symbol_args_node->childs, i) = VEC_AT(&ast_node.childs, 1);
					}
				}

				VEC_AT(&VEC_AT(&ast->nodes, ast_node_id).childs, 1) = symbol_args_node_id;
			} else {
				if (symbol_info->ast_node_id >= 0) {
					ast_node_add_child(&VEC_AT(&ast->nodes, ast_node_id),
							   symbol_info->ast_node_id);
				}
			}

			size_t new_node = VEC_AT(&VEC_AT(&ast->nodes, ast_node_id).childs, 1);

			resolve_symbols(ctx, ast, curr_scope_id, new_node);

		} else if (symbol_info->type == ENTRY_EXTERNAL) {
			struct ast_node *symbol_node = &VEC_AT(&ast->nodes, VEC_AT(&ast_node.childs, 0));
			symbol_node->type = FUNCTION;
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
			int new_scope = find_scope_child_by_node(&ctx->scopes, curr_scope_id,
								 ast_node_id);
			if (new_scope > 0)
				curr_scope_id = new_scope;
			else
				fprintf(stderr, "ERROR: Could not find call scope for node %zu\n", ast_node_id);
			resolve_calls_call(ctx, ast, curr_scope_id, ast_node_id);
			break;
		default:
			break;
	}
}

#define DO_CALLS_FUNC_SIGNATURE(func_name) \
static void func_name (struct eval_ctx *ctx, const struct ast *ast, FILE *out, size_t ast_node_id)

DO_CALLS_FUNC_SIGNATURE(do_calls);

DO_CALLS_FUNC_SIGNATURE(do_calls_program) {
	const struct ast_node *ast_node = &VEC_AT(&ast->nodes, ast_node_id);
	for (size_t i = 0; i < ast_node->childs.len; i++) {
		do_calls(ctx, ast, out, VEC_AT(&ast_node->childs, i));
	}
}

DO_CALLS_FUNC_SIGNATURE(do_calls_call) {
	const struct ast_node *ast_node = &VEC_AT(&ast->nodes, ast_node_id);

	if (ast_node->childs.len <= 1) {
		return;
	}


	const struct ast_node *symbol_node = &VEC_AT(&ast->nodes, VEC_AT(&ast_node->childs, 0));
	const struct ast_node *args_node = &VEC_AT(&ast->nodes, VEC_AT(&ast_node->childs, 1));

	if (symbol_node->type == FUNCTION){
		// writes the returns of the arguments calls to a temporary buffer,
		// so any nested call will be resolved normally
		char *buffer = (char*) calloc(1, EXTERN_CALL_BUFFER_SIZE);
		FILE *tmp_out = fmemopen(buffer, EXTERN_CALL_BUFFER_SIZE, "w+");
		for (size_t i = 0; i < args_node->childs.len; i++) {
			do_calls(ctx, ast, tmp_out, VEC_AT(&args_node->childs, i));
		}
		fflush(tmp_out);

		// find the extern function on the scope
		struct scope *curr_scope = &VEC_AT(&ctx->scopes, 0);
		struct namespace_entry *symbol_info = find_symbol_on_scopes(&ctx->scopes, curr_scope, symbol_node->text);

		// write the extern function call return on the out file
		fprintf(out, "%s", symbol_info->fp(buffer));
		fclose(tmp_out);
		free(buffer);
	}else{
		for (size_t i = 0; i < args_node->childs.len; i++) {
			do_calls(ctx, ast, out, VEC_AT(&args_node->childs, i));
		}
	}
}

DO_CALLS_FUNC_SIGNATURE(do_calls_text) {
	// TODO better handle this unused
	(void) ctx;
	const struct ast_node *ast_node = &VEC_AT(&ast->nodes, ast_node_id);

	for (size_t i = 0; i < ast_node->childs.len; i++) {
		const struct ast_node *child = &VEC_AT(&ast->nodes, VEC_AT(&ast_node->childs, i));
		if (child->type != WORD) {
			fprintf(stderr, "ERROR: TEXT node must hold only WORDS\n");
			return;
		}

		fprintf(out, "%s", child->text);
		if (i != ast_node->childs.len - 1)
			fprintf(out, " ");
	}
	fprintf(out, "\n");
}

DO_CALLS_FUNC_SIGNATURE(do_calls) {
	const struct ast_node *ast_node = &VEC_AT(&ast->nodes, ast_node_id);

	switch (ast_node->type) {
		case PROGRAM:
		case ARGS:
			do_calls_program(ctx, ast, out, ast_node_id);
			break;
		case CALL:
			do_calls_call(ctx, ast, out, ast_node_id);
			break;
		case TEXT:
			do_calls_text(ctx, ast, out, ast_node_id);
			break;
		default:
			break;
	}
}

void resolve_extern_functions(struct eval_ctx *ctx, struct loaded_lib lib){
	struct scope *curr_scope = &VEC_AT(&ctx->scopes, 0);

	int i = 0;
	struct extern_function f = lib.functions[i];
	while (f.name != 0){
		struct namespace_entry entry = {
			.name = f.name,
			.type = ENTRY_EXTERNAL,
			.fp = f.fp,
		};

		VEC_PUT(&curr_scope->namespace, entry);
		f = lib.functions[++i];
	}
}

struct loaded_lib *load_libs(struct eval_ctx *ctx, struct libraries *libraries){
	struct loaded_lib *loaded_libs = malloc(sizeof(struct loaded_lib) * libraries->counter);
	for (int i = 0; i < libraries->counter; i++){
		loaded_libs[i] = load_lib(libraries->names[i]);
		resolve_extern_functions(ctx, loaded_libs[i]);
	}
	return loaded_libs;
}

void unload_libs(struct loaded_lib *loaded_libs, int counter) {
	for (int i = 0; i < counter; i++) {
  	unload_lib(loaded_libs[i]);
  }
	free(loaded_libs);
}

void eval(struct eval_ctx *ctx, struct ast *ast, FILE *out, struct libraries *libraries) {
	VEC_PUT(&ctx->scopes, ((struct scope){
			.father = -1,
			.node = 0,
	}));

	struct loaded_lib *loaded_libs = load_libs(ctx, libraries);
	resolve_symbols(ctx, ast, 0, 0);
	resolve_calls(ctx, ast, 0, 0);
	do_calls(ctx, ast, out, 0);
	unload_libs(loaded_libs, libraries->counter);
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
