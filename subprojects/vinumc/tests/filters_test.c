#include <vunit/vunit.h>

#include "test_helpers.h"

// Filters test the pipe-versus-nested equivalence. Per Codex Vinum, [x | f | g]
// is sugar for [g [f [x]]]. We stand in for the standard library wrappers with
// user-defined functions so the tests only depend on core language semantics.

void test_nested_wrappers(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[winegrape : Vitis vinifera]\n"
					 "[wa : a([argument])]\n"
					 "[wb : b([argument])]\n"
					 "[wa [winegrape]] is the genus of [wb [winegrape]]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "a(Vitis vinifera) is the genus of b(Vitis vinifera)");
}

void test_deeply_nested_pipeline(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[winegrape : Vitis vinifera]\n"
					 "[wa : a([argument])]\n"
					 "[wb : b([argument])]\n"
					 "[wc : c([argument])]\n"
					 "[wc [wb [wa [winegrape]]]]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "c(b(a(Vitis vinifera)))");
}

void test_pipe_form(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[winegrape : Vitis vinifera]\n"
					 "[wa : a([argument])]\n"
					 "[wb : b([argument])]\n"
					 "[wc : c([argument])]\n"
					 "[winegrape | wa | wb | wc]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "c(b(a(Vitis vinifera)))");
}

void test_pipe_with_explicit_argument(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[wa : a([argument])]\n"
					 "[wb : b([argument])]\n"
					 "[wa Vitis vinifera | wb]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "b(a(Vitis vinifera))");
}

void test_mixed_nested_vs_pipe(struct vunit_test_ctx *ctx) {
	char *out_nested = NULL;
	char *out_pipe = NULL;

	vunit_run_vinumc_ok(ctx,
			    "[winegrape : Vitis vinifera]\n"
			    "[wa : a([argument])]\n"
			    "[wb : b([argument])]\n"
			    "[wb [wa [winegrape]]]\n",
			    &out_nested, NULL);

	vunit_run_vinumc_ok(ctx,
			    "[winegrape : Vitis vinifera]\n"
			    "[wa : a([argument])]\n"
			    "[wb : b([argument])]\n"
			    "[winegrape | wa | wb]\n",
			    &out_pipe, NULL);

	VUNIT_ASSERT_STREQ(ctx, out_nested, out_pipe);
}

struct vunit_test tests[] = {
	{ .name = "Nested user-defined wrappers",
	  .test_func = test_nested_wrappers,
	  .todo = true,
	  .todo_msg = "argument expansion in nested user wrappers not implemented" },
	{ .name = "Deeply nested pipeline with three wrappers",
	  .test_func = test_deeply_nested_pipeline,
	  .todo = true,
	  .todo_msg = "argument expansion in nested user wrappers not implemented" },
	{ .name = "Pipe form is equivalent to nested form",
	  .test_func = test_pipe_form,
	  .todo = true,
	  .todo_msg = "pipe operator [x | f | g] not implemented" },
	{ .name = "Pipe with explicit argument on leftmost function",
	  .test_func = test_pipe_with_explicit_argument,
	  .todo = true,
	  .todo_msg = "pipe operator with explicit argument not implemented" },
	{ .name = "Nested and pipe forms produce the same output",
	  .test_func = test_mixed_nested_vs_pipe,
	  .todo = true,
	  .todo_msg = "pipe operator [x | f | g] not implemented" },
	{},
};

VUNIT_TEST_SUITE(tests)
