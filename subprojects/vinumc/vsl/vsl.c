#include <stdlib.h>
#include <string.h>

#include <vinumc/extern_library.h>

struct return_value comment(call_ctx ctx) {
	(void)ctx;
	struct return_value ret = { 0 };
	ret.status = 1;
	return ret;
}

struct extern_function *vsl_expose_library() {
	static struct extern_function lib[] = { { "#", comment }, { "comment", comment }, {} };
	return lib;
}
