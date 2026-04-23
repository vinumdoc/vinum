#include <vunit/vunit.h>

#include "test_helpers.h"

void test_scope_alias(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[chapter\n"
					 "[section\n"
					 "[title : Introduction]\n"
					 "[author: Winemaker]\n"
					 "[scope intro]\n"
					 "]\n"
					 "[par As we'll see in section [intro_title]...]\n"
					 "]\n");

	VUNIT_ASSERT_STREQ(ctx, out,
			   "[chapter [section [title : Introduction] ] "
			   "[par As we'll see in section Introduction...] ]");
}

void test_label_cross_scope_projection(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[chapter\n"
					 "[title : Famous grapes]\n"
					 "[label grapes]\n"
					 "[section\n"
					 "[title : Negramaro]\n"
					 "[label : neg]\n"
					 "[section\n"
					 "[paragraph In this section of chapter [grapes_title] "
					 "we'll complement the Section [grapes_neg_title]]\n"
					 "]\n"
					 "]\n"
					 "]\n");

	VUNIT_ASSERT_STREQ(ctx, out,
			   "[chapter [section [section [paragraph In this section of chapter "
			   "Famous grapes we'll complement the Section Negramaro] ] ] ]");
}

// TODO(codex): see overview_test.c -- cocktailling test strategy still needs to be agreed on.
// The expected outputs below assume a hypothetical markdown translation of [section]/[title].
void test_label_call_form_vs_definition_form(struct vunit_test_ctx *ctx) {
	char *out_call = NULL;
	char *out_def = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[section [label foo] [title : A]]\n"
			    "[foo_title]\n",
			    &out_call, NULL);

	vunit_run_vinumc_ok(ctx,
			    "[section [label : bar] [title : A]]\n"
			    "[bar_title]\n",
			    &out_def, NULL);

	VUNIT_ASSERT_STREQ(ctx, out_call, "# A\n\nA\n");
	VUNIT_ASSERT_STREQ(ctx, out_def, "# A\n\nA\n");
}

struct vunit_test tests[] = {
	{ .name = "scope creates an alias for the current scope",
	  .test_func = test_scope_alias,
	  .todo = true,
	  .todo_msg = "[scope ...] alias not implemented" },
	{ .name = "label enables cross-scope projection",
	  .test_func = test_label_cross_scope_projection,
	  .todo = true,
	  .todo_msg = "[label ...] cross-scope projection not implemented" },
	{ .name = "label as call and as definition both work",
	  .test_func = test_label_call_form_vs_definition_form,
	  .todo = true,
	  .todo_msg = "depends on cocktailling/translation document strategy still TBD" },
	{},
};

VUNIT_TEST_SUITE(tests)
