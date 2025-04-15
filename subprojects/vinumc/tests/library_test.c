#include <vunit.h>

void test_call(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx, "[return_arg test]\n", &out, "--with",
			    "subprojects/vinumc/tests/libtestlib.so", NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "test\n");
}

void test_nested_call(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[a: This is a [return_arg Test!]!]\n"
			    "[a]\n",
			    &out, "--with", "subprojects/vinumc/tests/libtestlib.so", NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "This is a\nTest!\n!\n");
}

VUNIT_TEST_SUITE("suite", { "Test extern library call", test_call },
		 { "Test extern library call inside call", test_nested_call }, {}, )
