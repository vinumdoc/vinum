#include <vunit/vunit.h>

#include "test_helpers.h"

// Shots are shot aliases reserved for the Vinum Standard Library: only symbols
// starting with an alphabetic character can be user-defined, so `=`, `#` and
// their targets (`eval`, `comment`) are VSL-exclusive. These tests therefore
// call VSL by name on purpose -- we cannot stand in for them with user-defined
// functions without changing what is being tested.

void test_eval_shot_with_spaces(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[= 1 + 2]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "3");
}

void test_eval_shot_without_spaces(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[=1+2]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "3");
}

void test_comment_function(struct vunit_test_ctx *ctx) {
	char *out = compile_program(
		ctx, "[comment This text won't appear in the final document]\nAnother text.");

	VUNIT_ASSERT_STREQ(ctx, out, "Another text.");
}

void test_comment_hashtag_shot(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[# Hashtag is an alias for comment]\nAnother text.");

	VUNIT_ASSERT_STREQ(ctx, out, "Another text.");
}

void test_comment_shot_no_blank(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[###Don't worry about leading blanks]\nAnother text.");

	VUNIT_ASSERT_STREQ(ctx, out, "Another text.");
}

void test_argument_shot_bracketed(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[about : Common features of [*] wine]\n"
					 "[about Merlot]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Common features of Merlot wine");
}

void test_argument_shot_dollar(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[about : Common features of $* wine]\n"
					 "[about Merlot]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Common features of Merlot wine");
}

struct vunit_test tests[] = {
	{ .name = "eval shot with spaces [= 1 + 2]",
	  .test_func = test_eval_shot_with_spaces,
	  .todo = true,
	  .todo_msg = "[= ...] eval shot (VSL) not implemented" },
	{ .name = "eval shot without spaces [=1+2]",
	  .test_func = test_eval_shot_without_spaces,
	  .todo = true,
	  .todo_msg = "[=...] eval shot without separator not implemented" },
	{ .name = "comment function expands to nothing",
	  .test_func = test_comment_function,
	  .todo = true,
	  .todo_msg = "[comment ...] VSL function not implemented" },
	{ .name = "hashtag shot is an alias for comment",
	  .test_func = test_comment_hashtag_shot,
	  .todo = true,
	  .todo_msg = "[# ...] comment shot alias not implemented" },
	{ .name = "Shot needs no blank separator (hashes)",
	  .test_func = test_comment_shot_no_blank,
	  .todo = true,
	  .todo_msg = "shots without blank separator not implemented" },
	{ .name = "[*] dry shot for argument",
	  .test_func = test_argument_shot_bracketed,
	  .todo = true,
	  .todo_msg = "[*] argument shot not implemented" },
	{ .name = "$* sweet shot for argument inside dry block",
	  .test_func = test_argument_shot_dollar },
	{},
};

VUNIT_TEST_SUITE(tests)
