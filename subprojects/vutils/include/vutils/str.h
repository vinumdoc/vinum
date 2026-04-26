#ifndef __VUT_STR_H__
#define __VUT_STR_H__

#include <stdbool.h>

#include "vec.h"

#define VUT_STR_FMT "%.*s"
#define VUT_STR_ARG(s) (int)(s).len, (s).base

struct vut_str VUT_VEC_DEF(char);

struct vut_str vut_str_init(struct vut_allocator *alloc);
void vut_str_free(struct vut_str *str);

void vut_put_str(struct vut_str *to, const char *from);

void vut_put_blank_reduced_str(struct vut_str *to, char *from, bool trim_left, bool trim_right);

char *vut_str_move_to_cstr(struct vut_str *str);

#endif // __VUT_STR_H__
