#include <vunit/vunit.h>

#include "test_helpers.h"

void test_argument_automatic(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[shout : *** [argument] ***]\n"
					 "[shout If you drink, don't drive]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "*** If you drink, don't drive ***");
}

void test_automatic_empty_in_definition(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[a : argument was: [argument]]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "");
}

void test_symbol_automatic(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[par : [symbol] said: [argument]]\n"
					 "[par Red grapes]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "par said: Red grapes");
}

void test_count_automatic(struct vunit_test_ctx *ctx) {
	char *out =
		compile_program(ctx, "[item One]\n"
				     "[item Two]\n"
				     "[item Vinum animi speculum est [label foo]]\n"
				     "[par The proverb in item [foo_count] in the list above is "
				     "due to Alcuin of York (c. 735-804).]\n");

	VUNIT_ASSERT_STREQ(ctx, out,
			   "[item One] [item Two] [item Vinum animi speculum est ] "
			   "[par The proverb in item 3 in the list above is due to "
			   "Alcuin of York (c. 735-804).]");
}

void test_index_automatic_non_resetting(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[section\n"
					 "[footnote [label a] first]\n"
					 "]\n"
					 "[section\n"
					 "[footnote [label b] second]\n"
					 "]\n"
					 "[a_index] and [b_index]\n");

	VUNIT_ASSERT_STREQ(ctx, out,
			   "[section [footnote first] ] [section [footnote second] ] 1 and 2");
}

void test_content_reserved(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[echo : [content]]\n"
					 "[echo hello]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "");
}

struct vunit_test tests[] = {
	{ .name = "argument automatic inside a reference block",
	  .test_func = test_argument_automatic,
	  .todo = true,
	  .todo_msg = "[argument] automatic not expanded in references" },
	{ .name = "automatic variables are empty inside a definition",
	  .test_func = test_automatic_empty_in_definition },
	{ .name = "symbol automatic equals the block's name",
	  .test_func = test_symbol_automatic,
	  .todo = true,
	  .todo_msg = "[symbol] automatic not implemented" },
	{ .name = "count is the sequential block count in the scope",
	  .test_func = test_count_automatic,
	  .todo = true,
	  .todo_msg = "[count] automatic not implemented" },
	{ .name = "index is non-resetting across scopes",
	  .test_func = test_index_automatic_non_resetting,
	  .todo = true,
	  .todo_msg = "[index] automatic not implemented" },
	{ .name = "content is reserved and cannot recurse",
	  .test_func = test_content_reserved,
	  .todo = true,
	  .todo_msg = "[content] reserved automatic not implemented" },
	{},
};

VUNIT_TEST_SUITE(tests)
