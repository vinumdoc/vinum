#include <vunit/vunit.h>

#include "test_helpers.h"

void test_macro_cli_flag(struct vunit_test_ctx *ctx) {
	vunit_str_to_file(ctx, "my_macros.txt", "e.g. : [foreign e.g.]\n");

	char *out = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[defer foreign]\n"
			    "Use e.g. to introduce examples.\n",
			    &out, "--macro", "my_macros.txt", NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "Use [foreign e.g.] to introduce examples.");
}

void test_macro_inline_directive(struct vunit_test_ctx *ctx) {
	vunit_str_to_file(ctx, "my_macros.txt", "e.g. : [foreign e.g.]\n");

	char *out = compile_program(ctx, "[defer foreign]\n"
					 "<<macro my_macros.txt>>\n"
					 "Use e.g. to introduce examples.\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Use [foreign e.g.] to introduce examples.");
}

void test_macro_regex_backreference(struct vunit_test_ctx *ctx) {
	vunit_str_to_file(ctx, "editor_macros.txt", "(\\w+)z : \\1s\n");

	char *out = NULL;

	vunit_run_vinumc_ok(ctx, "The grape Shiraz is also called Syrah.\n", &out, "--macro",
			    "editor_macros.txt", NULL);

	VUNIT_ASSERT_STREQ(ctx, out, "The grape Shiras is also called Syrah.");
}

void test_macro_bypass_via_literal(struct vunit_test_ctx *ctx) {
	// TODO(codex): spec mismatch -- Codex uses `{ ... }`, the implementation uses `{# ... #}`
	char *out = compile_program(ctx, "[defer ordinal]\n"
					 "{# 1st #} is for 1st\n");

	VUNIT_ASSERT_STREQ(ctx, out, "1st is for [ordinal 1]");
}

void test_macro_category_off(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "<<macro_ordinal : off>>\n"
					 "The 1st grape.\n");

	VUNIT_ASSERT_STREQ(ctx, out, "The 1st grape.");
}

void test_macro_all_off(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "<<macro : off>>\n"
					 "1st, 2nd, e.g. stays.\n");

	VUNIT_ASSERT_STREQ(ctx, out, "1st, 2nd, e.g. stays.");
}

struct vunit_test tests[] = {
	{ .name = "--macro CLI flag applies replacement file",
	  .test_func = test_macro_cli_flag,
	  .todo = true,
	  .todo_msg = "--macro CLI flag not implemented" },
	{ .name = "<<macro ...>> inline directive applies replacement file",
	  .test_func = test_macro_inline_directive,
	  .todo = true,
	  .todo_msg = "<<macro ...>> directive not implemented" },
	{ .name = "Macro replacement supports PCRE backreferences",
	  .test_func = test_macro_regex_backreference,
	  .todo = true,
	  .todo_msg = "macro PCRE backreferences not implemented" },
	{ .name = "Literal block bypasses macro replacement",
	  .test_func = test_macro_bypass_via_literal,
	  .todo = true,
	  .todo_msg = "{# ... #} literal block + macros not implemented" },
	{ .name = "<<macro_ordinal : off>> disables only the ordinal category",
	  .test_func = test_macro_category_off,
	  .todo = true,
	  .todo_msg = "<<macro_ordinal : off>> directive not implemented" },
	{ .name = "<<macro : off>> disables all macro expansions",
	  .test_func = test_macro_all_off,
	  .todo = true,
	  .todo_msg = "<<macro : off>> directive not implemented" },
	{},
};

VUNIT_TEST_SUITE(tests)
