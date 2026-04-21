#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "allocator.h"
#include "arena_allocator.h"

struct vut_arena vut_arena_new(struct vut_allocator *base_allocator,
			       size_t common_max_allocation_size) {
	struct vut_arena new_arena = {
		.allocations = VUT_VEC_INIT(struct vut_arena_allocations, base_allocator),
		.base_allocator = base_allocator,
		.common_max_allocation_size = common_max_allocation_size,
	};
	return new_arena;
}

static void *stub_arena_alloc_malloc(void *arena, size_t bytes, size_t times) {
	return vut_arena_malloc(arena, bytes, times);
}

static void *stub_arena_alloc_calloc(void *arena, size_t bytes, size_t times) {
	return vut_arena_calloc(arena, bytes, times);
}

static void *stub_arena_alloc_realloc(void *arena, void *ptr, size_t bytes, size_t times) {
	return vut_arena_realloc(arena, ptr, bytes, times);
}

static void stub_arena_alloc_free(void *arena, void *ptr) {
	(void)arena;
	(void)ptr;
}

static struct vut_allocator_funcs arena_allocator_funcs = {
	.malloc = stub_arena_alloc_malloc,
	.calloc = stub_arena_alloc_calloc,
	.realloc = stub_arena_alloc_realloc,
	.free = stub_arena_alloc_free,
};

struct vut_allocator vut_arena_to_vut_allocator(struct vut_arena *arena) {
	struct vut_allocator new_allocator = {
		.base_allocator = arena,
		.funcs = arena_allocator_funcs,
	};

	return new_allocator;
}

static void append_new_buffer(struct vut_arena *arena, size_t buffer_size) {
	struct arena_buffer new = { 0 };
	new.buffer = vut_allocator_malloc(arena->base_allocator, buffer_size, 1);
	VUT_VEC_PUT(&arena->allocations, new);
}

static struct arena_buffer get_last_buffer(struct vut_arena *arena) {
	return VUT_VEC_AT(&arena->allocations, arena->allocations.len - 1);
}

void *vut_arena_malloc(struct vut_arena *arena, size_t bytes, size_t times) {
	if (bytes * times == 0)
		return NULL;
	size_t allocation_size = bytes * times;

	if (arena->allocations.len == 0)
		append_new_buffer(arena, arena->common_max_allocation_size);

	struct arena_buffer last_buffer = get_last_buffer(arena);

	if (allocation_size > arena->common_max_allocation_size) {
		append_new_buffer(arena, allocation_size);
		void *ptr = get_last_buffer(arena).buffer;
		append_new_buffer(arena, arena->common_max_allocation_size);
		return ptr;
	} else if (arena->common_max_allocation_size - last_buffer.used < allocation_size) {
		append_new_buffer(arena, arena->common_max_allocation_size);
		last_buffer = get_last_buffer(arena);
	}

	void *ptr = &last_buffer.buffer[last_buffer.used];
	last_buffer.used += allocation_size;

	return ptr;
}

void *vut_arena_calloc(struct vut_arena *arena, size_t bytes, size_t times) {
	if (bytes * times == 0)
		return NULL;

	void *ptr = vut_arena_malloc(arena, bytes, times);
	memset(ptr, 0, bytes * times);
	return ptr;
}

void *vut_arena_realloc(struct vut_arena *arena, void *old_ptr, size_t bytes, size_t times) {
	if (bytes * times == 0)
		return NULL;

	void *ptr = vut_arena_malloc(arena, bytes, times);
	memcpy(ptr, old_ptr, bytes * times);

	return ptr;
}

void vut_arena_free_all(struct vut_arena *arena) {
	VUT_VEC_FOREACH(&arena->allocations, _, struct arena_buffer * buffer) {
		vut_allocator_free(arena->base_allocator, buffer->buffer);
	}
	VUT_VEC_FREE(&arena->allocations);
}
