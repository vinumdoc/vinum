#ifndef __EXTERN_LIBRARY_H__
#define __EXTERN_LIBRARY_H__

#include <stdbool.h>

/**
 * The ctx type passed to extern library functions
 */
typedef struct _call_ctx *call_ctx;

/**
 * The return value returned by extern functions
 */
struct return_value {
	/** the text returned  */
	char *ptr;

	/** If the text should be freed */
	bool free;

	/** The return status, is false something went wrong */
	bool status;
};

/**
 * The extern function signature
 */
typedef struct return_value (*extern_function_pointer)(call_ctx ctx);

/**
 * Struct used to report what functions the external library exposes
 */
struct extern_function {
	/** Function name that will be used in the vin code to call the function */
	char *name;

	/** C function that will be executed */
	extern_function_pointer fp;
};

struct loaded_lib {
	void *dl_handle;
	struct extern_function *functions;
};

struct return_value ctx_get_arg(call_ctx ctx);
struct return_value ctx_eval_symbol(call_ctx ctx, const char *name);

#endif //__EXTERN_LIBRARY_H__
