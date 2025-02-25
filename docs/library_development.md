
Creating a Vinum library
==============================

Follow these steps to create a Vinum library:

 1. Include the `extern_library.h` from the vinumc subproject.

 2. Define functions, they should accept a parameter of type `char*` and return a `char*`.

 3. Register your functions by creating the `expose_library` function:
  ``` c
struct extern_function* expose_library() {
  static struct extern_function lib[] = {
    {"function1_name", function1_pointer},
    {"function2_name", function2_pointer},
    {}};
  return lib;
}
  ```

 4. Compile it as a shared library.

 5. You can now use your library by calling vinumc with `--with path/to/library`.
