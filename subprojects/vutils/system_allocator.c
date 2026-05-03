#include "allocator.h"
#include <stdlib.h>

static void *sys_malloc(void *_, size_t bytes, size_t times) {
	(void)_;
	if (bytes * times == 0)
		return NULL;
	return malloc(bytes * times);
}

static void *sys_calloc(void *_, size_t bytes, size_t times) {
	(void)_;
	if (bytes * times == 0)
		return NULL;
	return calloc(times, bytes);
}

static void *sys_realloc(void *_, void *ptr, size_t bytes, size_t times) {
	(void)_;
	return realloc(ptr, bytes * times);
}

static void sys_free(void *_, void *ptr) {
	(void)_;
	free(ptr);
}

static const struct vut_allocator_funcs system_alloc_funcs = {
	.malloc = sys_malloc,
	.calloc = sys_calloc,
	.realloc = sys_realloc,
	.free = sys_free,
};

struct vut_allocator sys_allocator = {
	.base_allocator = NULL,
	.funcs = &system_alloc_funcs,
};

struct vut_allocator vut_get_system_allocator() {
	return sys_allocator;
}
