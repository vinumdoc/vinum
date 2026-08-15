#include <vunit/vunit.h>

#include "test_helpers.h"

void test_flat_compound_projection(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx,
				    "[author:\n"
				    "[name: Wine Maker]\n"
				    "[email: vinum@example.com]\n"
				    "]\n"
				    "Our friend [author_name] can be reached at [author_email].\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Our friend Wine Maker can be reached at vinum@example.com.");
}

void test_nested_compound_projection(struct vunit_test_ctx *ctx) {
	char *out = compile_program(
		ctx, "[author:\n"
		     "[name:Wine Maker]\n"
		     "[email: vinum@example.com]\n"
		     "[company :\n"
		     "[name : The Amazing Vineyards]\n"
		     "[site : www.example.com]\n"
		     "]\n"
		     "]\n"
		     "Contact [author_name] through [author_email], or make an appointment at "
		     "[author_company_name] by visiting [author_company_site].\n");

	VUNIT_ASSERT_STREQ(ctx, out,
			   "Contact Wine Maker through vinum@example.com, or make an "
			   "appointment at The Amazing Vineyards by visiting www.example.com.");
}

void test_qualified_names_equivalence(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[author_name : Wine Maker]\n"
					 "[author_email : vinum@example.com]\n"
					 "[author_company_name : The Amazing Vineyards]\n"
					 "[author_company_site : www.example.com]\n"
					 "[author_name] at [author_company_name]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Wine Maker at The Amazing Vineyards");
}

void test_extend_compound_later(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[wine_body   : full]\n"
					 "[wine_finish : long]\n"
					 "[wine_ballance : harmonious]\n"
					 "[wine_body] [wine_finish] [wine_ballance]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "full long harmonious");
}

void test_unnamed_fields_auto_index(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[grape:\n"
					 "[: Cabernet Franc]\n"
					 "[: Pinot Noir]\n"
					 "[: Sauvignon Blanc]\n"
					 "]\n"
					 "[grape_1] / [grape_2] / [grape_3]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Cabernet Franc / Pinot Noir / Sauvignon Blanc");
}

void test_compound_projects_outer_text(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[wine: cabernet\n"
					 "[body : full]\n"
					 "sauvignon\n"
					 "[finish : long]\n"
					 "]\n"
					 "[wine] // [wine_body] // [wine_finish]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "cabernet sauvignon // full // long");
}

struct vunit_test tests[] = {
	{ .name = "Flat compound projects fields",
	  .test_func = test_flat_compound_projection,
	  .todo = true,
	  .todo_msg = "compound projection (foo_bar) not implemented" },
	{ .name = "Nested compound projects with underscore path",
	  .test_func = test_nested_compound_projection,
	  .todo = true,
	  .todo_msg = "nested compound projection not implemented" },
	{ .name = "Qualified-name form is equivalent to compound form",
	  .test_func = test_qualified_names_equivalence,
	  .todo_msg = "qualified-name form not implemented" },
	{ .name = "Extending a compound via qualified name",
	  .test_func = test_extend_compound_later,
	  .todo = true,
	  .todo_msg = "compound extension via qualified name not implemented" },
	{ .name = "Unnamed inner fields get sequential integer keys",
	  .test_func = test_unnamed_fields_auto_index,
	  .todo = true,
	  .todo_msg = "auto-indexed unnamed compound fields not implemented" },
	{ .name = "Outer compound symbol projects non-block content",
	  .test_func = test_compound_projects_outer_text,
	  .todo = true,
	  .todo_msg = "outer compound projection of free text not implemented" },
	{},
};

VUNIT_TEST_SUITE(tests)
