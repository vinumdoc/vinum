#ifndef __EVAL_H__
#define __EVAL_H__

#include <stdio.h>

#include "ast.h"
#include "vec.h"

struct program_ctx_childs_t VEC_DEF(size_t);

struct namespace_entry {
	char *name;
	int ast_node_id;
};

struct program_ctx_namespace_t VEC_DEF(struct namespace_entry);

// TODO: s/program_ctx/scope
struct program_ctx {
	struct program_ctx_childs_t childs;

	struct program_ctx_namespace_t namespace;
};

struct eval_ctx_program_ctxs_t VEC_DEF(struct program_ctx);

struct eval_ctx {
	struct eval_ctx_program_ctxs_t program_ctxs;
};

struct eval_ctx eval_ctx_new();
void eval(struct eval_ctx *ctx, struct ast *ast, FILE *out);

#endif // __EVAL_H__
