#include "vunit.h"

void test_extra_closing(struct vunit_test_ctx *ctx) {
	char *out = NULL;
	char *error = NULL;

	vunit_run_vinumc_error(ctx,
		"[a: Hello World!]]\n"
		"[a]\n",
		&out,
		&error,
		NULL
	);

	VUNIT_ASSERT_STREQ(ctx, out, "Hello World!\n");
	VUNIT_ASSERT_STREQ(ctx, error, "[ERROR]:syntax error\n(stdin):1:18: error: extra ']'\n");
}

void test_extra_opening(struct vunit_test_ctx *ctx) {
	char *out = NULL;
	char *error = NULL;

	vunit_run_vinumc_error(ctx,
		"[a: Hello World!]\n"
		"[a]\n"
		"[[a]\n",
		&out,
		&error,
		NULL
	);

	VUNIT_ASSERT_STREQ(ctx, out, "Hello World!\n");
	VUNIT_ASSERT_STREQ(ctx, error, "[ERROR]:syntax error\n(stdin):3:1: error: no matching for '['\n");
}

VUNIT_TEST_SUITE("suite",
	{"Extra closing ']'", test_extra_closing},
	{"Extra opening '['", test_extra_opening},
	{},
)
