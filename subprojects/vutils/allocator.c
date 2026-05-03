#include "allocator.h"

void *vut_allocator_malloc(struct vut_allocator allocator, size_t bytes, size_t times) {
	return allocator.funcs->malloc(allocator.base_allocator, bytes, times);
}

void *vut_allocator_calloc(struct vut_allocator allocator, size_t bytes, size_t times) {
	return allocator.funcs->calloc(allocator.base_allocator, bytes, times);
}

void *vut_allocator_realloc(struct vut_allocator allocator, void *old_ptr, size_t bytes,
			    size_t times) {
	return allocator.funcs->realloc(allocator.base_allocator, old_ptr, bytes, times);
}

void vut_allocator_free(struct vut_allocator allocator, void *ptr) {
	allocator.funcs->free(allocator.base_allocator, ptr);
}
