#include <vunit/vunit.h>

#include "../vec.h"

struct int_vec_t VEC_DEF(int);

void test_vec_reserve_exact(struct vunit_test_ctx *ctx) {
	struct int_vec_t vec = {};

	VEC_RESERVE_EXACT(&vec, 5);
	VUNIT_ASSERT_EQ(ctx, vec.len, 0);
	VUNIT_ASSERT_EQ(ctx, vec.capacity, 5);

	VEC_PUT(&vec, 1);
	VEC_PUT(&vec, 2);
	VUNIT_ASSERT_EQ(ctx, vec.len, 2);

	VEC_RESERVE_EXACT(&vec, 2);
	VUNIT_ASSERT_EQ_MSG(
		ctx, vec.capacity, 5,
		"VEC_RESERVE_EXACT should not change the capacity when enough space is available");

	VEC_FREE(&vec);
}

void test_vec_reserve(struct vunit_test_ctx *ctx) {
	struct int_vec_t vec = {};

	VEC_RESERVE(&vec, 5);
	VUNIT_ASSERT_EQ_MSG(ctx, vec.len, 0, "VEC_RESERVE should not change the length");
	VUNIT_ASSERT_EQ_MSG(ctx, vec.capacity, 8, "VEC_RESERVE should use the next power of 2");

	VEC_PUT(&vec, 1);
	VEC_PUT(&vec, 2);
	VUNIT_ASSERT_EQ(ctx, vec.len, 2);

	VEC_RESERVE(&vec, 2);
	VUNIT_ASSERT_EQ_MSG(
		ctx, vec.capacity, 8,
		"VEC_RESERVE should not change the capacity when enough space is available");

	VEC_FREE(&vec);
}

void test_vec_put(struct vunit_test_ctx *ctx) {
	struct int_vec_t vec = {};

	VEC_RESERVE_EXACT(&vec, 3);
	VEC_PUT(&vec, 1);
	VEC_PUT(&vec, 2);
	VEC_PUT(&vec, 3);

	VUNIT_ASSERT_EQ(ctx, VEC_AT(&vec, 0), 1);
	VUNIT_ASSERT_EQ(ctx, VEC_AT(&vec, 1), 2);
	VUNIT_ASSERT_EQ(ctx, VEC_AT(&vec, 2), 3);
	VUNIT_ASSERT_EQ(ctx, vec.len, 3);
	VUNIT_ASSERT_EQ_MSG(
		ctx, vec.capacity, 3,
		"VEC_PUT should not change the capacity when enough space is available");

	VEC_PUT(&vec, 4);
	VUNIT_ASSERT_EQ(ctx, vec.len, 4);
	VUNIT_ASSERT_EQ_MSG(ctx, vec.capacity, 6, "VEC_PUT should double the capacity when needed");
	VUNIT_ASSERT_EQ(ctx, VEC_AT(&vec, 3), 4);

	VEC_FREE(&vec);
}

void test_vec_pop(struct vunit_test_ctx *ctx) {
	struct int_vec_t vec = {};

	VEC_RESERVE_EXACT(&vec, 3);

	VEC_PUT(&vec, 0);
	VEC_POP(&vec);
	VEC_POP(&vec);

	VUNIT_ASSERT_EQ_MSG(ctx, vec.len, 0,
			    "VEC_POP should not change the length of an empty vector");

	VEC_PUT(&vec, 1);
	VEC_PUT(&vec, 2);
	VEC_PUT(&vec, 3);

	VUNIT_ASSERT_EQ(ctx, vec.len, 3);
	VUNIT_ASSERT_EQ(ctx, VEC_AT(&vec, 2), 3);

	VEC_POP(&vec);
	VUNIT_ASSERT_EQ(ctx, vec.len, 2);
	VUNIT_ASSERT_EQ(ctx, VEC_AT(&vec, 1), 2);

	VEC_FREE(&vec);
}

void test_vec_put_many(struct vunit_test_ctx *ctx) {
	int source[] = { 9, 8, 7, 6 };
	struct int_vec_t vec = {};

	VEC_PUT_MANY(&vec, source, 4);

	VUNIT_ASSERT_EQ(ctx, vec.len, 4);
	VUNIT_ASSERT_EQ(ctx, VEC_AT(&vec, 0), 9);
	VUNIT_ASSERT_EQ(ctx, VEC_AT(&vec, 1), 8);
	VUNIT_ASSERT_EQ(ctx, VEC_AT(&vec, 2), 7);
	VUNIT_ASSERT_EQ(ctx, VEC_AT(&vec, 3), 6);

	VUNIT_ASSERT_EQ_MSG(ctx, vec.capacity, 8,
			    "VEC_PUT_MANY should call VEC_RESERVE for 4 + 1 elements");

	VEC_FREE(&vec);
}

struct vunit_test tests[] = {
	{ .name = "Test VEC_RESERVE behavior", .test_func = test_vec_reserve },
	{ .name = "Test VEC_RESERVE_EXACT behavior", .test_func = test_vec_reserve_exact },
	{ .name = "Test VEC_PUT behavior", .test_func = test_vec_put },
	{ .name = "Test VEC_POP behavior", .test_func = test_vec_pop },
	{ .name = "Test VEC_PUT_MANY behavior", .test_func = test_vec_put_many },
	{},
};

VUNIT_TEST_SUITE(tests)
