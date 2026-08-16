#include <vunit/vunit.h>

#include "test_helpers.h"

void test_winegrape_definition(struct vunit_test_ctx *ctx) {
	char *out = compile_program(
		ctx, "[winegrape : Vitis vinifera]\n"
		     "Most traditional wines are made of grapes from [winegrape].\n");

	VUNIT_ASSERT_STREQ(ctx, out,
			   "Most traditional wines are made of grapes from Vitis vinifera.");
}

void test_nested_first(struct vunit_test_ctx *ctx) {
	char *out = compile_program(
		ctx, "[winegrape : Vitis vinifera]\n"
		     "[describe : wrap([argument])]\n"
		     "Most famous wine grapes belong to the genus [describe [winegrape]].\n");

	VUNIT_ASSERT_STREQ(ctx, out,
			   "Most famous wine grapes belong to the genus wrap(Vitis vinifera).");
}

void test_user_function_with_argument(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[shout : *** [argument] ***]\n"
					 "[shout If you drink, don't drive]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "*** If you drink, don't drive ***");
}

// TODO(codex): these tests assume vunit_run_vinumc_ok uses an embedded translation document
// when resolving deferred blocks. That is not currently the case, so we still need to decide:
// - Should vunit_run_vinumc_ok resolve deferred blocks on its own?
// - Should cocktailling be exercised from a dedicated test file instead?
// One possibility is to let callers pass a translation document into vunit_run_vinumc_ok.
// The expected outputs below are written against a hypothetical markdown translation.
void test_section_with_paragraphs(struct vunit_test_ctx *ctx) {
	// See TODO above: cocktailling test strategy still needs to be agreed on.
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[section\n"
			    "[paragraph Red wines are rich in tannin]\n"
			    "[paragraph Tannin contributes to define the wine structure]\n"
			    "]\n",
			    &out, NULL);

	VUNIT_ASSERT_STREQ(ctx, out,
			   "Red wines are rich in tannin\n"
			   "\n"
			   "Tannin contributes to define the wine structure\n");
}

void test_sec_par_aliases(struct vunit_test_ctx *ctx) {
	// See TODO above: cocktailling test strategy still needs to be agreed on.
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[sec [title: Chianti valley]\n"
			    "[par It's the historic heartland of the Sangiovese grape.]\n"
			    "]\n",
			    &out, NULL);

	VUNIT_ASSERT_STREQ(ctx, out,
			   "# Chianti valley\n"
			   "\n"
			   "It's the historic heartland of the Sangiovese grape.\n");
}

void test_defer_declaration(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[defer footnote]\n"
			    "[footnote A side remark]\n",
			    &out, NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "[footnote A side remark]");
}

void test_html_translation(struct vunit_test_ctx *ctx) {
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[section: <h2> [title] </h2> [argument] ]\n"
			    "[paragraph: <p> [argument] </p>]\n"
			    "[section [title: Chianti valley]\n"
			    "[paragraph It's the historic heartland of the Sangiovese grape.]\n"
			    "]\n",
			    &out, NULL);

	VUNIT_ASSERT_STREQ(ctx, out,
			   "<h2> Chianti valley </h2> "
			   "<p> It's the historic heartland of the Sangiovese grape. </p>");
}

void test_latex_translation(struct vunit_test_ctx *ctx) {
	// TODO(codex): Codex writes `\section` literally, but Vinum's backslash escape
	// reduces `\s` to `s`; doubling the backslash preserves the intended LaTeX output.
	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[section: \\\\section{[title]} [argument] ]\n"
			    "[paragraph: \\\\par [argument] ]\n"
			    "[section [title: Chianti valley]\n"
			    "[paragraph It's the historic heartland of the Sangiovese grape.]\n"
			    "]\n",
			    &out, NULL);

	VUNIT_ASSERT_STREQ(ctx, out,
			   "\\section{Chianti valley} "
			   "\\par It's the historic heartland of the Sangiovese grape.");
}

struct vunit_test tests[] = {
	{ .name = "winegrape definition and reference",
	  .test_func = test_winegrape_definition,
	  .todo = true,
	  .todo_msg = "symbol reference resolution not implemented" },
	{ .name = "Nested user function expanding a reference",
	  .test_func = test_nested_first,
	  .todo = true,
	  .todo_msg = "argument automatic not expanded inside user functions" },
	{ .name = "User function with argument automatic",
	  .test_func = test_user_function_with_argument,
	  .todo = true,
	  .todo_msg = "argument automatic not expanded inside user functions" },
	{ .name = "section with two paragraph blocks",
	  .test_func = test_section_with_paragraphs,
	  .todo = true,
	  .todo_msg = "depends on cocktailling/translation document strategy still TBD" },
	{ .name = "sec/par short aliases",
	  .test_func = test_sec_par_aliases,
	  .todo = true,
	  .todo_msg = "depends on cocktailling/translation document strategy still TBD" },
	{ .name = "defer declares an editor function",
	  .test_func = test_defer_declaration,
	  .todo = true,
	  .todo_msg = "defer/editor function preservation not implemented" },
	{ .name = "HTML translation document definitions",
	  .test_func = test_html_translation,
	  .todo = true,
	  .todo_msg = "translation document/[argument] expansion not implemented" },
	{ .name = "LaTeX translation document definitions",
	  .test_func = test_latex_translation,
	  .todo = true,
	  .todo_msg = "translation document/[argument] expansion not implemented" },
	{},
};

VUNIT_TEST_SUITE(tests)
