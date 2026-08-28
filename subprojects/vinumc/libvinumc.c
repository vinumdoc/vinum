#include "libvinumc.h"
#include "dry_flex.h"
#include <vutils/arena_allocator.h>

struct compiler_ctx compiler_ctx_init(struct vut_allocator alloc) {
	struct compiler_ctx ctx = {
		.ast = ast_new(alloc),
		.eval_ctx = eval_ctx_new(alloc),
		.libraries = VUT_VEC_INIT(struct sv_vec, alloc),
		.cocktailing_libraries = VUT_VEC_INIT(struct sv_vec, alloc),
		.dry_flex_text_buffer = vut_str_init(alloc),

		.alloc = alloc,
	};

	struct vut_arena *arena = vut_allocator_malloc(alloc, sizeof(*arena), 1);
	*arena = vut_arena_new(alloc, 1024 * 1024),

	ctx.dry_arena = vut_arena_to_vut_allocator(arena);
	ctx.dry_flex_text_buffer = vut_str_init(ctx.dry_arena);
	ctx.line_buffer = vut_str_init(ctx.dry_arena);

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
	yyset_lineno(0, scanner);

	yyparse(scanner, ctx);

	yy_delete_buffer(buffer, scanner);
	yylex_destroy(scanner);
}

struct vut_str compiler_eval(struct compiler_ctx *ctx) {
	return eval(ctx);
}

struct vut_str compiler_compile(struct compiler_ctx *ctx, struct vut_str *program,
				struct vut_str *cocktail) {
	compiler_parse(ctx, program);
	struct vut_str eval_output = eval(ctx);

	if (VUT_STR_EMPTY(*cocktail)) {
		return eval_output;
	}

	struct compiler_ctx cocktail_ctx = compiler_ctx_init(ctx->alloc);

	vut_str_put_sv(&eval_output, vut_sv_from_vut_str(cocktail));

	compiler_parse(&cocktail_ctx, &eval_output);

	struct vut_str cocktail_output = compiler_eval(&cocktail_ctx);

	vut_str_free(&eval_output);
	compiler_ctx_free(&cocktail_ctx);

	return cocktail_output;
}

#define ANSI_BOLD_RED "\x1b[1;31m"
#define ANSI_RESET "\x1b[0m"

void print_error(const char *error_msg, int line, int column, int length, char *error_line_text,
		 const char *filename) {
	fprintf(stderr, "%s:%d:%d: ", filename, line, column);
	fprintf(stderr, ANSI_BOLD_RED "error: " ANSI_RESET);
	fprintf(stderr, "%s", error_msg);
	fprintf(stderr, "\n");

	// TODO: color error on line
	fprintf(stderr, "%5d | %s\n", line, error_line_text);
	fprintf(stderr, "      | ");
	for (int i = 1; i < column; i++) {
		fprintf(stderr, " ");
	}
	fprintf(stderr, ANSI_BOLD_RED "^");
	for (int i = 1; i < length; i++) {
		fprintf(stderr, ANSI_BOLD_RED "~");
	}
	fprintf(stderr, ANSI_RESET "\n\n");
}
