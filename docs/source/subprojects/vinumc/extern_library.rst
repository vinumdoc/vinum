==================================
Creating an External Vinum Library
==================================

The vinumc compiler allows you to execute C functions directly from within your Vinum code. To
achieve this, you must compile your C code into a shared library that follows a specific structural
contract.

Requirements
------------

Include the `extern_library.h` from the vinumc subproject.


Define Functions
----------------

Every C function intended for use within vinum code must have the
:c:type:`extern_function_pointer` signature.

Use the API functions
---------------------

.. c:autofunction:: ctx_get_text
   :file: subprojects/vinumc/include/vinumc/extern_library.h

Register your functions
-----------------------

To make the functions visible to the compiler, the external library must have a
`expose_library` entry point. This function will return an array of
:c:struct:`extern_function`'s. This array will tell the compiler which functions will be expose for
the vinum code.

To register your functions create the `expose_library` function:

.. code-block:: c

  struct extern_function* expose_library() {
    static struct extern_function lib[] = {
      {"function1_name", function1_pointer},
      {"function2_name", function2_pointer},
      {}};
    return lib;
  }

Compile it as a shared library
------------------------------

Compile your library as a shared object:

.. code-block:: bash

   gcc -shared -fPIC -o your_lib.so your_lib.c -I/path/to/extern_library.h -L/path/to/libextern_library.so -lextern_library

Usage
-----

You can now use your library with:

.. code-block:: bash

   vinumc --with path/to/your_lib.so

Complet Example
---------------

Here is a complete example of a external library source code:

.. literalinclude:: ../../../../subprojects/vinumc/tests/testlib.c
   :language: c

Reference
-------

.. c:autodoc:: subprojects/vinumc/include/vinumc/extern_library.h

