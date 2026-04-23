#include <stdlib.h>

#include <vunit/vunit.h>

void test_output_flag(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[a: Hello World!]\n"
			    "[a]\n",
			    &out, "-o", "out.txt", NULL);

	char *out_file_str = vunit_file_to_str(ctx, "out.txt");
	VUNIT_ASSERT_STREQ(ctx, out_file_str, "Hello World!");
	VUNIT_ASSERT_STREQ(ctx, out, "");

	vunit_run_vinumc_ok(ctx,
			    "[a: Hello World!]\n"
			    "[a]\n",
			    &out, "--output", "out.txt", NULL);

	out_file_str = vunit_file_to_str(ctx, "out.txt");
	VUNIT_ASSERT_STREQ(ctx, out, "");

	VUNIT_ASSERT_STREQ(ctx, out_file_str, "Hello World!");
}

struct vunit_test tests[] = {
	{ .name = "Test output flag", .test_func = test_output_flag },
	{},
};

VUNIT_TEST_SUITE(tests)
