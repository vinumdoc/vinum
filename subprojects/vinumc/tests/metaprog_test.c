#include <vunit/vunit.h>

#include "test_helpers.h"

void test_call_name_returned_by_another_call(struct vunit_test_ctx *ctx) {
	const char *out = compile_program(ctx, "[foo: bar]\n"
					       "[bar: Hello World!]\n"
					       "[[foo]]\n");

	VUNIT_ASSERT_STREQ(ctx, out, "Hello World!");
}

struct vunit_test tests[] = {
	{ .name = "Cal symbol whose name is resolved by another call",
	  .test_func = test_call_name_returned_by_another_call },
	{},
};

VUNIT_TEST_SUITE(tests)
