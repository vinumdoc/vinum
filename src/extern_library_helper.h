#ifndef __EXTERN_LIBRARY_HELPER_H__
#define __EXTERN_LIBRARY_HELPER_H__

typedef char* (*function_pointer) (char* argv);

typedef struct {
  char* name;
  function_pointer fp;
} extern_function;

typedef struct {
  int counter;
  extern_function* functions;
} library_definition;

typedef struct {
  void* dl_handle;
  library_definition functions;
} loaded_lib;

loaded_lib load_lib(char* name);
void unload_lib(loaded_lib lib);

#endif //__EXTERN_LIBRARY_HELPER_H__
