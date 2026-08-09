#include <vunit/vunit.h>

#include "libvinumc.h"

static char *compile_program_with_testlib(const char *program_cstr, const char *cocktail_cstr,
					  struct vut_allocator alloc) {
	struct compiler_ctx comp = compiler_ctx_init(alloc);

	struct vut_str program = vut_str_init(alloc);
	vut_str_put_cstr(&program, program_cstr);

	struct vut_str cocktail = vut_str_init(alloc);
	vut_str_put_cstr(&cocktail, cocktail_cstr);

	struct vut_str out = compiler_compile(&comp, &program, &cocktail);

	return vut_str_move_to_cstr(&out);
}

void test_undefined(struct vunit_test_ctx *ctx) {
	const char *out = compile_program_with_testlib("[author]", "", ctx->allocator);

	VUNIT_ASSERT_STREQ(ctx, out, "[author]");
}

void test_defined(struct vunit_test_ctx *ctx) {
	const char *out =
		compile_program_with_testlib("[author]", "[author: Vinum Enjoyer]", ctx->allocator);
	VUNIT_ASSERT_STREQ(ctx, out, "Vinum Enjoyer");
}

void test_args(struct vunit_test_ctx *ctx) {
	const char *out = compile_program_with_testlib("[doc [par Hello, [author]!]]",
						       "[author: Vinum Enjoyer]\n"
						       "[par: <p> $* </p>]\n"
						       "[doc: <head></head><body> $* </body>]",
						       ctx->allocator);
	VUNIT_ASSERT_STREQ(ctx, out, "<head></head><body> <p> Hello,Vinum Enjoyer! </p> </body>");
}

struct vunit_test tests[] = {
	{ .name = "Test symbol not defined in program and cocktail", .test_func = test_undefined },
	{ .name = "Test symbol only defined on cocktail", .test_func = test_defined },
	{ .name = "Test defined on cocktail with arguments", .test_func = test_args },
	{},
};

VUNIT_TEST_SUITE(tests)
