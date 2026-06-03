#ifndef __V_LIB_H__
#define __V_LIB_H__

#include <stdbool.h>

#include "libvinumc.h"

struct _call_ctx {
	char *arg_text;
	struct compiler_ctx *compiler_ctx;
	size_t scope_id;
	size_t ast_node;
};

#endif //__V_LIB_H__
