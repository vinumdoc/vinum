#ifndef __VUT_FUTILS_H__
#define __VUT_FUTILS_H__

#include <stdio.h>

#include "str.h"

long vut_fut_get_size_FILE(FILE *fp);
struct vut_str vut_fut_read_all_FILE(FILE *fp, struct vut_allocator *alloc);

#endif // __VUT_FUTILS_H__
