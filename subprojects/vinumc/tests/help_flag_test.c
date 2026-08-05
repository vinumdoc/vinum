#include <stdlib.h>

#include <vunit/vunit.h>

void test_output_flag(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[a: Hello World!]\n"
			    "[a]\n",
			    &out, "-h", NULL);

	const char *help_output =
		"usage: vinumc [-o | --output <output_path>] [-h | --help] [-w | "
		"--with <libraries>] [-e | --effect <cocktailing_libraries>] [-v | --version] "
		"[<file-input>]\n"
		"\n"
		"Options:\n"
		"  -o, --output\n"
		"	Set file name output, if not set the output will be stdout\n"
		"\n"
		"  -h, --help\n"
		"	Show help\n"
		"\n"
		"  -w, --with (can be specified multiple times)\n"
		"	Set a library to be loaded\n"
		"\n"
		"  -e, --effect (can be specified multiple times)\n"
		"	Set a vinum cocktailing library to be loaded\n"
		"\n"
		"  -v, --version\n"
		"	Show the current vinumc version\n";

	VUNIT_ASSERT_STREQ(ctx, out, help_output);

	out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[a: Hello World!]\n"
			    "[a]\n",
			    &out, "--help", NULL);

	VUNIT_ASSERT_STREQ(ctx, out, help_output);
}

struct vunit_test tests[] = {
	{ .name = "Test help flag", .test_func = test_output_flag },
	{},
};

VUNIT_TEST_SUITE(tests)
