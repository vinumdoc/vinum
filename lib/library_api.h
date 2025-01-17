#ifndef __LIBRARY_API_H__
#define __LIBRARY_API_H__

#include "../src/extern_library_helper.h"

#define FUNCTION(name, func) (extern_function){name, func}

#define DEFINE_LIBRARY(...)                                     \
    library_definition expose_library() {                       \
        static extern_function functions[] = {__VA_ARGS__};     \
        library_definition lib = {                              \
            sizeof(functions) / sizeof(extern_function),        \
            functions                                           \
        };                                                      \
        return lib;                                             \
    }

#endif //__LIBRARY_API_H__
