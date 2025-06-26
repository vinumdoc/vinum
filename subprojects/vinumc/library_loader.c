#include <assert.h>
#include <dlfcn.h>
#include <extern_library.h>

struct loaded_lib load_lib(char *name) {
	void *handle = dlopen(name, RTLD_LAZY);

	// TODO: deal with the error of not being able to open the library
	assert(handle);

	struct extern_function *(*expose_library)() = dlsym(handle, "expose_library");

	struct extern_function *functions = expose_library();

	struct loaded_lib lib = { handle, functions };

	return lib;
}

void unload_lib(struct loaded_lib lib) {
	dlclose(lib.dl_handle);
}
