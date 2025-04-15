#include "extern_library.h"

struct return_value return_arg(char *arg) {
	return (struct return_value) {.ptr=arg};
}

struct extern_function *expose_library() {
	static struct extern_function lib[] = { { "return_arg", return_arg }, {} };
	return lib;
}
