#ifndef __EVAL_H__
#define __EVAL_H__

#include <stdio.h>

#include <vutils/vec.h>

#include "ast.h"
#include "extern_library.h"

struct scope_childs_t VUT_VEC_DEF(size_t);

enum entry_type {
	ENTRY_INTERNAL,
	ENTRY_EXTERNAL,
};

struct namespace_entry {
	struct vut_sv name;
	enum entry_type type;
	union {
		int ast_node_id;
		extern_function_pointer func;
	} as;
};

struct scope_namespace_t VUT_VEC_DEF(struct namespace_entry);

struct scope {
	int father;
	ast_node_id_t node;

	struct scope_childs_t childs;

	struct scope_namespace_t namespace;
};

struct eval_ctx_scopes_t VUT_VEC_DEF(struct scope);

struct eval_ctx {
	struct eval_ctx_scopes_t scopes;

	struct vut_allocator allocator;
	struct vut_allocator scopes_childs_arena;
	struct vut_allocator scopes_namespace_arena;
};

struct eval_ctx eval_ctx_new(struct vut_allocator allocator);
void eval_ctx_free(struct eval_ctx *ctx);

struct sv_vec VUT_VEC_DEF(struct vut_sv);

struct vut_str eval(struct compiler_ctx *cctx);

void eval_node(struct compiler_ctx *cctx, ast_node_id_t ast_node, size_t scope_id,
	       struct vut_str *ret_str);

void eval_dot(const struct eval_ctx *ctx, FILE *stream);

struct namespace_entry *eval_find_symbol_on_scopes(const struct eval_ctx *ctx, size_t curr_scope,
						   const struct vut_sv name);

#endif // __EVAL_H__
