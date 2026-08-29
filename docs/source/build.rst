============
How to build
============

Dependencies
------------

To build Vinum, you will need the following tools:

* C compiler (GCC or Clang)
* `Meson <https://mesonbuild.com>`_
* Bison
* Flex

If you want to build the documentation you will also need

* `Sphinx <https://www.sphinx-doc.org/en/master/usage/installation.html>`_
* `Hawkmoth <https://jnikula.github.io/hawkmoth/dev/installation.html>`_

This project uses Meson for its build system. All internal libraries and modules
reside in the :file:`subprojects/` directory.

Build Instructions
------------------

On the project root, use this command to create the build directory:

.. code-block:: bash

   meson setup <build_dir>

.. tip::
   You can perform out-of-tree builds by specifying a path outside the project
   directory, such as :file:`/tmp/build`.

After setup, compile the project using:

.. code-block:: bash

   meson compile -C <build_dir>

If you want to build the documentation you can run:

.. code-block:: bash

   meson configure -Ddocs=enabled <build_dir>
   meson compile -C <build_dir> docs

The html files will be located on <build_dir>/docs/html

Configuring Subprojects
-----------------------

Each subproject has its own specific build options. You can view or modify them
using the following commands:

**To view current options:**

.. code-block:: bash

   meson configure -C <build_dir>

**To modify an option:**

.. code-block:: bash

   meson configure -C <build_dir> -D<option>=<option_value>

Testing and Running
-------------------

**To run the unit tests:**

.. code-block:: bash

   meson test -C <build_dir>

**To run a binary without installing it:**

.. code-block:: bash

   ./<build_dir>/subprojects/<subproject>/<binary>

Installation
------------

To install the binaries and libraries to your system, run:

.. code-block:: bash

   meson install -C <build_dir>
