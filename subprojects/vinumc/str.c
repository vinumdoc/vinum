#include "str.h"
#include <string.h>

void put_str(struct str *to, char *from) {
	if ((to)->len > 0) {
		VEC_POP(to);
	}
	size_t _len = strlen(from);
	VEC_PUT_MANY(to, from, _len);
	VEC_PUT(to, '\0');
}
