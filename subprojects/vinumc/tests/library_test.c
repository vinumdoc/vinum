#include <vunit/vunit.h>

#include "libvinumc.h"

static char *compile_program_with_testlib(const char *prg_cstr, struct vut_allocator alloc) {
	struct compiler_ctx comp = compiler_ctx_init(alloc);

	struct vut_str prg = vut_str_init(alloc);
	vut_str_put_cstr(&prg, prg_cstr);
	VUT_VEC_PUT(&comp.libraries, vut_sv_from_cstr("subprojects/vinumc/tests/libtestlib.so"));

	struct vut_str out = compiler_compile(&comp, &prg);

	return vut_str_move_to_cstr(&out);
}

void test_call(struct vunit_test_ctx *ctx) {
	const char *out = compile_program_with_testlib("[return_arg test]\n", ctx->allocator);

	VUNIT_ASSERT_STREQ(ctx, out, "test");
}

void test_nested_call(struct vunit_test_ctx *ctx) {
	const char *out = compile_program_with_testlib("[a: This is a [return_arg Test!]!]\n"
						       "[a]\n",
						       ctx->allocator);

	VUNIT_ASSERT_STREQ(ctx, out, "This is a Test!!");
}

void test_empty_nested_call(struct vunit_test_ctx *ctx) {
	const char *out = compile_program_with_testlib("[return_arg [return_arg [return_arg]]]",
						       ctx->allocator);

	VUNIT_ASSERT_STREQ(ctx, out, "");
}

void test_call_no_args(struct vunit_test_ctx *ctx) {
	const char *out = compile_program_with_testlib("[parenthesize]", ctx->allocator);

	VUNIT_ASSERT_STREQ(ctx, out, "()");
}

struct vunit_test tests[] = {
	{ .name = "Test extern library call", .test_func = test_call },
	{ .name = "Test extern library call inside call", .test_func = test_nested_call },
	{ .name = "Test nested empty library calls", .test_func = test_empty_nested_call },
	{
		.name = "Test extern library call with no arguments",
		.test_func = test_call_no_args,
	},
	{},
};

VUNIT_TEST_SUITE(tests)
