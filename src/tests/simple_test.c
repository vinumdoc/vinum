#include "vunit.h"

void test_basic(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
		"[a: Hello World!]\n"
		"[a]\n",
		&out,
		NULL
	);

	VUNIT_ASSERT_STREQ(ctx, out, "Hello World!\n");
}

void test_define_later(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
		"[a]\n"
		"[a: Hello World!]\n",
		&out,
		NULL
	);

	VUNIT_ASSERT_STREQ(ctx, out, "Hello World!\n");
}

void test_function_text_args(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
		"[a: $*]\n"
		"[a Hello World!]\n",
		&out,
		NULL
	);

	VUNIT_ASSERT_STREQ(ctx, out, "Hello World!\n");
}

void test_function_call_args(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
		"[a: Hello World!]\n"
		"[b: $*]\n"
		"[b [a]]\n",
		&out,
		NULL
	);

	VUNIT_ASSERT_STREQ(ctx, out, "Hello World!\n");
}

VUNIT_TEST_SUITE("suite",
	{"Basic test", test_basic},
	{"Symbol defined later", test_define_later},
	{"Function with text args", test_function_text_args},
	{"Function with function calls args", test_function_call_args},
	{},
)
