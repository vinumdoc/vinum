#include <vunit/vunit.h>

#include "test_helpers.h"

void test_lazy_binding(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[vintage : 2025]\n"
					 "[merlot_year : [vintage]]\n"
					 "[vintage : 2030]\n"
					 "The vintage of [merlot_year] will be excellent.\n");

	VUNIT_ASSERT_STREQ(ctx, out, "The vintage of 2030 will be excellent.");
}

void test_declarative_last_wins(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[country : Germany]\n"
					 "Merlot is common in [country]\n"
					 "[country : Chile]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Merlot is common in Chile");
}

void test_eager_symbol_in_definition(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[grape : cabernet]\n"
					 "[[grape] : Cabernet Sauvignon]\n"
					 "[cabernet] is common in Chile\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Cabernet Sauvignon is common in Chile");
}

void test_eager_symbol_in_reference(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[grape : Merlot]\n"
					 "[wine : grape]\n"
					 "[[wine]] is also common in Chile\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Merlot is also common in Chile");
}

struct vunit_test tests[] = {
	{ .name = "Lazy binding keeps expression unevaluated",
	  .test_func = test_lazy_binding,
	  .todo = true,
	  .todo_msg = "lazy binding semantics not implemented" },
	{ .name = "Declarative scope: last definition wins",
	  .test_func = test_declarative_last_wins,
	  .todo = true,
	  .todo_msg = "declarative scope (last-wins) not implemented" },
	{ .name = "Eager symbol expansion in definition block",
	  .test_func = test_eager_symbol_in_definition,
	  .todo = true,
	  .todo_msg = "eager [[symbol]] expansion in definition keys not implemented" },
	{ .name = "Eager symbol expansion in reference block",
	  .test_func = test_eager_symbol_in_reference,
	  .todo_msg = "eager [[symbol]] expansion in reference position not implemented" },
	{},
};

VUNIT_TEST_SUITE(tests)
