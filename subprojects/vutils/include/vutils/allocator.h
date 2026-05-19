#ifndef __VUT_ALLOCATOR_H__
#define __VUT_ALLOCATOR_H__

#include "allocator.h"
#include <stddef.h>

struct vut_allocator_funcs {
	void *(*malloc)(void *allocator, size_t bytes, size_t times);
	void *(*calloc)(void *allocator, size_t bytes, size_t times);
	void *(*realloc)(void *allocator, void *old_ptr, size_t bytes, size_t times);
	void (*free)(void *allocator, void *ptr);
};

struct vut_allocator {
	void *base_allocator;
	const struct vut_allocator_funcs *funcs;
};

void *vut_allocator_malloc(struct vut_allocator allocator, size_t bytes, size_t times);
void *vut_allocator_calloc(struct vut_allocator allocator, size_t bytes, size_t times);
void *vut_allocator_realloc(struct vut_allocator allocator, void *old_ptr, size_t bytes,
			    size_t times);
void vut_allocator_free(struct vut_allocator allocator, void *ptr);

#endif // __VUT_ALLOCATOR_H__
