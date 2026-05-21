#include <vunit/vunit.h>

#include "test_helpers.h"

#ifdef __APPLE__
#define TESTLIB_PATH "subprojects/vinumc/tests/libtestlib.dylib"
#else
#define TESTLIB_PATH "subprojects/vinumc/tests/libtestlib.so"
#endif

void test_call(struct vunit_test_ctx *ctx) {
	const char *out =
		compile_program_with_libraries(ctx, "[return_arg test]\n", TESTLIB_PATH, NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "test");
}

void test_nested_call(struct vunit_test_ctx *ctx) {
	const char *out = compile_program_with_libraries(ctx,
							 "[a: This is a [return_arg Test!]!]\n"
							 "[a]\n",
							 TESTLIB_PATH, NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "This is a Test!!");
}

void test_empty_nested_call(struct vunit_test_ctx *ctx) {
	const char *out = compile_program_with_libraries(
		ctx, "[return_arg [return_arg [return_arg]]]", TESTLIB_PATH, NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "");
}

void test_call_no_args(struct vunit_test_ctx *ctx) {
	const char *out = compile_program_with_libraries(ctx, "[parenthesize]", TESTLIB_PATH, NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "()");
}

void test_call_eval_symbol(struct vunit_test_ctx *ctx) {
	const char *out = compile_program_with_libraries(ctx,
							 "[author: Lorem Ipsum]\n"
							 "[author_last_name]\n",
							 TESTLIB_PATH, NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "Ipsum");
}

void test_call_eval_symbol_inside(struct vunit_test_ctx *ctx) {
	const char *out = compile_program_with_libraries(
		ctx, "[author_last_name [author: Lorem Ipsum]]\n", TESTLIB_PATH, NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "Ipsum");
}

void test_call_eval_extern_symbol(struct vunit_test_ctx *ctx) {
	const char *out = compile_program_with_libraries(ctx,
							 "[author_last_name]\n"
							 "[author: Lorem [parenthesize Ipsum]]\n",
							 TESTLIB_PATH, NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "(Ipsum)");
}

struct vunit_test tests[] = {
	{ .name = "Test extern library call", .test_func = test_call },
	{ .name = "Test extern library call inside call", .test_func = test_nested_call },
	{ .name = "Test nested empty library calls", .test_func = test_empty_nested_call },
	{
		.name = "Test extern library call with no arguments",
		.test_func = test_call_no_args,
	},
	{
		.name = "Test extern library call that search symbol by name",
		.test_func = test_call_eval_symbol,
	},
	{
		.name = "Test extern library call that search symbol by name declared in function "
			"arg",
		.test_func = test_call_eval_symbol_inside,
	},
	{
		.name = "Test extern library call that call extern symbol by name",
		.test_func = test_call_eval_extern_symbol,
	},
	{},
};

VUNIT_TEST_SUITE(tests)
