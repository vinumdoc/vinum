#include <string.h>

#include <str.h>

static bool is_blank_char(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

void vut_put_str(struct vut_str *to, char *from) {
	if ((to)->len > 0) {
		VUT_VEC_POP(to);
	}
	size_t _len = strlen(from);
	VUT_VEC_PUT_MANY(to, from, _len);
	VUT_VEC_PUT(to, '\0');
}

// Puts the contents of `from` into `to`, performing blank reduction,
// that is, collapsing consecutive blank characters into a single space.
//
// If `trim_left` is true, leading blanks are removed.
// If `trim_right` is true, trailing blanks are removed.
void vut_put_blank_reduced_str(struct vut_str *to, char *from, bool trim_left, bool trim_right) {
	if (to->len > 0) {
		VUT_VEC_POP(to);
	}

	size_t start = 0;
	size_t end = strlen(from);
	if (trim_left) {
		while (start < end && is_blank_char(from[start])) {
			start++;
		}
	}
	if (trim_right) {
		while (end > start && is_blank_char(from[end - 1])) {
			end--;
		}
	}

	bool is_previous_blank = to->len == 0 || is_blank_char(VUT_VEC_AT(to, to->len - 1));

	VUT_VEC_RESERVE(to, (end - start + 1));
	for (size_t i = start; i < end; i++) {
		char c = from[i];

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
	VUT_VEC_PUT(to, '\0');
}
