#ifndef __VEC_H__
#define __VEC_H__

#include <stdlib.h>

#define VEC_DEF(base_type) { \
	base_type *base; \
	size_t len; \
	size_t capacity; \
}

#define VEC_PUT(arr, e) do { \
	if ((arr)->len >= (arr)->capacity) { \
		if ((arr)->capacity == 0) \
			(arr)->capacity = 1; \
\
		(arr)->capacity *= 2; \
		(arr)->base = realloc((arr)->base, (arr)->capacity * sizeof(*(arr)->base)); \
	} \
\
	(arr)->base[(arr)->len] = (e); \
	(arr)->len++; \
} while(0)

#endif // __VEC_H__
