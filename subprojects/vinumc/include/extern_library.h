#ifndef __EXTERN_LIBRARY_HELPER_H__
#define __EXTERN_LIBRARY_HELPER_H__

typedef char* (*extern_function_pointer) (char *argv);

struct extern_function {
  char *name;
  extern_function_pointer fp;
};

struct loaded_lib {
  void *dl_handle;
  struct extern_function *functions;
};

struct loaded_lib load_lib(char *name);
void unload_lib(struct loaded_lib lib);

#endif //__EXTERN_LIBRARY_HELPER_H__
