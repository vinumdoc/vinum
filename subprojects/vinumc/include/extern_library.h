#ifndef __EXTERN_LIBRARY_H__
#define __EXTERN_LIBRARY_H__

typedef char *(*extern_function_pointer)(char *argv);

typedef enum {freeable, non_freeable} mem_handle_t; 

struct extern_function {
	char *name;
	extern_function_pointer fp;
	mem_handle_t memory;
};

struct loaded_lib {
	void *dl_handle;
	struct extern_function *functions;
};

#endif //__EXTERN_LIBRARY_H__
