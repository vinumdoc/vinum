#include <stddef.h>
#include <vunit/vunit.h>

#include <vutils/allocator.h>
#include <vutils/arena_allocator.h>
#include <vutils/system_allocator.h>

#define ARRRAY_LEN(arr) (sizeof((arr))) / (sizeof((arr)[0]))
#define DEFAULT_ALLOC_SIZE 1024

struct allocator_case;

typedef struct vut_allocator (*gen_allocator_func_t)(void);
typedef void (*destroy_allocator_func_t)(struct vut_allocator allocator);

struct allocator_case {
	gen_allocator_func_t gen_allocator;
	destroy_allocator_func_t destroy_allocator;
};

static struct vut_allocator gen_sys_allocator() {
	return vut_get_system_allocator();
}

static struct vut_allocator gen_arena_allocator() {
	struct vut_arena *arena =
		vut_allocator_malloc(vut_get_system_allocator(), sizeof(*arena), 1);
	*arena = vut_arena_new(vut_get_system_allocator(), DEFAULT_ALLOC_SIZE * DEFAULT_ALLOC_SIZE);
	return vut_arena_to_vut_allocator(arena);
}

static void destroy_arena_allocator(struct vut_allocator allocator) {
	struct vut_arena *arena = allocator.base_allocator;
	vut_arena_free_all(arena);
	vut_allocator_free(vut_get_system_allocator(), arena);
}

static struct allocator_case allocator_cases[] = {
	{
		.gen_allocator = gen_sys_allocator,
	},
	{
		.gen_allocator = gen_arena_allocator,
		.destroy_allocator = destroy_arena_allocator,
	},
};

static void test_malloc_simple(struct vunit_test_ctx *ctx) {
	for (size_t i = 0; i < ARRRAY_LEN(allocator_cases); i++) {
		struct allocator_case *c = &allocator_cases[i];
		struct vut_allocator alloc = c->gen_allocator();

		size_t *arr = vut_allocator_malloc(alloc, sizeof(*arr), DEFAULT_ALLOC_SIZE);
		for (size_t j = 0; j < DEFAULT_ALLOC_SIZE; j++) {
			arr[j] = j;
			VUNIT_ASSERT_EQ(ctx, arr[j], j);
		}

		vut_allocator_free(alloc, arr);

		if (c->destroy_allocator != NULL)
			c->destroy_allocator(alloc);
	}
}

static void test_malloc_zero_size_allocation(struct vunit_test_ctx *ctx) {
	for (size_t i = 0; i < ARRRAY_LEN(allocator_cases); i++) {
		struct allocator_case *c = &allocator_cases[i];
		struct vut_allocator alloc = c->gen_allocator();

		size_t *arr = vut_allocator_malloc(alloc, sizeof(*arr), 0);
		VUNIT_ASSERT_EQ(ctx, arr, NULL);

		vut_allocator_free(alloc, arr);

		if (c->destroy_allocator != NULL)
			c->destroy_allocator(alloc);
	}
}

static void test_calloc_simple(struct vunit_test_ctx *ctx) {
	for (size_t i = 0; i < ARRRAY_LEN(allocator_cases); i++) {
		struct allocator_case *c = &allocator_cases[i];
		struct vut_allocator alloc = c->gen_allocator();

		size_t *arr = vut_allocator_calloc(alloc, sizeof(*arr), DEFAULT_ALLOC_SIZE);
		for (size_t j = 0; j < DEFAULT_ALLOC_SIZE; j++) {
			VUNIT_ASSERT_EQ(ctx, arr[j], 0);
			arr[j] = j;
			VUNIT_ASSERT_EQ(ctx, arr[j], j);
		}

		vut_allocator_free(alloc, arr);

		if (c->destroy_allocator != NULL)
			c->destroy_allocator(alloc);
	}
}

static void test_calloc_zero_size_allocation(struct vunit_test_ctx *ctx) {
	for (size_t i = 0; i < ARRRAY_LEN(allocator_cases); i++) {
		struct allocator_case *c = &allocator_cases[i];
		struct vut_allocator alloc = c->gen_allocator();

		size_t *arr = vut_allocator_calloc(alloc, sizeof(*arr), 0);
		VUNIT_ASSERT_EQ(ctx, arr, NULL);

		if (c->destroy_allocator != NULL)
			c->destroy_allocator(alloc);
	}
}

static void test_realloc_simple(struct vunit_test_ctx *ctx) {
	for (size_t i = 0; i < ARRRAY_LEN(allocator_cases); i++) {
		struct allocator_case *c = &allocator_cases[i];
		struct vut_allocator alloc = c->gen_allocator();

		size_t *arr = vut_allocator_malloc(alloc, sizeof(*arr), DEFAULT_ALLOC_SIZE);
		for (size_t j = 0; j < DEFAULT_ALLOC_SIZE; j++) {
			arr[j] = j;
		}
		arr = vut_allocator_realloc(alloc, arr, sizeof(*arr), DEFAULT_ALLOC_SIZE * 2);
		for (size_t j = 0; j < DEFAULT_ALLOC_SIZE; j++) {
			VUNIT_ASSERT_EQ(ctx, arr[j], j);
		}
		for (size_t j = DEFAULT_ALLOC_SIZE; j < DEFAULT_ALLOC_SIZE * 2; j++) {
			arr[j] = j;
			VUNIT_ASSERT_EQ(ctx, arr[j], j);
		}

		vut_allocator_free(alloc, arr);

		if (c->destroy_allocator != NULL)
			c->destroy_allocator(alloc);
	}
}

static void test_realloc_zero_size(struct vunit_test_ctx *ctx) {
	for (size_t i = 0; i < ARRRAY_LEN(allocator_cases); i++) {
		struct allocator_case *c = &allocator_cases[i];
		struct vut_allocator alloc = c->gen_allocator();

		size_t *arr = vut_allocator_malloc(alloc, sizeof(*arr), DEFAULT_ALLOC_SIZE);
		for (size_t j = 0; j < DEFAULT_ALLOC_SIZE; j++) {
			arr[j] = j;
		}
		arr = vut_allocator_realloc(alloc, arr, sizeof(*arr), 0);
		VUNIT_ASSERT_EQ(ctx, arr, NULL);

		if (c->destroy_allocator != NULL)
			c->destroy_allocator(alloc);
	}
}

static void test_free_null(struct vunit_test_ctx *ctx) {
	(void)ctx;
	for (size_t i = 0; i < ARRRAY_LEN(allocator_cases); i++) {
		struct allocator_case *c = &allocator_cases[i];
		struct vut_allocator alloc = c->gen_allocator();

		vut_allocator_free(alloc, NULL);

		if (c->destroy_allocator != NULL)
			c->destroy_allocator(alloc);
	}
}

struct vunit_test tests[] = {
	{ .name = "Simple malloc alocation", .test_func = test_malloc_simple },
	{ .name = "Zero size malloc alocation should return NULL",
	  .test_func = test_malloc_zero_size_allocation },
	{ .name = "Simple calloc alocation", .test_func = test_calloc_simple },
	{ .name = "Zero size calloc alocation should return NULL",
	  .test_func = test_calloc_zero_size_allocation },
	{ .name = "Simple realloc alocation", .test_func = test_realloc_simple },
	{ .name = "Zero size realloc alocation should return NULL",
	  .test_func = test_realloc_zero_size },
	{ .name = "Free a NULL ptr should not error", .test_func = test_free_null },
	{},
};

VUNIT_TEST_SUITE(tests)
