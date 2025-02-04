# VUnit

An unit test framework inspired by
[KUnit](https://www.kernel.org/doc/html/v6.11/dev-tools/kunit/index.html) for
testing the others Vinum subprojects.

A example of a standalone VUnit test file:

```c
#include <vunit.h>

char* hello() {
    return "Hello World!"
}

void test(struct vunit_test_ctx *ctx) {
	char *out = hello();

	VUNIT_ASSERT_STREQ(ctx, out, "Hello World!");
}

VUNIT_TEST_SUITE("suite",
	{"Basic test", test},
	{},
)
```

For the output it uses the [TAP](https://testanything.org/) protocol.
