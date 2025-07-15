#ifndef __EXTERN_LIBRARY_H__
#define __EXTERN_LIBRARY_H__

#include <stdbool.h>

typedef struct _call_ctx *call_ctx;

struct return_value {
	char *ptr;
	bool free;
	bool status;
};

typedef struct return_value (*extern_function_pointer)(call_ctx ctx);

struct extern_function {
	char *name;
	extern_function_pointer fp;
};

struct loaded_lib {
	void *dl_handle;
	struct extern_function *functions;
};

struct return_value ctx_get_text(call_ctx ctx);

#endif //__EXTERN_LIBRARY_H__
