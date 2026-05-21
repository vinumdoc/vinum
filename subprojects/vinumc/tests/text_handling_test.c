#include "test_helpers.h"

void test_blank_reduction(struct vunit_test_ctx *ctx) {
	const char *out = compile_program(ctx, "[a:    Content   with spaces!    ]\n"
					       "[a]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Content with spaces!");
}

void test_character_escaping(struct vunit_test_ctx *ctx) {
	const char *out = compile_program(ctx, "[a: \\[ hello \\] \\{# world #}!]\n"
					       "[a]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "[ hello ] {# world #}!");
}

void test_literal_delimiter(struct vunit_test_ctx *ctx) {
	const char *out = compile_program(ctx, "[a: {# [hello] {# world #}#}]\n"
					       "[a]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "[hello] {# world #}");
}

struct vunit_test tests[] = {
	{ .name = "Test blank reduction", .test_func = test_blank_reduction },
	{ .name = "Test character escaping", .test_func = test_character_escaping },
	{ .name = "Test literal delimiter", .test_func = test_literal_delimiter },
	{},
};

VUNIT_TEST_SUITE(tests)
