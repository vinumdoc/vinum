#include <vunit/vunit.h>

void test_generic_directive_parses(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "<<directive argument>>\n"
			    "hello\n",
			    &out, NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "hello");
}

void test_warn_undefined_off(struct vunit_test_ctx *ctx) {
	char *out = NULL;
	char *err = NULL;

	int ret = vunit_run_vinumc(ctx,
				   "<<warn_undefined off>>\n"
				   "[nonexistent]\n",
				   &out, &err, NULL, 0);

	VUNIT_ASSERT_EQ(ctx, ret, 0);
	VUNIT_ASSERT_STREQ(ctx, err, "");
}

void test_warn_undefined_on_emits_warning(struct vunit_test_ctx *ctx) {
	char *out = NULL;
	char *err = NULL;

	int ret = vunit_run_vinumc(ctx,
				   "<<warn_undefined off>>\n"
				   "[nonexistent_a]\n"
				   "<<warn_undefined on>>\n"
				   "[nonexistent_b]\n",
				   &out, &err, NULL, 0);

	VUNIT_ASSERT_EQ(ctx, ret, 0);
	VUNIT_ASSERT_NEQ(ctx, err[0], '\0');
}

void test_warn_as_errors_exits_nonzero(struct vunit_test_ctx *ctx) {
	char *out = NULL;
	char *err = NULL;

	int ret = vunit_run_vinumc(ctx,
				   "<<warn_as_errors>>\n"
				   "[nonexistent]\n",
				   &out, &err, NULL, 0);

	VUNIT_ASSERT_NEQ(ctx, ret, 0);
}

struct vunit_test tests[] = {
	{ .name = "Generic <<directive>> does not crash the compiler",
	  .test_func = test_generic_directive_parses,
	  .todo = true,
	  .todo_msg = "<<directive>> generic preprocessor parsing not implemented" },
	{ .name = "<<warn_undefined off>> suppresses warnings",
	  .test_func = test_warn_undefined_off,
	  .todo = true,
	  .todo_msg = "<<warn_undefined off>> directive not implemented" },
	{ .name = "<<warn_undefined on>> re-enables warnings",
	  .test_func = test_warn_undefined_on_emits_warning },
	{ .name = "<<warn_as_errors>> causes non-zero exit",
	  .test_func = test_warn_as_errors_exits_nonzero,
	  .todo = true,
	  .todo_msg = "<<warn_as_errors>> directive not implemented" },
	{},
};

VUNIT_TEST_SUITE(tests)
