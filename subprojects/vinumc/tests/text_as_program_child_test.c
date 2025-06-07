#include <vunit.h>

void test_simple_text(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx, "Hello World!", &out, NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "Hello World!\n");
}

void test_mixed_text_and_blocks(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "Hello [a]\n"
			    "[a: World!]\n",
			    &out, NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "Hello\nWorld!\n");
}

void test_ref_all_args_is_empty(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx, "$*", &out, NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "");
}

VUNIT_TEST_SUITE("suite", { "Simple text", test_simple_text },
		 { "Mixed text and blocks", test_mixed_text_and_blocks },
		 { "Ref all args is empty", test_ref_all_args_is_empty }, {}, )
