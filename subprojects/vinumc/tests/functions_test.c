#include <vunit/vunit.h>

#include "test_helpers.h"

void test_simple_definition(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[winegrape : Vitis vinifera]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "");
}

void test_reference_after_definition(struct vunit_test_ctx *ctx) {
	char *out = compile_program(
		ctx, "[winegrape : Vitis vinifera]\n"
		     "Most traditional wines are made of grapes from [winegrape].\n");

	VUNIT_ASSERT_STREQ(ctx, out,
			   "Most traditional wines are made of grapes from Vitis vinifera.");
}

void test_optional_argument_binding(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[brief: [argument] makes an amazing [color] wine]\n"
					 "[brief Muscat [color: white] ].\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Muscat makes an amazing white wine.");
}

void test_nested_function_call(struct vunit_test_ctx *ctx) {
	char *out = compile_program(
		ctx,
		"[xmas : 2024-12-25]\n"
		"[weekday : day([argument])]\n"
		"I expect you next [weekday [xmas]] for a glass of wine and a special meal!\n");

	VUNIT_ASSERT_STREQ(
		ctx, out,
		"I expect you next day(2024-12-25) for a glass of wine and a special meal!");
}

void test_redefinition(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[wine : Merlot]\n"
					 "[wine : Cabernet]\n"
					 "[wine]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Cabernet");
}

struct vunit_test tests[] = {
	{ .name = "Definition block expands to nothing", .test_func = test_simple_definition },
	{ .name = "Reference after definition",
	  .test_func = test_reference_after_definition,
	  .todo = true,
	  .todo_msg = "symbol reference resolution not implemented" },
	{ .name = "Optional argument via nested binding",
	  .test_func = test_optional_argument_binding,
	  .todo = true,
	  .todo_msg = "nested optional bindings not implemented" },
	{ .name = "Nested function call expanding argument",
	  .test_func = test_nested_function_call,
	  .todo = true,
	  .todo_msg = "argument expansion in nested calls not implemented" },
	{ .name = "Redefinition: last binding wins",
	  .test_func = test_redefinition,
	  .todo = true,
	  .todo_msg = "symbol redefinition (last-wins) not implemented" },
	{},
};

VUNIT_TEST_SUITE(tests)
