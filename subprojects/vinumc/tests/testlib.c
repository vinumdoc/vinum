#include "extern_library.h"
#include <stdlib.h>
#include <string.h>

struct return_value return_arg(char *arg) {
	return (struct return_value){ .ptr = arg };
}

struct return_value parenthesize(char *arg) {
	size_t len = strlen(arg);
	char *result = malloc(len + 3);

	result[0] = '(';
	memcpy(result + 1, arg, len);
	result[len + 1] = ')';
	result[len + 2] = '\0';

	return (struct return_value){ .ptr = result, .free = true };
}

struct extern_function *expose_library() {
	static struct extern_function lib[] = { { "return_arg", return_arg },
						{ "parenthesize", parenthesize },
						{} };
	return lib;
}
