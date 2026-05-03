#include <string.h>

#include <str.h>

struct vut_str vut_str_init(struct vut_allocator alloc) {
	return VUT_VEC_INIT(struct vut_str, alloc);
}

struct vut_str vut_str_from_vut_sv(struct vut_sv sv, struct vut_allocator alloc) {
	struct vut_str ret = vut_str_init(alloc);
	vut_str_put_sv(&ret, sv);
	return ret;
}

void vut_str_free(struct vut_str *str) {
	VUT_VEC_FREE(str);
}

static bool is_blank_char(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

void vut_str_put_cstr(struct vut_str *to, const char *from) {
	vut_str_put_sv(to, vut_sv_from_cstr(from));
}

void vut_str_put_sv(struct vut_str *to, const struct vut_sv sv) {
	VUT_VEC_PUT_MANY(to, sv.base, sv.len);
}

// Puts the contents of `from` into `to`, performing blank reduction,
// that is, collapsing consecutive blank characters into a single space.
//
// If `trim_left` is true, leading blanks are removed.
// If `trim_right` is true, trailing blanks are removed.
void vut_str_put_blank_reduced_cstr(struct vut_str *to, char *from, bool trim_left,
				    bool trim_right) {
	vut_str_put_blank_reduced_sv(to, vut_sv_from_cstr(from), trim_left, trim_right);
}

void vut_str_put_blank_reduced_sv(struct vut_str *to, struct vut_sv from, bool trim_left,
				  bool trim_right) {
	size_t start = 0;
	size_t end = from.len;
	if (trim_left) {
		while (start < end && is_blank_char(from.base[start])) {
			start++;
		}
	}
	if (trim_right) {
		while (end > start && is_blank_char(from.base[end - 1])) {
			end--;
		}
	}

	bool is_previous_blank = to->len == 0 || is_blank_char(VUT_VEC_AT(to, to->len));

	VUT_VEC_RESERVE(to, (end - start + 1));
	for (size_t i = start; i < end; i++) {
		char c = from.base[i];

		if (!is_blank_char(c)) {
			VUT_VEC_PUT(to, c);
			is_previous_blank = false;
			continue;
		}
		if (is_previous_blank) {
			continue;
		}
		is_previous_blank = true;
		VUT_VEC_PUT(to, ' ');
	}
}

char *vut_str_move_to_cstr(struct vut_str *str) {
	VUT_VEC_PUT(str, '\0');
	char *ret = str->base;
	*str = (struct vut_str){};
	return ret;
}
