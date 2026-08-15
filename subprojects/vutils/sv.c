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

char *vut_sv_to_cstr(const struct vut_sv sv, struct vut_allocator alloc) {
	char *ret = vut_allocator_calloc(alloc, sizeof(*ret), sv.len + 1);
	memcpy(ret, sv.base, sizeof(*ret) * sv.len);
	return ret;
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

bool vut_sv_is_empty(struct vut_sv sv) {
	return sv.len == 0;
}

struct vut_sv vut_sv_splitc(struct vut_sv *sv, char c) {
	for (size_t i = 0; i < sv->len; i++) {
		if (sv->base[i] == c) {
			struct vut_sv ret = vut_sv_from_str(sv->base, i);
			sv->base += i + 1;
			sv->len -= i + 1;
			return ret;
		}
	}

	struct vut_sv ret = *sv;
	sv->base += sv->len;
	sv->len = 0;
	return ret;
}
