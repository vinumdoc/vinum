#include <vunit.h>

void test_blank_reduction(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[a:    Content   with spaces!    ]\n"
			    "[a]\n",
			    &out, NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "Content with spaces!");
}

struct vunit_test tests[] = {
	{ .name = "Test blank reduction", .test_func = test_blank_reduction },
	{},
};

VUNIT_TEST_SUITE(tests)
