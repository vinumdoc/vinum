#include <vunit/vunit.h>

#include "test_helpers.h"

void test_backslash_escapes_bracket(struct vunit_test_ctx *ctx) {
	char *out =
		compile_program(ctx, "[wine : Merlot]\n"
				     "I don't want \\[wine] to be interpreted as a function.\n");

	VUNIT_ASSERT_STREQ(ctx, out, "I don't want [wine] to be interpreted as a function.");
}

void test_backslash_before_regular_char(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[a: \\a \\b \\1]\n"
					 "[a]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "a b 1");
}

void test_literal_block_prevents_expansion(struct vunit_test_ctx *ctx) {
	// TODO(codex): spec mismatch -- Codex uses `{ ... }`, the implementation uses `{# ... #}`
	char *out = compile_program(ctx, "[wine : Merlot]\n"
					 "Here {# [wine] #} will not expand to [wine].\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Here [wine] will not expand to Merlot.");
}

void test_literal_preserves_lineation(struct vunit_test_ctx *ctx) {
	// TODO(codex): spec mismatch -- Codex uses `{ ... }`, the implementation uses `{# ... #}`
	char *out = compile_program(ctx, "{#\n"
					 "Vino, estrella terrestre,\n"
					 "vino, puro\n"
					 "hijo del sol,\n"
					 "de la tierra,\n"
					 "de la vida\n"
					 "#}\n");

	VUNIT_ASSERT_STREQ(ctx, out,
			   "\nVino, estrella terrestre,\n"
			   "vino, puro\n"
			   "hijo del sol,\n"
			   "de la tierra,\n"
			   "de la vida\n");
}

void test_literal_editor_function_preserves_block(struct vunit_test_ctx *ctx) {
	// TODO(codex): spec mismatch -- Codex uses `{ ... }`, the implementation uses `{# ... #}`
	char *out = compile_program(ctx, "[wine : Merlot]\n"
					 "[literal {# [wine] #} ]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "[literal {# [wine] #} ]");
}

void test_escape_the_escape_inside_literal(struct vunit_test_ctx *ctx) {
	// TODO(codex): spec mismatch -- Codex uses `{ ... }`, the implementation uses `{# ... #}`
	char *out = compile_program(ctx, "[grape : Syrah]\n"
					 "{# The block [grape] will expand to \\[grape] #}.\n");

	VUNIT_ASSERT_STREQ(ctx, out, "The block [grape] will expand to Syrah.");
}

struct vunit_test tests[] = {
	{ .name = "Backslash escapes an opening bracket",
	  .test_func = test_backslash_escapes_bracket,
	  .todo = true,
	  .todo_msg = "backslash escape of [ not implemented" },
	{ .name = "Backslash before a non-special character yields the character",
	  .test_func = test_backslash_before_regular_char },
	{ .name = "Literal block prevents inner expansion",
	  .test_func = test_literal_block_prevents_expansion,
	  .todo = true,
	  .todo_msg = "{# ... #} literal block not implemented" },
	{ .name = "Literal block preserves poem lineation",
	  .test_func = test_literal_preserves_lineation,
	  .todo = true,
	  .todo_msg = "{# ... #} literal block not implemented" },
	{ .name = "[literal ...] editor function survives vinification",
	  .test_func = test_literal_editor_function_preserves_block,
	  .todo = true,
	  .todo_msg = "[literal ...] editor function not implemented" },
	{ .name = "Escape-the-escape inside a literal block",
	  .test_func = test_escape_the_escape_inside_literal,
	  .todo = true,
	  .todo_msg = "{# ... #} literal block with backslash escape not implemented" },
	{},
};

VUNIT_TEST_SUITE(tests)
