#include "extern_library_helper.h"
#include <dlfcn.h>

loaded_lib load_lib(char* name){
  // load library
  void *handle = dlopen(name, RTLD_LAZY);
  if (!handle) {
    // error
  }

  // clear any existing errors
  dlerror();

  library_definition (*expose_library) () = dlsym(handle, "expose_library");

  library_definition functions = expose_library();

  loaded_lib lib = {
    handle,
    functions
  };

  return lib;
}

void unload_lib(loaded_lib lib) {
  dlclose(lib.dl_handle);
}
