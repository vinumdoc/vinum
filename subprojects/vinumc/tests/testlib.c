#include "extern_library.h"
#include <stdlib.h>
#include <string.h>

struct return_value return_arg(call_ctx ctx) {
	return ctx_get_text(ctx);
}

struct return_value parenthesize(call_ctx ctx) {
	struct return_value ret = ctx_get_text(ctx);
	if (!ret.status) {
		return ret;
	}
	char *arg_text = ret.ptr;

	size_t len = strlen(arg_text);
	char *result = malloc(len + 3);

	result[0] = '(';
	memcpy(result + 1, arg_text, len);
	result[len + 1] = ')';
	result[len + 2] = '\0';

	return (struct return_value){ .ptr = result, .free = true, .status = true };
}

struct extern_function *expose_library() {
	static struct extern_function lib[] = { { "return_arg", return_arg },
						{ "parenthesize", parenthesize },
						{} };
	return lib;
}
