#include <vunit/vunit.h>

#include <vutils/futils.h>

void test_fut_get_size_FILE(struct vunit_test_ctx *ctx) {
	FILE *fp = fopen("test.txt", "wr");

	char buff[1024];
	fwrite(buff, 1024, 1, fp);

	long size = vut_fut_get_size_FILE(fp);

	VUNIT_ASSERT_EQ(ctx, size, 1024);
}

void test_fut_read_all_FILE(struct vunit_test_ctx *ctx) {
	FILE *fp = fopen("test.txt", "w+");

	const char buff[] = "hello world!";
	fwrite(buff, sizeof(buff), 1, fp);

	struct vut_str msg = vut_fut_read_all_FILE(fp, &ctx->allocator);

	char *ret = vut_str_move_to_cstr(&msg);

	VUNIT_ASSERT_STREQ(ctx, ret, "hello world!");
}

struct vunit_test tests[] = {
	{ .name = "Test vut_fut_get_size_FILE behavior", .test_func = test_fut_get_size_FILE },
	{ .name = "Test vut_fut_read_all_FILE behavior", .test_func = test_fut_read_all_FILE },
	{},
};

VUNIT_TEST_SUITE(tests)
