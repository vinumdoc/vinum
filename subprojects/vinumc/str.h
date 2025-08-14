#ifndef __STR_H__
#define __STR_H__

#include <stdbool.h>

#include "vec.h"

struct str VEC_DEF(char);

void put_str(struct str *to, char *from);

void put_blank_reduced_str(struct str *to, char *from, bool trim_left, bool trim_right);

#endif // __STR_H__
