#ifndef __LIBRARY_API_H__
#define __LIBRARY_API_H__

#include <extern_library.h>

#define DEFINE_LIBRARY(...)                                          \
    struct extern_function* expose_library() {                       \
        static struct extern_function lib[] = {__VA_ARGS__, {}};     \
        return lib;                                                  \
    }

#endif //__LIBRARY_API_H__
