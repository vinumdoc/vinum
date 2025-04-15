#include "extern_library.h"

char *return_arg(char *arg) {
	return arg;
}

struct extern_function *expose_library() {
	static struct extern_function lib[] = { { "return_arg", return_arg }, {} };
	return lib;
}
