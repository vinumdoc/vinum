#include "libvinumc.h"
#include "dry_flex.h"

struct compiler_ctx compiler_ctx_init(struct vut_allocator *alloc) {
	struct compiler_ctx ctx = {
		.ast = ast_new(alloc),
		.eval_ctx = eval_ctx_new(alloc),
		.libraries = VUT_VEC_INIT(struct sv_vec, alloc),
		.dry_flex_text_buffer = vut_str_init(alloc),

		.alloc = alloc,
	};

	return ctx;
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
	return eval(&ctx->eval_ctx, &ctx->ast, &ctx->libraries);
}

struct vut_str compiler_compile(struct compiler_ctx *ctx, struct vut_str *program) {
	compiler_parse(ctx, program);
	return compiler_eval(ctx);
}
