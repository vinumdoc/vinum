#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/wait.h>

#include "vunit.h"

static char* alloc_printf(const char* fmt, ...) {
	va_list ap;

	char *ret = NULL;

	va_start(ap, fmt);
	int len = vsnprintf(ret, 0, fmt, ap);
	va_end(ap);

	assert(len >= 0);
	len++;

	ret = malloc(len * sizeof(*ret));
	assert(ret != NULL);

	va_start(ap, fmt);
	vsnprintf(ret, len, fmt, ap);
	va_end(ap);


	return ret;
}

void __vunit_assert(struct vunit_test_ctx *ctx, const bool predicate, const char *predicate_str,
		    const char *yaml, const char *file_path, const size_t linenum) {
	if (predicate)
		return;

	ctx->lonjmp_msg = alloc_printf(TAP_TAB "---\n"
				       "%s"
				       TAP_TAB "condition: %s\n"
				       TAP_TAB "location: %s:%d\n"
				       TAP_TAB "...", yaml, predicate_str, file_path, linenum);

	longjmp(ctx->env, -1);
}

void __vunit_assert_strcmp(struct vunit_test_ctx *ctx, const char *lhs, const char *rhs,
			   const enum __vunit_strcmp_res desire, const char *yaml,
			   const char *file_path, const size_t linenum) {
	char *new_yaml = NULL;

	int ret = strcmp(lhs, rhs);

	if (desire == EQ && ret == 0)
		return;
	else if (desire == NEQ && (ret < 0 || ret > 0))
		return;
	else if (desire == LT && ret < 0)
		return;
	else if (desire == GT && ret > 0)
		return;

	switch (desire) {
		case EQ:
			new_yaml = alloc_printf(TAP_TAB "reason: \"Strings are not equal!\"\n"
						TAP_TAB "lhs: '%s'\n"
						TAP_TAB "rhs: '%s'\n"
						"%s", lhs, rhs, yaml);
			break;
		case NEQ:
			new_yaml = alloc_printf(TAP_TAB "reason: \"Strings are equal!\"\n"
						TAP_TAB "string: '%s'\n"
						"%s", lhs, yaml);
			break;
		case LT:
			new_yaml = alloc_printf(TAP_TAB "reason: \"Lhs string is not smaller than rhs\"\n"
						TAP_TAB "lhs: '%s'\n"
						TAP_TAB "rhs: '%s'\n"
						"%s", lhs, rhs, yaml);
			break;
		case GT:
			new_yaml = alloc_printf(TAP_TAB "reason: \"Lhs string is not bigger than rhs\"\n"
						TAP_TAB "lhs: '%s'\n"
						TAP_TAB "rhs: '%s'\n"
						"%s", lhs, rhs, yaml);
			break;
	}



	__vunit_assert(ctx, 0, "NOT APPLIED", new_yaml, file_path, linenum);
}

static size_t get_num_tests(const struct vunit_test *tests) {
	size_t num = 0;

	while ((tests++)->name != NULL) {
		num++;
		if (num > VUNIT_MAX_NUM_TEST) {
			fprintf(stderr, "ERROR: Maximum number of tests reached! "
					"Please increse the limit");
			exit(EXIT_FAILURE);
		}
	}

	return num;
}

int __vunit_main(const struct vunit_test *tests, int argc, char *argv[]) {
	// TODO: Make use of the command args
	(void)argc;
	(void)argv;

	size_t num_test = get_num_tests(tests);

	printf("TAP Version 14\n");
	printf("1..%zu\n", num_test);

	int return_value = 0;

	for (size_t i = 0; i < num_test; i++) {
		const struct vunit_test *test = &tests[i];
		struct vunit_test_ctx ctx = {};

		int ret = setjmp(ctx.env);

		bool test_failed = false;
		if (!ret) {
			test->test_func(&ctx);
		}
		else {
			return_value = ret;
			test_failed = true;
		}

		if (test_failed)
			printf("not ");

		printf("ok %zu - %s\n", i + 1, test->name);

		if (test_failed && ctx.lonjmp_msg != NULL)
			printf("%s\n", ctx.lonjmp_msg);
	}

	return return_value;
}
