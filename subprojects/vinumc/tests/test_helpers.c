#include "test_helpers.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <vutils/str.h>
#include <vutils/sv.h>
#include <vutils/vec.h>

#define STDERR_CAPTURE_MAX 4096

static void stderr_capture_begin(int saved_fd[1], int pipe_fd[2]) {
	fflush(stderr);
	saved_fd[0] = dup(STDERR_FILENO);
	if (pipe(pipe_fd) != 0) {
		pipe_fd[0] = -1;
		pipe_fd[1] = -1;
		return;
	}
	dup2(pipe_fd[1], STDERR_FILENO);
	close(pipe_fd[1]);
}

static void stderr_capture_end(struct vunit_test_ctx *ctx, int saved_fd, int read_fd) {
	fflush(stderr);
	dup2(saved_fd, STDERR_FILENO);
	close(saved_fd);

	char err[STDERR_CAPTURE_MAX] = { 0 };
	ssize_t n = read(read_fd, err, sizeof(err) - 1);
	close(read_fd);

	if (n > 0)
		fprintf(stderr, "PROGRAM STDERR:\n%s\n", err);

	VUNIT_ASSERT_EQ(ctx, strlen(err), 0);
}

static char *do_compile(struct vunit_test_ctx *ctx, struct compiler_ctx *comp,
			const char *prg_cstr) {
	int saved_fd[1];
	int pipe_fd[2];
	stderr_capture_begin(saved_fd, pipe_fd);

	struct vut_str prg = vut_str_init(ctx->allocator);
	vut_str_put_cstr(&prg, prg_cstr);

	struct vut_str cocktail = vut_str_init(ctx->allocator);
	struct vut_str out = compiler_compile(comp, &prg, &cocktail);
	char *result = vut_str_move_to_cstr(&out);

	stderr_capture_end(ctx, saved_fd[0], pipe_fd[0]);

	return result;
}

char *compile_program(struct vunit_test_ctx *ctx, const char *prg_cstr) {
	struct compiler_ctx comp = compiler_ctx_init(ctx->allocator);
	return do_compile(ctx, &comp, prg_cstr);
}

char *compile_program_with_libraries(struct vunit_test_ctx *ctx, const char *prg_cstr, ...) {
	struct compiler_ctx comp = compiler_ctx_init(ctx->allocator);

	va_list ap;
	va_start(ap, prg_cstr);
	char *curr_lib;
	while ((curr_lib = va_arg(ap, char *)) != NULL)
		VUT_VEC_PUT(&comp.libraries, vut_sv_from_cstr(curr_lib));
	va_end(ap);

	return do_compile(ctx, &comp, prg_cstr);
}
