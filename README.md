# vinum

Vinum is a document preparation system.

This software is in pre-alpha stage.

# vinumc

The Vinum compiler, it turns the `.vin` files into your desired output document

## How to build it

### Dependencies

- C compiler
- GNU Autotools
- Bison
- Flex

This project uses the GNU Autotools for its build system.

This is how you use it.

One the project root, use `autoreconf` to generate the `./configure` script.

```console
autoreconf -i
```

Run the `./configure` script for generating the Makefile.

```console
./configure
```

And finally, run `make` to build the compiler. For subsequent builds you only
need to run this last command.

```console
make
```
