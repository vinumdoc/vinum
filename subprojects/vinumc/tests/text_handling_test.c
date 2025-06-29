#include <vunit.h>

void test_blank_reduction(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[a:    Content   with spaces!    ]\n"
			    "[a]\n",
			    &out, NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "Content with spaces!");
}

void test_character_escaping(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[a: \\[ hello \\] \\{# world #}!]\n"
			    "[a]\n",
			    &out, NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "[ hello ] {# world #}!");
}

struct vunit_test tests[] = {
	{ .name = "Test blank reduction", .test_func = test_blank_reduction },
	{ .name = "Test character escaping", .test_func = test_character_escaping },
	{},
};

VUNIT_TEST_SUITE(tests)
