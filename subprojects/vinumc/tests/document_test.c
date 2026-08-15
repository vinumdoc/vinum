#include <vunit/vunit.h>

#include "test_helpers.h"

void test_document_scope_empty_symbol(struct vunit_test_ctx *ctx) {
	char *out =
		compile_program(ctx, "[title : All about wine]\n"
				     "[section\n"
				     "[title : Wine in mythology]\n"
				     "[paragraph In this article entitled [_title], the reader is "
				     "reminded that wine is profusely mentioned in the mythology "
				     "of many cultures since ancient times.\n"
				     "]\n"
				     "]\n");

	VUNIT_ASSERT_STREQ(ctx, out,
			   "[section [title : Wine in mythology] "
			   "[paragraph In this article entitled All about wine, the reader "
			   "is reminded that wine is profusely mentioned in the mythology "
			   "of many cultures since ancient times. ] ]");
}

void test_global_equivalent_to_root_prefix(struct vunit_test_ctx *ctx) {
	char *out_global = NULL;
	char *out_root = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[global whitewine]\n"
			    "[whitewine : white wine]\n"
			    "[_whitewine]\n",
			    &out_global, NULL);

	vunit_run_vinumc_ok(ctx,
			    "[_whitewine : white wine]\n"
			    "[_whitewine]\n",
			    &out_root, NULL);

	VUNIT_ASSERT_STREQ(ctx, out_global, "white wine");
	VUNIT_ASSERT_STREQ(ctx, out_root, "white wine");
}

void test_label_cross_section_reference(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[section [label intro] [title : Introduction]]\n"
					 "[section\n"
					 "As seeing in section [intro_title]\n"
					 "]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "[section ] [section As seeing in section Introduction ]");
}

void test_fqn_stored_as_label(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[section\n"
					 "[_intro : [fqn]]\n"
					 "[title : Introduction]\n"
					 "]\n"
					 "[_intro]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "_section1");
}

void test_doc_editor_function(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[title : My Page]\n"
					 "[doc:\n"
					 "<html lang=\"en\">\n"
					 "<head>\n"
					 "<title> [title] </title>\n"
					 "</head>\n"
					 "<body>\n"
					 "[content]\n"
					 "</body>\n"
					 "</html>\n"
					 "]\n"
					 "Hello there.\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Hello there.");
}

struct vunit_test tests[] = {
	{ .name = "Document scope exposes _-prefixed fully qualified names",
	  .test_func = test_document_scope_empty_symbol,
	  .todo = true,
	  .todo_msg = "_-prefixed FQN access at document scope not implemented" },
	{ .name = "global and _-prefix are equivalent",
	  .test_func = test_global_equivalent_to_root_prefix,
	  .todo = true,
	  .todo_msg = "[global ...] / _-prefix equivalence not implemented" },
	{ .name = "label creates a cross-section reference",
	  .test_func = test_label_cross_section_reference,
	  .todo = true,
	  .todo_msg = "[label ...] cross-section reference not implemented" },
	{ .name = "fqn exposes the current fully qualified name",
	  .test_func = test_fqn_stored_as_label,
	  .todo = true,
	  .todo_msg = "[fqn] automatic not implemented" },
	{ .name = "[doc:] editor function defers to cocktailing",
	  .test_func = test_doc_editor_function,
	  .todo_msg = "[doc:] editor function / cocktailing not implemented" },
	{},
};

VUNIT_TEST_SUITE(tests)
