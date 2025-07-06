#ifndef __VEC_H__
#define __VEC_H__

#include <stdlib.h>

#define VEC_DEF(base_type)                                                                         \
	{                                                                                          \
		base_type *base;                                                                   \
		size_t len;                                                                        \
		size_t capacity;                                                                   \
	}

#define VEC_AT(arr, idx) ((arr)->base[(idx)])

#define VEC_RESERVE_EXACT(arr, size)                                                               \
	do {                                                                                       \
		size_t needed = (arr)->len + (size);                                               \
		if (needed <= (arr)->capacity)                                                     \
			break;                                                                     \
		(arr)->capacity = needed;                                                          \
		(arr)->base = realloc((arr)->base, (arr)->capacity * sizeof(*(arr)->base));        \
	} while (0)

#define VEC_RESERVE(arr, size)                                                                     \
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
		(arr)->base = realloc((arr)->base, (arr)->capacity * sizeof(*(arr)->base));        \
	} while (0)

#define VEC_PUT(arr, e)                                                                            \
	do {                                                                                       \
		if ((arr)->len >= (arr)->capacity) {                                               \
			size_t more = (arr)->capacity == 0 ? 2 : (arr)->capacity;                  \
                                                                                                   \
			VEC_RESERVE_EXACT((arr), more);                                            \
		}                                                                                  \
                                                                                                   \
		(arr)->base[(arr)->len] = (e);                                                     \
		(arr)->len++;                                                                      \
	} while (0)

#define VEC_FREE(vec)                                                                              \
	do {                                                                                       \
		free((vec)->base);                                                                 \
		(vec)->base = NULL;                                                                \
		(vec)->len = 0;                                                                    \
		(vec)->capacity = 0;                                                               \
	} while (0)

#define VEC_POP(arr)                                                                               \
	do {                                                                                       \
		if ((arr)->len > 0) {                                                              \
			(arr)->len--;                                                              \
		}                                                                                  \
	} while (0)

#define VEC_PUT_MANY(to, from, from_size)                                                          \
	do {                                                                                       \
		VEC_RESERVE((to), from_size + 1);                                                  \
		for (size_t _i = 0; _i < from_size; ++_i) {                                        \
			(to)->base[(to)->len + _i] = from[_i];                                     \
		}                                                                                  \
		(to)->len += from_size;                                                            \
	} while (0)

#endif // __VEC_H__
