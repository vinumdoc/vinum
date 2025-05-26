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

#define VEC_PUT(arr, e)                                                                            \
	do {                                                                                       \
		if ((arr)->len >= (arr)->capacity) {                                               \
			if ((arr)->capacity == 0)                                                  \
				(arr)->capacity = 1;                                               \
                                                                                                   \
			(arr)->capacity *= 2;                                                      \
			(arr)->base =                                                              \
				realloc((arr)->base, (arr)->capacity * sizeof(*(arr)->base));      \
		}                                                                                  \
                                                                                                   \
		(arr)->base[(arr)->len] = (e);                                                     \
		(arr)->len++;                                                                      \
	} while (0)

#define VEC_RESERVE(arr, size)                                                                     \
	do {                                                                                       \
		(arr)->capacity = (size);                                                          \
		(arr)->base = realloc((arr)->base, (arr)->capacity * sizeof(*(arr)->base));        \
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
		(arr)->len--;                                                                      \
	} while (0)

#define VEC_PUT_MANY(to, from, from_size)                                                          \
	do {                                                                                       \
		if ((to)->len + from_size > (to)->capacity) {                                      \
			VEC_RESERVE((to), (to)->len + from_size + 1);                              \
		}                                                                                  \
		for (size_t _i = 0; _i < from_size; ++_i) {                                        \
			(to)->base[(to)->len + _i] = from[_i];                                     \
		}                                                                                  \
		(to)->len += from_size;                                                            \
	} while (0)

#endif // __VEC_H__
