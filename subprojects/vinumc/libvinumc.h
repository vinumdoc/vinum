#ifndef __LIBVINUMC_H__
#define __LIBVINUMC_H__

#include <vutils/allocator.h>
#include <vutils/arena_allocator.h>
#include <vutils/str.h>

#include "ast.h"
#include "eval.h"

struct compiler_ctx {
	struct ast ast;
	struct eval_ctx eval_ctx;
	struct sv_vec libraries;
	struct sv_vec cocktailing_libraries;
	// used to handle character escaping inside the regular text blocks
	// preventing having two consecutive TEXT nodes
	// that could be just one node
	struct vut_str dry_flex_text_buffer;

	struct vut_allocator dry_arena;
	struct vut_allocator alloc;
};

struct compiler_ctx compiler_ctx_init(struct vut_allocator alloc);
void compiler_ctx_free(struct compiler_ctx *ctx);

void compiler_parse(struct compiler_ctx *ctx, struct vut_str *program);
struct vut_str compiler_eval(struct compiler_ctx *ctx);
struct vut_str compiler_compile(struct compiler_ctx *ctx, struct vut_str *program);

#endif // __LIBVINUMC_H__
