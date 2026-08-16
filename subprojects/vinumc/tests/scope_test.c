#include <vunit/vunit.h>

#include "test_helpers.h"

void test_local_definition_in_section(struct vunit_test_ctx *ctx) {
	char *out =
		compile_program(ctx, "[section\n"
				     "[title : Spanish varieties]\n"
				     "[winegrape: Vitis vinifera]\n"
				     "Tempranillo is a variety of [winegrape] typical of Spain.\n"
				     "]\n");

	VUNIT_ASSERT_STREQ(ctx, out,
			   "[section [title : Spanish varieties] "
			   "Tempranillo is a variety of Vitis vinifera typical of Spain. ]");
}

void test_sibling_section_does_not_inherit(struct vunit_test_ctx *ctx) {
	char *out =
		compile_program(ctx, "[section\n"
				     "[title : Spanish varieties]\n"
				     "[winegrape: Vitis vinifera]\n"
				     "Tempranillo is a variety of [winegrape] typical of Spain.\n"
				     "]\n"
				     "[section\n"
				     "[title : French varieties]\n"
				     "Pinot Noir is a variety of [winegrape] typical of France.\n"
				     "]\n");

	VUNIT_ASSERT_STREQ(ctx, out,
			   "[section [title : Spanish varieties] "
			   "Tempranillo is a variety of Vitis vinifera typical of Spain. ] "
			   "[section [title : French varieties] "
			   "Pinot Noir is a variety of typical of France. ]");
}

void test_parent_scope_inheritance(struct vunit_test_ctx *ctx) {
	char *out =
		compile_program(ctx, "[chapter\n"
				     "[title : Grape varieties]\n"
				     "[winegrape: Vitis vinifera]\n"
				     "[section\n"
				     "[title : Spanish varieties]\n"
				     "Tempranillo is a variety of [winegrape] typical of Spain.\n"
				     "]\n"
				     "[section\n"
				     "[title : French varieties]\n"
				     "Pinot Noir is a variety of [winegrape] typical of France.\n"
				     "]\n"
				     "]\n");

	VUNIT_ASSERT_STREQ(ctx, out,
			   "[chapter [title : Grape varieties] "
			   "[section [title : Spanish varieties] "
			   "Tempranillo is a variety of Vitis vinifera typical of Spain. ] "
			   "[section [title : French varieties] "
			   "Pinot Noir is a variety of Vitis vinifera typical of France. ] ]");
}

void test_local_function_outside_local(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[wine : Merlot]\n"
					 "[section\n"
					 "[par We like [local wine]]\n"
					 "]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "[section [par We like ] ]");
}

void test_at_shot_for_local(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[author : Outer]\n"
					 "[thought: [argument] ([@ author])]\n"
					 "[thought Nunc est bibendum [author: Horace] ]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Nunc est bibendum (Horace)");
}

void test_at_shot_no_blank(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[author : Outer]\n"
					 "[thought: [argument] ([@author])]\n"
					 "[thought Nunc est bibendum]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Nunc est bibendum ()");
}

void test_global_declaration(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[section\n"
					 "[global wine]\n"
					 "[wine : Merlot]\n"
					 "]\n"
					 "[section [wine]]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "[section ] [section Merlot]");
}

void test_undefine_function(struct vunit_test_ctx *ctx) {
	char *out = compile_program(ctx, "[wine : Merlot]\n"
					 "[undefine wine]\n"
					 "[wine]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "[wine]");
}

struct vunit_test tests[] = {
	{ .name = "Local definition referenced in same section",
	  .test_func = test_local_definition_in_section,
	  .todo = true,
	  .todo_msg = "lexical scoping inside [section] not implemented" },
	{ .name = "Sibling section does not inherit local variable",
	  .test_func = test_sibling_section_does_not_inherit,
	  .todo = true,
	  .todo_msg = "lexical scoping isolation between sections not implemented" },
	{ .name = "Children sections inherit from parent chapter",
	  .test_func = test_parent_scope_inheritance,
	  .todo = true,
	  .todo_msg = "scope inheritance from parent block not implemented" },
	{ .name = "local function returns empty outside local definition",
	  .test_func = test_local_function_outside_local,
	  .todo = true,
	  .todo_msg = "[local ...] VSL function not implemented" },
	{ .name = "@ shot reads only local binding",
	  .test_func = test_at_shot_for_local,
	  .todo = true,
	  .todo_msg = "[@ ...] local-only shot not implemented" },
	{ .name = "@author without blank reads only local binding",
	  .test_func = test_at_shot_no_blank,
	  .todo = true,
	  .todo_msg = "[@symbol] local-only shot without blank not implemented" },
	{ .name = "global declares a symbol at document scope",
	  .test_func = test_global_declaration,
	  .todo = true,
	  .todo_msg = "[global ...] declaration not implemented" },
	{ .name = "undefine removes a symbol",
	  .test_func = test_undefine_function,
	  .todo = true,
	  .todo_msg = "[undefine ...] not implemented" },
	{},
};

VUNIT_TEST_SUITE(tests)
