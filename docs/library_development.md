# Creating a Vinum library

This guide explains how to create a Vinum library.

## 1. Requirements

Include the `extern_library.h` from the vinumc subproject.

## 2. Define Functions

Functions must follow this signature:
```c
struct return_value your_function (call_ctx ctx)
```

Where return_value is defined as:
```c
struct return_value {
  char *ptr; // the return string
  bool free; // indicates whether ptr should be freed
  bool status; // indicates if the function succeed
};
```

## 3. Use the API functions
You can use this functions to get data from the callee context:
```c
// get all the args text
struct return_value ctx_get_arg(call_ctx ctx);
```

## 4. Register your functions

To register your functions create the `expose_library` function:
``` c
struct extern_function* expose_library() {
  static struct extern_function lib[] = {
    {"function1_name", function1_pointer},
    {"function2_name", function2_pointer},
    {}};
  return lib;
}
```

## 5. Compile it as a shared library

Compile your library as a shared object:
```sh
gcc -shared -fPIC -o your_lib.so your_lib.c -I/path/to/extern_library.h -L/path/to/libextern_library.so -lextern_library
```
Where

## 6. Usage

You can now use your library with:
```sh
vinumc --with path/to/your_lib.so
```
