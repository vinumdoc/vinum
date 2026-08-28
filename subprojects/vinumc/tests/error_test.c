#include <vunit/vunit.h>

#include "libvinumc.h"

#define ANSI_BOLD_RED "\x1b[1;31m"
#define ANSI_RESET "\x1b[0m"

static char *compile_program(char *prg_cstr, struct vunit_test_ctx *ctx) {
	char *out = NULL;
	char *error = NULL;

	vunit_run_vinumc(ctx, prg_cstr, &out, &error, NULL, 0);

	return error;
}

void test_basic(struct vunit_test_ctx *ctx) {
	const char *expected_output =
		"input:1:18: " ANSI_BOLD_RED "error: " ANSI_RESET "syntax error\n"
		"    1 | [a: Hello World!]]\n"
		"      |                  " ANSI_BOLD_RED "^" ANSI_RESET "\n\n";

	const char *out = compile_program("[a: Hello World!]]\n"
					  "[a]\n",
					  ctx);

	VUNIT_ASSERT_STREQ(ctx, out, expected_output);
}

struct vunit_test tests[] = {
	{ .name = "Basic error output test", .test_func = test_basic },
	{},
};

VUNIT_TEST_SUITE(tests)
