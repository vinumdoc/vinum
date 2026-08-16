#include <vunit/vunit.h>

#include "test_helpers.h"

void test_if_truthy(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[if 1 [then: A] [else: B]]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "A");
}

void test_if_empty_is_false(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[empty :]\n"
					 "[if [empty] [then: A] [else: B]]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "B");
}

void test_if_zero_is_false(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[if 0 [then: A] [else: B]]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "B");
}

void test_if_with_user_predicate(struct vunit_test_ctx *ctx) {
	// Stand-in for the Codex's [is_workday] example: use a user-defined flag
	// so the test is deterministic and stays independent from VSL.
	char *out = compile_program(
		ctx, "[workday : 1]\n"
		     "Order your ticket [if [workday] [then: today] [else: by Monday]]!\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Order your ticket today!");
}

struct vunit_test tests[] = {
	{ .name = "if with truthy expression picks then",
	  .test_func = test_if_truthy,
	  .todo = true,
	  .todo_msg = "[if ...] control structure not implemented" },
	{ .name = "if with empty expression picks else",
	  .test_func = test_if_empty_is_false,
	  .todo = true,
	  .todo_msg = "[if ...] control structure not implemented" },
	{ .name = "if with 0 picks else",
	  .test_func = test_if_zero_is_false,
	  .todo = true,
	  .todo_msg = "[if ...] control structure not implemented" },
	{ .name = "if branching on a user-defined predicate",
	  .test_func = test_if_with_user_predicate,
	  .todo = true,
	  .todo_msg = "[if ...] control structure not implemented" },
	{},
};

VUNIT_TEST_SUITE(tests)
