#include <vunit.h>
#include <stdlib.h>

void test_output_flag(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
		"[a: Hello World!]\n"
		"[a]\n",
		&out,
		"-o",
		"out.txt",
		NULL
	);

	char *out_file_str = vunit_file_to_str(ctx, "out.txt");
	VUNIT_ASSERT_STREQ(ctx, out_file_str, "Hello World!\n");
	VUNIT_ASSERT_STREQ(ctx, out, "");

	free(out_file_str);
	free(out);

	vunit_run_vinumc_ok(ctx,
		"[a: Hello World!]\n"
		"[a]\n",
		&out,
		"--output",
		"out.txt",
		NULL
	);

	out_file_str = vunit_file_to_str(ctx, "out.txt");
	VUNIT_ASSERT_STREQ(ctx, out, "");

	VUNIT_ASSERT_STREQ(ctx, out_file_str, "Hello World!\n");
}

VUNIT_TEST_SUITE("suite",
	{"Test output flag", test_output_flag},
	{},
)
