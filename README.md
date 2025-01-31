# vinum

Vinum is a document preparation system.

This software is in pre-alpha stage.

# vinumc

The Vinum compiler, it turns the `.vin` files into your desired output document

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
