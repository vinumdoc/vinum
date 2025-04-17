#include <stdlib.h>
#include <vunit.h>

void test_output_flag(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[a: Hello World!]\n"
			    "[a]\n",
			    &out, "-h", NULL);

	const char *help_output =
		"usage: vinumc [-o | --output <output_path>] [-h | --help] [<file-input>]\n"
		"\n"
		"Options:\n"
		"  -o, --output\n"
		"	Set file name output, if not set the output will be stdout\n"
		"\n"
		"  -h, --help\n"
		"	Show help\n";

	VUNIT_ASSERT_STREQ(ctx, out, help_output);

	free(out);
	out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[a: Hello World!]\n"
			    "[a]\n",
			    &out, "--help", NULL);

	VUNIT_ASSERT_STREQ(ctx, out, help_output);
}

VUNIT_TEST_SUITE("suite", { "Test help flag", test_output_flag }, {}, )
