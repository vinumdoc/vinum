# vinum

<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="./resources/logo/logo-dark-256.png">
    <source media="(prefers-color-scheme: light)" srcset="./resources/logo/logo-light-256.png">
    <img alt="Default vinum logo" src="./resources/logo/logo-dark-256.png">
  </picture>
</p>

Vinum is a document preparation system.

This is a [Monorepo](https://en.m.wikipedia.org/wiki/Monorepo) where all the
Vinum related project exists.

The project are:

## vinumc

The Vinum compiler, it turns the `.vin` files into your desired output document.

## stdlib

A collection of useful functions for vinum.

## VUnit

An unit test framework for testing the others Vinum subprojects inspired by
[KUnit](https://www.kernel.org/doc/html/v6.11/dev-tools/kunit/index.html).

## How to build it

### Dependencies

- C compiler
- Meson
- Bison
- Flex

This project uses [Meson](mesombuild.com) for its build system, all the projects
reside on the [`subproject`](./subprojects/) directory.

This is how you use it.

On the project root, use this command to create the build directory.


```console
meson setup <build_dir>
```

> [!TIP]
> You can make out of tree builds by specifying a path outside the project, like
> `/tmp/build`

After that you can compile the project with:

```console
meson compile -C <build_dir>
```

Each subproject has it build options, you can see and modify them with the
commands below, respectively:

```console
meson configure -C <build_dir>
```

```console
meson configure -C <build_dir> -D'<option>=<option_value>'
```

If you want to run the unit tests:
```console
meson test -C <build_dir>
```
