#ifndef __LIBRARY_LOADER_H__
#define __LIBRARY_LOADER_H__

struct loaded_lib load_lib(char *name);
void unload_lib(struct loaded_lib lib);

#endif //__LIBRARY_LOADER_H__
