#ifndef __VUT_VEC_H__
#define __VUT_VEC_H__

#include "allocator.h"

/// Defines the struct body for a vector of a given base type.
#define VUT_VEC_DEF(base_type)                                                                     \
	{                                                                                          \
		base_type *base;                                                                   \
		size_t len;                                                                        \
		size_t capacity;                                                                   \
		struct vut_allocator allocator;                                                    \
	}

#define VUT_VEC_INIT(vec_type, allocator_obj)                                                      \
	(vec_type) {                                                                               \
		.allocator = (allocator_obj)                                                       \
	}

#define VUT_VEC_AT(arr, idx) ((arr)->base[(idx)])

/// Reserves the minimum capacity needed to hold exactly `size` more elements.
///
/// If the vector's current capacity is already enough, it does nothing.
#define VUT_VEC_RESERVE_EXACT(arr, size)                                                           \
	do {                                                                                       \
		size_t needed = (arr)->len + (size);                                               \
		if (needed <= (arr)->capacity)                                                     \
			break;                                                                     \
		(arr)->capacity = needed;                                                          \
		(arr)->base = vut_allocator_realloc((arr)->allocator, (arr)->base,                 \
						    sizeof(*(arr)->base), (arr)->capacity);        \
	} while (0)

/// Ensures that the vector has enough capacity to hold at least `size` more elements, possibly
/// reserving more space to speculatively avoid frequent reallocations.
///
/// If the vector's current capacity is already enough, it does nothing.
#define VUT_VEC_RESERVE(arr, size)                                                                 \
	do {                                                                                       \
		size_t needed = (arr)->len + (size);                                               \
		if (needed <= (arr)->capacity)                                                     \
			break;                                                                     \
		/* rounds needed to the nearest power of 2 >= its current value */                 \
		needed--;                                                                          \
		for (size_t i = 1; i < sizeof(size_t) * 8; i <<= 1)                                \
			needed |= needed >> i;                                                     \
		needed++;                                                                          \
                                                                                                   \
		(arr)->capacity = needed;                                                          \
		(arr)->base = vut_allocator_realloc((arr)->allocator, (arr)->base,                 \
						    sizeof(*(arr)->base), (arr)->capacity);        \
	} while (0)

/// Adds an element to the end of the vector, resizing it if necessary.
#define VUT_VEC_PUT(arr, e)                                                                        \
	do {                                                                                       \
		if ((arr)->len >= (arr)->capacity) {                                               \
			size_t more = (arr)->capacity == 0 ? 2 : (arr)->capacity;                  \
                                                                                                   \
			VUT_VEC_RESERVE_EXACT((arr), more);                                        \
		}                                                                                  \
                                                                                                   \
		(arr)->base[(arr)->len] = (e);                                                     \
		(arr)->len++;                                                                      \
                                                                                                   \
	} while (0)

/// Dealocates the vector. Resets `base` to `NULL` and `len` and `capacity` to 0.
#define VUT_VEC_FREE(vec)                                                                          \
	do {                                                                                       \
		vut_allocator_free((vec)->allocator, (vec)->base);                                 \
		(vec)->base = NULL;                                                                \
		(vec)->len = 0;                                                                    \
		(vec)->capacity = 0;                                                               \
	} while (0)

/// Decrements the length of the vector by one.
#define VUT_VEC_POP(arr)                                                                           \
	do {                                                                                       \
		if ((arr)->len > 0) {                                                              \
			(arr)->len--;                                                              \
		}                                                                                  \
	} while (0)

/// Will append `from_size` elements from `from` to `to`.
/// It will reserve enough space for `from_size + 1` elements in `to`.
/// This ensures that there is space for a possible null terminator.
#define VUT_VEC_PUT_MANY(to, from, from_size)                                                      \
	do {                                                                                       \
		VUT_VEC_RESERVE((to), from_size + 1);                                              \
		for (size_t _i = 0; _i < from_size; ++_i) {                                        \
			(to)->base[(to)->len + _i] = from[_i];                                     \
		}                                                                                  \
		(to)->len += from_size;                                                            \
	} while (0)

#define VUT_VEC_FOREACH(arr, idx, var)                                                             \
	for (size_t(idx) = 0, _capture_var = 1; (idx) < (arr)->len; (idx)++, _capture_var = 1)     \
		for (var = &(arr)->base[(idx)]; _capture_var; _capture_var = 0)

#endif // __VUT_VEC_H__
