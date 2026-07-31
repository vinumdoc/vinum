#include <vinumc/extern_library.h>

struct extern_function *vsl_expose_library() {
	static struct extern_function lib[] = { {} };
	return lib;
}
