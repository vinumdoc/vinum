#ifndef __VUT_ARENA_ALLOCATOR_H__
#define __VUT_ARENA_ALLOCATOR_H__

#include <stddef.h>
#include <stdint.h>

#include "allocator.h"
#include "vec.h"

struct vut_arena {
	struct vut_allocator *base_allocator;
	uint8_t *buffer;
	size_t buffer_size;
	uint8_t *free_ptr;
};

struct vut_arena vut_arena_new(struct vut_allocator *base_allocator,
			       size_t common_max_allocation_size);
struct vut_allocator vut_arena_to_vut_allocator(struct vut_arena *arena);

void *vut_arena_malloc(struct vut_arena *arena, size_t bytes, size_t times);
void *vut_arena_calloc(struct vut_arena *arena, size_t bytes, size_t times);
void *vut_arena_realloc(struct vut_arena *arena, void *old_ptr, size_t bytes, size_t times);
void vut_arena_free_all(struct vut_arena *arena);

#endif // __VUT_ARENA_ALLOCATOR_H__
