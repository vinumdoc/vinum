#include <vunit/vunit.h>

#include "test_helpers.h"

void test_blank_reduction_across_newline(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[ brunello : Brunello di\n"
					 "Montalcino ]\n"
					 "Try [brunello], a great wine from Tuscany.\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Try Brunello di Montalcino, a great wine from Tuscany.");
}

void test_leading_trailing_blanks_suppressed(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[a :   padded value   ]\n"
					 "|[a]|\n");

	VUNIT_ASSERT_STREQ(ctx, out, "|padded value|");
}

void test_symbol_field_blanks_ignored(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[   wine   : Merlot]\n"
					 "[wine]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Merlot");
}

struct vunit_test tests[] = {
	{ .name = "Blank reduction collapses newlines into one space",
	  .test_func = test_blank_reduction_across_newline,
	  .todo = true,
	  .todo_msg = "blank reduction across newlines not implemented" },
	{ .name = "Leading and trailing blanks are suppressed",
	  .test_func = test_leading_trailing_blanks_suppressed,
	  .todo = true,
	  .todo_msg = "leading/trailing blank suppression not implemented" },
	{ .name = "Blanks around symbol field are ignored",
	  .test_func = test_symbol_field_blanks_ignored },
	{},
};

VUNIT_TEST_SUITE(tests)
