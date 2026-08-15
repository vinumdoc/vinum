#ifndef __VUT_SV_H__
#define __VUT_SV_H__

#include <stdbool.h>

#include "allocator.h"

#define VUT_SV_FMT "%.*s"
#define VUT_SV_ARG(s) (int)(s).len, (s).base

struct vut_sv {
	const char *base;
	size_t len;
};

struct vut_str;

struct vut_sv vut_sv_from_str(const char *str, size_t len);
struct vut_sv vut_sv_from_cstr(const char *cstr);
struct vut_sv vut_sv_from_vut_str(const struct vut_str *str);

char *vut_sv_to_cstr(const struct vut_sv sv, struct vut_allocator alloc);

bool vut_sv_eq(struct vut_sv a, struct vut_sv b);
bool vut_sv_is_empty(struct vut_sv sv);

struct vut_sv vut_sv_splitc(struct vut_sv *sv, char c);

#endif // __VUT_SV_H__
