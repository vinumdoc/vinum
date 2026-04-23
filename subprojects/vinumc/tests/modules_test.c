#include <vunit/vunit.h>

#include "test_helpers.h"

void test_import_brings_globals(struct vunit_test_ctx *ctx) {
	vunit_str_to_file(ctx, "wine.vin",
			  "[winegrape : Vitis Vinifera]\n"
			  "[shout : *** [argument] ***]\n");

	char *out =
		compile_program(ctx, "[import wine.vin]\n"
				     "[section [title: Introduction]\n"
				     "Grapes from the species [winegrape] make traditional wines.\n"
				     "But, beware, [shout if you drink, don't drive].\n"
				     "]\n");

	VUNIT_ASSERT_STREQ(
		ctx, out,
		"[section [title: Introduction] Grapes from the species Vitis Vinifera "
		"make traditional wines. But, beware, *** if you drink, don't drive ***. ]");
}

void test_import_duplicate_keeps_original(struct vunit_test_ctx *ctx) {
	vunit_str_to_file(ctx, "wine.vin", "[winegrape : Vitis Vinifera]\n");

	char *out = NULL;
	char *err = NULL;

	int ret = vunit_run_vinumc(ctx,
				   "[winegrape : Kept]\n"
				   "[import wine.vin]\n"
				   "[winegrape]\n",
				   &out, &err, NULL, 0);

	VUNIT_ASSERT_EQ(ctx, ret, 0);
	VUNIT_ASSERT_STREQ(ctx, out, "Kept");
}

void test_overwrite_replaces_symbols(struct vunit_test_ctx *ctx) {
	vunit_str_to_file(ctx, "wine.vin", "[winegrape : Vitis Vinifera]\n");

	char *out = compile_program(ctx, "[winegrape : Original]\n"
					 "[overwrite wine.vin]\n"
					 "[winegrape]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Vitis Vinifera");
}

void test_import_with_as(struct vunit_test_ctx *ctx) {
	vunit_str_to_file(ctx, "wine.vin",
			  "[winegrape : Vitis Vinifera]\n"
			  "[shout : *** [argument] ***]\n");

	char *out = compile_program(ctx, "[import wine.vin [as : wine]]\n"
					 "[wine_winegrape] / [wine_shout please]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Vitis Vinifera / *** please ***");
}

void test_import_copies_reference_blocks(struct vunit_test_ctx *ctx) {
	vunit_str_to_file(ctx, "wine.vin",
			  "[winegrape : Vitis Vinifera]\n"
			  "[shout : *** [argument] ***]\n"
			  "hello world\n");

	char *out = compile_program(ctx, "[import wine.vin]\n"
					 "[winegrape]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "hello world Vitis Vinifera");
}

void test_include_verbatim(struct vunit_test_ctx *ctx) {
	vunit_str_to_file(ctx, "snippet.vin",
			  "[wine : Merlot]\n"
			  "[wine] is tasty\n");

	char *out = compile_program(ctx, "Today: [include snippet.vin].\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Today: Merlot is tasty.");
}

void test_include_composes_sections(struct vunit_test_ctx *ctx) {
	vunit_str_to_file(ctx, "section1.vin", "[section [title: First] one]\n");
	vunit_str_to_file(ctx, "section2.vin", "[section [title: Second] two]\n");

	char *out = compile_program(ctx, "[chapter\n"
					 "[include section1.vin]\n"
					 "[include section2.vin]\n"
					 "]\n");

	VUNIT_ASSERT_STREQ(ctx, out,
			   "[chapter [section [title: First] one] "
			   "[section [title: Second] two] ]");
}

struct vunit_test tests[] = {
	{ .name = "import brings module globals",
	  .test_func = test_import_brings_globals,
	  .todo = true,
	  .todo_msg = "[import ...] not implemented" },
	{ .name = "import keeps original on duplicate symbol",
	  .test_func = test_import_duplicate_keeps_original,
	  .todo = true,
	  .todo_msg = "[import ...] duplicate-keep semantics not implemented" },
	{ .name = "overwrite replaces existing symbols",
	  .test_func = test_overwrite_replaces_symbols,
	  .todo = true,
	  .todo_msg = "[overwrite ...] not implemented" },
	{ .name = "import ... [as : wine] creates namespace",
	  .test_func = test_import_with_as,
	  .todo = true,
	  .todo_msg = "[import ... [as : ...]] namespacing not implemented" },
	{ .name = "import also copies non-definition blocks",
	  .test_func = test_import_copies_reference_blocks,
	  .todo = true,
	  .todo_msg = "[import ...] copy of non-definition blocks not implemented" },
	{ .name = "include copies content verbatim",
	  .test_func = test_include_verbatim,
	  .todo = true,
	  .todo_msg = "[include ...] not implemented" },
	{ .name = "include composes section files into chapter",
	  .test_func = test_include_composes_sections,
	  .todo = true,
	  .todo_msg = "[include ...] composition not implemented" },
	{},
};

VUNIT_TEST_SUITE(tests)
