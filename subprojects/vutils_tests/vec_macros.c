#include <vunit/vunit.h>

#include <vutils/system_allocator.h>
#include <vutils/vec.h>

struct int_vec_t VUT_VEC_DEF(int);

void test_vec_reserve_exact(struct vunit_test_ctx *ctx) {
	struct int_vec_t vec = VUT_VEC_INIT(struct int_vec_t, vut_get_system_allocator());

	VUT_VEC_RESERVE_EXACT(&vec, 5);
	VUNIT_ASSERT_EQ(ctx, vec.len, 0);
	VUNIT_ASSERT_EQ(ctx, vec.capacity, 5);

	VUT_VEC_PUT(&vec, 1);
	VUT_VEC_PUT(&vec, 2);
	VUNIT_ASSERT_EQ(ctx, vec.len, 2);

	VUT_VEC_RESERVE_EXACT(&vec, 2);
	VUNIT_ASSERT_EQ_MSG(ctx, vec.capacity, 5,
			    "VUT_VEC_RESERVE_EXACT should not change the capacity when enough "
			    "space is available");

	VUT_VEC_FREE(&vec);
}

void test_vec_reserve(struct vunit_test_ctx *ctx) {
	struct int_vec_t vec = VUT_VEC_INIT(struct int_vec_t, vut_get_system_allocator());

	VUT_VEC_RESERVE(&vec, 5);
	VUNIT_ASSERT_EQ_MSG(ctx, vec.len, 0, "VUT_VEC_RESERVE should not change the length");
	VUNIT_ASSERT_EQ_MSG(ctx, vec.capacity, 8, "VUT_VEC_RESERVE should use the next power of 2");

	VUT_VEC_PUT(&vec, 1);
	VUT_VEC_PUT(&vec, 2);
	VUNIT_ASSERT_EQ(ctx, vec.len, 2);

	VUT_VEC_RESERVE(&vec, 2);
	VUNIT_ASSERT_EQ_MSG(
		ctx, vec.capacity, 8,
		"VUT_VEC_RESERVE should not change the capacity when enough space is available");

	VUT_VEC_FREE(&vec);
}

void test_vec_put(struct vunit_test_ctx *ctx) {
	struct int_vec_t vec = VUT_VEC_INIT(struct int_vec_t, vut_get_system_allocator());

	VUT_VEC_RESERVE_EXACT(&vec, 3);
	VUT_VEC_PUT(&vec, 1);
	VUT_VEC_PUT(&vec, 2);
	VUT_VEC_PUT(&vec, 3);

	VUNIT_ASSERT_EQ(ctx, VUT_VEC_AT(&vec, 0), 1);
	VUNIT_ASSERT_EQ(ctx, VUT_VEC_AT(&vec, 1), 2);
	VUNIT_ASSERT_EQ(ctx, VUT_VEC_AT(&vec, 2), 3);
	VUNIT_ASSERT_EQ(ctx, vec.len, 3);
	VUNIT_ASSERT_EQ_MSG(
		ctx, vec.capacity, 3,
		"VUT_VEC_PUT should not change the capacity when enough space is available");

	VUT_VEC_PUT(&vec, 4);
	VUNIT_ASSERT_EQ(ctx, vec.len, 4);
	VUNIT_ASSERT_EQ_MSG(ctx, vec.capacity, 6,
			    "VUT_VEC_PUT should double the capacity when needed");
	VUNIT_ASSERT_EQ(ctx, VUT_VEC_AT(&vec, 3), 4);

	VUT_VEC_FREE(&vec);
}

void test_vec_pop(struct vunit_test_ctx *ctx) {
	struct int_vec_t vec = VUT_VEC_INIT(struct int_vec_t, vut_get_system_allocator());

	VUT_VEC_RESERVE_EXACT(&vec, 3);

	VUT_VEC_PUT(&vec, 0);
	VUT_VEC_POP(&vec);
	VUT_VEC_POP(&vec);

	VUNIT_ASSERT_EQ_MSG(ctx, vec.len, 0,
			    "VUT_VEC_POP should not change the length of an empty vector");

	VUT_VEC_PUT(&vec, 1);
	VUT_VEC_PUT(&vec, 2);
	VUT_VEC_PUT(&vec, 3);

	VUNIT_ASSERT_EQ(ctx, vec.len, 3);
	VUNIT_ASSERT_EQ(ctx, VUT_VEC_AT(&vec, 2), 3);

	VUT_VEC_POP(&vec);
	VUNIT_ASSERT_EQ(ctx, vec.len, 2);
	VUNIT_ASSERT_EQ(ctx, VUT_VEC_AT(&vec, 1), 2);

	VUT_VEC_FREE(&vec);
}

void test_vec_put_many(struct vunit_test_ctx *ctx) {
	int source[] = { 9, 8, 7, 6 };
	struct int_vec_t vec = VUT_VEC_INIT(struct int_vec_t, vut_get_system_allocator());

	VUT_VEC_PUT_MANY(&vec, source, 4);

	VUNIT_ASSERT_EQ(ctx, vec.len, 4);
	VUNIT_ASSERT_EQ(ctx, VUT_VEC_AT(&vec, 0), 9);
	VUNIT_ASSERT_EQ(ctx, VUT_VEC_AT(&vec, 1), 8);
	VUNIT_ASSERT_EQ(ctx, VUT_VEC_AT(&vec, 2), 7);
	VUNIT_ASSERT_EQ(ctx, VUT_VEC_AT(&vec, 3), 6);

	VUNIT_ASSERT_EQ_MSG(ctx, vec.capacity, 8,
			    "VUT_VEC_PUT_MANY should call VUT_VEC_RESERVE for 4 + 1 elements");

	VUT_VEC_FREE(&vec);
}

void test_vec_foreach(struct vunit_test_ctx *ctx) {
	int source[] = { 9, 8, 7, 6 };
	struct int_vec_t vec = VUT_VEC_INIT(struct int_vec_t, vut_get_system_allocator());

	VUT_VEC_PUT_MANY(&vec, source, sizeof(source) / sizeof(source[0]));

	VUT_VEC_FOREACH(&vec, i, int *v) {
		VUNIT_ASSERT_EQ(ctx, *v, source[i]);
	}

	VUT_VEC_FREE(&vec);
}

struct vunit_test tests[] = {
	{ .name = "Test VUT_VEC_RESERVE behavior", .test_func = test_vec_reserve },
	{ .name = "Test VUT_VEC_RESERVE_EXACT behavior", .test_func = test_vec_reserve_exact },
	{ .name = "Test VUT_VEC_PUT behavior", .test_func = test_vec_put },
	{ .name = "Test VUT_VEC_POP behavior", .test_func = test_vec_pop },
	{ .name = "Test VUT_VEC_PUT_MANY behavior", .test_func = test_vec_put_many },
	{ .name = "Test VUT_VEC_FOREACH behavior", .test_func = test_vec_foreach },
	{},
};

VUNIT_TEST_SUITE(tests)
