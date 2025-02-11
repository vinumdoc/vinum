# vinumc

The Vinum compiler, it turns the `.vin` files into your desired output document.

> [!IMPORTANT]
> This software is in pre-alpha stage.

## How to use

```console
vinumc doc.vin --output doc.html
```

## Meson options

- `build_vin2dot`: Build vin2dot utility. it prints some vinumc internal
  structure in dot format, useful for debugging
