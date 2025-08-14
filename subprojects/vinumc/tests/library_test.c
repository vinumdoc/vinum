#include <vunit.h>

void test_call(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx, "[return_arg test]\n", &out, "--with",
			    "subprojects/vinumc/tests/libtestlib.so", NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "test");
}

void test_nested_call(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[a: This is a [return_arg Test!]!]\n"
			    "[a]\n",
			    &out, "--with", "subprojects/vinumc/tests/libtestlib.so", NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "This is a Test!!");
}

void test_empty_nested_call(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx, "[return_arg [return_arg [return_arg]]]", &out, "--with",
			    "subprojects/vinumc/tests/libtestlib.so", NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "");
}

void test_call_no_args(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx, "[parenthesize]", &out, "--with",
			    "subprojects/vinumc/tests/libtestlib.so", NULL);

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
