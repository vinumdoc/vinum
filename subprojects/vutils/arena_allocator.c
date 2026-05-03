#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "allocator.h"
#include "arena_allocator.h"

#define ALIGN_UP(addr, align) ((((uintptr_t)(addr)) + ((align) - 1)) & ~((uintptr_t)((align) - 1)))

struct vut_arena vut_arena_new(struct vut_allocator *base_allocator, size_t arena_size) {
	struct vut_arena new_arena = {};
	new_arena.base_allocator = base_allocator;
	new_arena.buffer_size = arena_size;
	new_arena.buffer = vut_allocator_malloc(base_allocator, sizeof(uint8_t), arena_size);
	new_arena.free_ptr = new_arena.buffer;
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

void *vut_arena_malloc(struct vut_arena *arena, size_t bytes, size_t times) {
	if (bytes * times == 0)
		return NULL;

	assert(arena->buffer - arena->free_ptr <= (long)arena->buffer_size);

	uint8_t *aligned_ptr = (uint8_t *)ALIGN_UP(arena->free_ptr, _Alignof(max_align_t));
	uint8_t *end_ptr = aligned_ptr + bytes * times;

	arena->free_ptr = end_ptr;

	return aligned_ptr;
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
	if (old_ptr != NULL)
		memcpy(ptr, old_ptr, bytes * times);

	return ptr;
}

void vut_arena_free_all(struct vut_arena *arena) {
	vut_allocator_free(arena->base_allocator, arena->buffer);
}
