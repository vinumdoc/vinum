#include <vunit/vunit.h>

#include "libvinumc.h"

static char *compile_program(const char *prg_cstr, struct vut_allocator alloc) {
	struct compiler_ctx comp = compiler_ctx_init(alloc);

	struct vut_str prg = vut_str_init(alloc);
	vut_str_put_cstr(&prg, prg_cstr);

	struct vut_str cocktail = vut_str_init(alloc);
	struct vut_str out = compiler_compile(&comp, &prg, &cocktail);

	return vut_str_move_to_cstr(&out);
}

void test_call_name_returned_by_another_call(struct vunit_test_ctx *ctx) {
	const char *out = compile_program("[foo: bar]\n"
					  "[bar: Hello World!]\n"
					  "[[foo]]\n",
					  ctx->allocator);

	VUNIT_ASSERT_STREQ(ctx, out, "Hello World!");
}

struct vunit_test tests[] = {
	{ .name = "Cal symbol whose name is resolved by another call",
	  .test_func = test_call_name_returned_by_another_call },
	{},
};

VUNIT_TEST_SUITE(tests)
