#include <vunit/vunit.h>

#include "test_helpers.h"

// Deferral is a core language concept, but the Codex illustrates it with VSL
// author functions like [first] and [count]. These tests stand in for them
// with user-defined functions so only the language semantics are under test.

void test_nested_author_functions(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[winegrape : Vitis vinifera]\n"
					 "[echo : [argument]]\n"
					 "[echo [winegrape]]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Vitis vinifera");
}

void test_editor_function_preserved_through_vinification(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[defer foreign]\n"
					 "[winegrape : [foreign Vitis vinifera]]\n"
					 "[winegrape] originates from the Near East\n");

	VUNIT_ASSERT_STREQ(ctx, out, "[foreign Vitis vinifera] originates from the Near East");
}

void test_scope_lifting_preserves_wrapper(struct vunit_test_ctx *ctx) {
	char *out = compile_program(
		ctx, "[defer foreign]\n"
		     "[winegrape : [foreign Vitis vinifera]]\n"
		     "[echo : [argument]]\n"
		     "Other species of [echo [winegrape]] are mainly used for breeding.\n");

	VUNIT_ASSERT_STREQ(
		ctx, out,
		"Other species of [foreign Vitis vinifera] are mainly used for breeding.");
}

void test_discard_policy_drops_editor_wrapper(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[defer foreign]\n"
					 "[strip : [policy : discard][argument]]\n"
					 "[strip [foreign a priori] [foreign in locus]]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "a priori in locus");
}

void test_user_function_preserves_nested_editor(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[defer foreign]\n"
					 "[my : [argument]]\n"
					 "[my [foreign Vitis vinifera]]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "[foreign Vitis vinifera]");
}

void test_policy_discard_declaration(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[defer foreign]\n"
					 "[plain : [policy : discard][argument]]\n"
					 "[plain [foreign Vitis vinifera]]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Vitis vinifera");
}

struct vunit_test tests[] = {
	{ .name = "Plain nested user functions use applicative order",
	  .test_func = test_nested_author_functions,
	  .todo = true,
	  .todo_msg = "[argument] expansion in nested user functions not implemented" },
	{ .name = "Editor function is preserved across vinification",
	  .test_func = test_editor_function_preserved_through_vinification,
	  .todo = true,
	  .todo_msg = "[defer ...] editor function preservation not implemented" },
	{ .name = "Scope lifting preserves the editor wrapper",
	  .test_func = test_scope_lifting_preserves_wrapper,
	  .todo = true,
	  .todo_msg = "[defer ...] scope lifting not implemented" },
	{ .name = "[policy : discard] drops deferred editor wrappers from the argument",
	  .test_func = test_discard_policy_drops_editor_wrapper,
	  .todo = true,
	  .todo_msg = "[policy : discard] not implemented" },
	{ .name = "User-defined functions default to preserve policy",
	  .test_func = test_user_function_preserves_nested_editor,
	  .todo = true,
	  .todo_msg = "default preserve policy for user functions not implemented" },
	{ .name = "[policy : discard] explicitly set inside a user function",
	  .test_func = test_policy_discard_declaration,
	  .todo = true,
	  .todo_msg = "[policy : discard] inside user function not implemented" },
	{},
};

VUNIT_TEST_SUITE(tests)
