#ifndef __EXTERN_LIBRARY_H__
#define __EXTERN_LIBRARY_H__

typedef char *(*extern_function_pointer)(char *argv);

struct extern_function {
	char *name;
	extern_function_pointer fp;
};

struct loaded_lib {
	void *dl_handle;
	struct extern_function *functions;
};

#endif //__EXTERN_LIBRARY_H__
