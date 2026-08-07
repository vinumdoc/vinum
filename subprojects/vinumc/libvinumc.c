#include "libvinumc.h"
#include "dry_flex.h"
#include <vutils/arena_allocator.h>

struct compiler_ctx compiler_ctx_init(struct vut_allocator alloc) {
	struct compiler_ctx ctx = {
		.ast = ast_new(alloc),
		.eval_ctx = eval_ctx_new(alloc),
		.libraries = VUT_VEC_INIT(struct sv_vec, alloc),
		.dry_flex_text_buffer = vut_str_init(alloc),

		.alloc = alloc,
	};

	struct vut_arena *arena = vut_allocator_malloc(alloc, sizeof(*arena), 1);
	*arena = vut_arena_new(alloc, 1024 * 1024),

	ctx.dry_arena = vut_arena_to_vut_allocator(arena);
	ctx.dry_flex_text_buffer = vut_str_init(ctx.dry_arena);

	return ctx;
}

void compiler_ctx_free(struct compiler_ctx *ctx) {
	vut_arena_free_all(ctx->dry_arena.base_allocator);
	vut_allocator_free(ctx->alloc, ctx->dry_arena.base_allocator);

	VUT_VEC_FREE(&ctx->libraries);
	eval_ctx_free(&ctx->eval_ctx);
	ast_free(&ctx->ast);

	*ctx = (struct compiler_ctx){ 0 };
}

typedef struct yy_buffer_state *YY_BUFFER_STATE;

void compiler_parse(struct compiler_ctx *ctx, struct vut_str *program) {
	yyscan_t scanner;
	yylex_init_extra(ctx, &scanner);

	// Using yy_scan_buffer lets flex scan the memory directly without copying.
	// Flex requires that the buffer ends with two trailing null terminators.
	VUT_VEC_PUT_MANY(program, "\0\0", 2);
	YY_BUFFER_STATE buffer = yy_scan_buffer(program->base, program->len, scanner);

	yyparse(scanner, ctx);

	yy_delete_buffer(buffer, scanner);
	yylex_destroy(scanner);
}

struct vut_str compiler_eval(struct compiler_ctx *ctx) {
	return eval(ctx);
}

struct vut_str compiler_compile(struct compiler_ctx *ctx, struct vut_str *program) {
	compiler_parse(ctx, program);
	return eval(ctx);
}
