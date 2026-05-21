#include <vunit/vunit.h>

#include "test_helpers.h"

void test_basic(struct vunit_test_ctx *ctx) {
	const char *out = compile_program(ctx, "[a: Hello World!]\n"
					       "[a]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Hello World!");
}

void test_define_later(struct vunit_test_ctx *ctx) {
	const char *out = compile_program(ctx, "[a]\n"
					       "[a: Hello World!]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Hello World!");
}

void test_function_text_args(struct vunit_test_ctx *ctx) {
	const char *out = compile_program(ctx, "[a: $*]\n"
					       "[a Hello World!]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Hello World!");
}

void test_function_call_args(struct vunit_test_ctx *ctx) {
	const char *out = compile_program(ctx, "[a: Hello World!]\n"
					       "[b: $*]\n"
					       "[b [a]]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Hello World!");
}

void test_file_input(struct vunit_test_ctx *ctx) {
	vunit_str_to_file(ctx, "input.vin",
			  "[a: Hello World!]\n"
			  "[b: $*]\n"
			  "[b [a]]\n");

	char *out = NULL;

	vunit_run_vinumc_ok(ctx, NULL, &out, "input.vin", NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "Hello World!");
}

struct vunit_test tests[] = {
	{ .name = "Basic test", .test_func = test_basic },
	{ .name = "Symbol defined later", .test_func = test_define_later },
	{ .name = "Function with text args", .test_func = test_function_text_args },
	{ .name = "Function with function calls args", .test_func = test_function_call_args },
	{ .name = "File as input", .test_func = test_file_input },
	{},
};

VUNIT_TEST_SUITE(tests)
