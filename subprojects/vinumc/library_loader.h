#ifndef __LIBRARY_LOADER_H__
#define __LIBRARY_LOADER_H__

#include <vutils/allocator.h>
#include <vutils/sv.h>

struct loaded_lib load_lib(struct vut_sv name, struct vut_allocator alloc);
void unload_lib(struct loaded_lib lib);

#endif //__LIBRARY_LOADER_H__
