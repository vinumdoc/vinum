#include <vunit.h>

void test_call(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
		"[day_of_week 2024-09-26]\n",
		&out,
		"--with","subprojects/stdlib/libstdlib.so" , NULL
	);

	VUNIT_ASSERT_STREQ(ctx, out, "Wednesday");
}

void test_nested_call(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
		"[a: Today is [day_of_week 2024-09-26].]\n"
		"[a]\n",
		&out,
		"--with","subprojects/stdlib/libstdlib.so" , NULL
	);

	VUNIT_ASSERT_STREQ(ctx, out, "Today is\nWednesday.\n");
}


VUNIT_TEST_SUITE("suite",
	{"Test extern library call", test_call},
	{"Test extern library call inside call", test_nested_call},
	{},
)
