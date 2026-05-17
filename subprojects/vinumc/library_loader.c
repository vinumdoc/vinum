#include <assert.h>
#include <dlfcn.h>

#include <extern_library.h>

#include "library_loader.h"

struct loaded_lib load_lib(struct vut_sv name, struct vut_allocator alloc) {
	char *name_cstr = vut_sv_to_cstr(name, alloc);
	void *handle = dlopen(name_cstr, RTLD_LAZY);
	vut_allocator_free(&alloc, name_cstr);

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
