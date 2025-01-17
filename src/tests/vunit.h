#ifndef __VUNIT_H__
#define __VUNIT_H__

#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

struct vunit_test_ctx {
	jmp_buf env;
	char *lonjmp_msg;
};

typedef void (*vunit_test_func_t)(struct vunit_test_ctx*);

struct vunit_test {
	char *name;
	vunit_test_func_t test_func;
};

#define VUNIT_MAX_NUM_TEST 1024

#define VUNIT_TEST_SUITE(name, ...) \
	int main(int argc, char *argv[]) { \
		const struct vunit_test tests[] = {__VA_ARGS__}; \
		return __vunit_main(tests, argc, argv); \
	}
	

#define TAP_TAB "    "

#define VUNIT_ASSERT_YAML(ctx, predicate, yaml) \
	__vunit_assert((ctx), (predicate), #predicate, yaml, __FILE__, __LINE__)
#define VUNIT_ASSERT_MSG_YAML(ctx, predicate, msg, yaml) \
	VUNIT_ASSERT_YAML((ctx), (predicate), TAP_TAB "message: " msg "\n" yaml)
#define VUNIT_ASSERT_MSG(ctx, predicate, msg) VUNIT_ASSERT_MSG_YAML((ctx), (predicate), msg, "")
#define VUNIT_ASSERT(ctx, predicate) VUNIT_ASSERT_YAML((ctx), (predicate), "")

#define VUNIT_ASSERT_EQ_MSG(ctx, lhs, rhs, msg) \
	VUNIT_ASSERT_MSG_YAML((ctx), (lhs) == (rhs), msg, \
			TAP_TAB "reason: EQ failed\n" \
			TAP_TAB "lhs: " #lhs "\n" \
			TAP_TAB "rhs: " #rhs "\n" \
			)
#define VUNIT_ASSERT_EQ(ctx, lhs, rhs) VUNIT_ASSERT_EQ_MSG((ctx), (lhs), (rhs), "")
#define VUNIT_ASSERT_NEQ_MSG(ctx, lhs, rhs, msg) \
	VUNIT_ASSERT_MSG_YAML((ctx), (lhs) != (rhs), msg, \
			TAP_TAB "reason: NEQ failed\n" \
			TAP_TAB "lhs: " #lhs "\n" \
			TAP_TAB "rhs: " #rhs "\n" \
			)
#define VUNIT_ASSERT_NEQ(ctx, lhs, rhs) VUNIT_ASSERT_NEQ_MSG((ctx), (lhs), (rhs), "")

#define VUNIT_ASSERT_STREQ_YAML(ctx, lhs, rhs, yaml) \
	__vunit_assert_strcmp((ctx), (lhs), (rhs), EQ, yaml, __FILE__, __LINE__)
#define VUNIT_ASSERT_STREQ(ctx, lhs, rhs) VUNIT_ASSERT_STREQ_YAML((ctx), (lhs), (rhs), "")

void __vunit_assert(struct vunit_test_ctx *ctx, const bool predicate, const char *predicate_str,
		    const char *yaml, const char *file_path, const size_t linenum);

enum __vunit_strcmp_res {
	EQ,
	NEQ,
	LT,
	GT,
};

void __vunit_assert_strcmp(struct vunit_test_ctx *ctx, const char *lhs, const char *rhs,
			   const enum __vunit_strcmp_res desire, const char *yaml,
			   const char *file_path, const size_t linenum);

int __vunit_main(const struct vunit_test *tests, int argc, char *argv[]);

#endif // __VUNIT_H__
