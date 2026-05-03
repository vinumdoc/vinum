#include <string.h>

#include "str.h"
#include "sv.h"

struct vut_sv vut_sv_from_str(const char *str, size_t len) {
	struct vut_sv ret = {
		.base = str,
		.len = len,
	};
	return ret;
}

struct vut_sv vut_sv_from_cstr(const char *cstr) {
	return vut_sv_from_str(cstr, strlen(cstr));
}

struct vut_sv vut_sv_from_vut_str(const struct vut_str *str) {
	return vut_sv_from_str(str->base, str->len);
}

bool vut_sv_eq(struct vut_sv a, struct vut_sv b) {
	if (a.len != b.len)
		return false;

	for (size_t i = 0; i < a.len; i++) {
		if (a.base[i] != b.base[i])
			return false;
	}

	return true;
}
