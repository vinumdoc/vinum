#ifndef __VUT_STR_H__
#define __VUT_STR_H__

#include <stdbool.h>

#include "sv.h"
#include "vec.h"

#define VUT_STR_FMT "%.*s"
#define VUT_STR_ARG(s) (int)(s).len, (s).base

struct vut_str VUT_VEC_DEF(char);

struct vut_str vut_str_init(struct vut_allocator alloc);
struct vut_str vut_str_from_vut_sv(struct vut_sv sv, struct vut_allocator alloc);
void vut_str_free(struct vut_str *str);

void vut_str_put_cstr(struct vut_str *to, const char *from);
void vut_str_put_sv(struct vut_str *to, const struct vut_sv sv);

void vut_str_put_blank_reduced_cstr(struct vut_str *to, char *from, bool trim_left,
				    bool trim_right);
void vut_str_put_blank_reduced_sv(struct vut_str *to, struct vut_sv from, bool trim_left,
				  bool trim_right);

char *vut_str_move_to_cstr(struct vut_str *str);

#endif // __VUT_STR_H__
