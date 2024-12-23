#ifndef __EVAL_H__
#define __EVAL_H__

#include <stdio.h>

#include "ast.h"
#include "vec.h"

struct scope_childs_t VEC_DEF(size_t);

struct namespace_entry {
	char *name;
	int ast_node_id;
};

struct scope_namespace_t VEC_DEF(struct namespace_entry);

struct scope {
	int father;
	ast_node_id_t node;

	struct scope_childs_t childs;

	struct scope_namespace_t namespace;
};

struct eval_ctx_scopes_t VEC_DEF(struct scope);

struct eval_ctx {
	struct eval_ctx_scopes_t scopes;
};

struct eval_ctx eval_ctx_new();
void eval(struct eval_ctx *ctx, struct ast *ast, FILE *out);

void eval_dot(const struct eval_ctx *ctx, FILE *stream);

#endif // __EVAL_H__
