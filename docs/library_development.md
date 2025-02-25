
Creating a Vinum library
==============================
 
Follow these steps to create a Vinum library:

 1. Include the `library_api.h`.

 2. Define functions, they should accept a parameter of type char* and return a char*. 

 3. Register your functions using the macro `DEFINE_LIBRARY` this way:
   ``` c
  DEFINE_LIBRARY(
      {"function_name", function_pointer},
      {"function_name2", function_pointer2}
      )
  ```

 4. Compile it as a shared library and you're good to go.
