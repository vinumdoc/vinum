#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>

#include <vutils/system_allocator.h>

#include <vunit.h>

#define VUNIT_ARENA_SIZE 1024 * 1024 * 32

static char *alloc_printf(struct vunit_test_ctx *ctx, const char *fmt, ...) {
	va_list ap;

	char *ret = NULL;

	va_start(ap, fmt);
	int len = vsnprintf(ret, 0, fmt, ap);
	va_end(ap);

	assert(len >= 0);
	len++;

	ret = vut_allocator_malloc(ctx->allocator, sizeof(*ret), len);
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

	ctx->lonjmp_msg = alloc_printf(ctx,
				       TAP_TAB "---\n"
					       "%s" TAP_TAB "condition: %s\n" TAP_TAB
					       "location: %s:%d\n" TAP_TAB "...",
				       yaml, predicate_str, file_path, linenum);

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
		new_yaml = alloc_printf(ctx,
					TAP_TAB "reason: \"Strings are not equal!\"\n" TAP_TAB
						"lhs: '%s'\n" TAP_TAB "rhs: '%s'\n"
						"%s",
					lhs, rhs, yaml);
		break;
	case NEQ:
		new_yaml = alloc_printf(ctx,
					TAP_TAB "reason: \"Strings are equal!\"\n" TAP_TAB
						"string: '%s'\n"
						"%s",
					lhs, yaml);
		break;
	case LT:
		new_yaml = alloc_printf(ctx,
					TAP_TAB
					"reason: \"Lhs string is not smaller than rhs\"\n" TAP_TAB
					"lhs: '%s'\n" TAP_TAB "rhs: '%s'\n"
					"%s",
					lhs, rhs, yaml);
		break;
	case GT:
		new_yaml = alloc_printf(ctx,
					TAP_TAB
					"reason: \"Lhs string is not bigger than rhs\"\n" TAP_TAB
					"lhs: '%s'\n" TAP_TAB "rhs: '%s'\n"
					"%s",
					lhs, rhs, yaml);
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

enum test_return_status {
	PASSED = 0,
	FAILED,
	SKIPPED,
};

static enum test_return_status run_test(const struct vunit_test *test, struct vunit_test_ctx *ctx) {
	if (test->skip) {
		return SKIPPED;
	}

	int ret = setjmp(ctx->env);

	bool test_failed = false;
	if (!ret) {
		test->test_func(ctx);
	} else {
		test_failed = true;
	}

	if (test_failed)
		return FAILED;

	return PASSED;
}

int __vunit_main(const struct vunit_test *tests, int argc, char *argv[]) {
	assert(argc <= 2);

	char *pattern = NULL;
	if (argc == 2) {
		pattern = argv[1];
	}

	size_t num_test = get_num_tests(tests);

	printf("TAP version 14\n");
	printf("1..%zu\n", num_test);

	for (size_t i = 0; i < num_test; i++) {
		const struct vunit_test *test = &tests[i];
		struct vut_arena arena =
			vut_arena_new(vut_get_system_allocator(), VUNIT_ARENA_SIZE);
		struct vunit_test_ctx ctx = {
			.allocator = vut_arena_to_vut_allocator(&arena),
		};

		bool skip_test = false;

		if (pattern != NULL && pattern[0] != 0) {
			skip_test = strstr(test->name, pattern) == NULL;
		}

		enum test_return_status status;
		if (!skip_test) {
			status = run_test(test, &ctx);
		} else {
			status = SKIPPED;
		}

		const char *todo_str = "";
		if (test->todo) {
			if (test->todo_msg != NULL && test->todo_msg[0] != '\0')
				todo_str = alloc_printf(&ctx, " # TODO %s", test->todo_msg);
			else
				todo_str = " # TODO";
		}

		switch (status) {
		case PASSED:
			printf("ok %zu - %s%s\n", i + 1, test->name, todo_str);
			break;
		case FAILED:
			printf("not ok %zu - %s%s\n", i + 1, test->name, todo_str);
			if (ctx.lonjmp_msg != NULL && !test->todo)
				printf("%s\n", ctx.lonjmp_msg);
			break;
		case SKIPPED:
			printf("ok %zu - %s # SKIP\n", i + 1, test->name);
			break;
		}

		vut_arena_free_all(&arena);
	}

	return 0;
}

static char *read_all_from_pipe(struct vunit_test_ctx *ctx, int fd) {
	static char tmp_buf[1024];

	ssize_t read_len = 0;
	size_t total_len = 0;
	char *ret = vut_allocator_malloc(ctx->allocator, sizeof(*ret), 1);

	while ((read_len = read(fd, tmp_buf, sizeof(tmp_buf))) > 0) {
		ret = vut_allocator_realloc(ctx->allocator, ret, sizeof(*ret),
					    total_len + read_len + 1);
		memcpy(ret + total_len, tmp_buf, sizeof(*ret) * read_len);
		total_len += read_len;
	}
	VUNIT_ASSERT_EQ(ctx, read_len, 0);

	ret[total_len] = 0;

	return ret;
}

int vunit_run_vinumc(struct vunit_test_ctx *ctx, char *input, char **output, char **error,
		     char *const argv[], const int argc) {
	// A child that exits before reading its stdin (e.g. `vinumc -h`) closes
	// the read end while we're still writing; without this the parent dies
	// from SIGPIPE before we get to assert anything.
	signal(SIGPIPE, SIG_IGN);

	// These variable names refer to the direction of the messsages
	int father_to_child_pipe[2];
	int child_to_father_pipe_stdout[2];
	int child_to_father_pipe_stderr[2];

	int ret;
	if (input != NULL) {
		int ret = pipe(father_to_child_pipe);
		VUNIT_ASSERT_NEQ_MSG(ctx, ret, -1, "pipe");
	}

	if (output != NULL) {
		ret = pipe(child_to_father_pipe_stdout);
		VUNIT_ASSERT_NEQ_MSG(ctx, ret, -1, "pipe");
	}

	ret = pipe(child_to_father_pipe_stderr);
	VUNIT_ASSERT_NEQ_MSG(ctx, ret, -1, "pipe");

	pid_t pid = fork();
	VUNIT_ASSERT_NEQ_MSG(ctx, ret, -1, "fork");

	if (pid != 0) {
		if (input != NULL) {
			int tx = father_to_child_pipe[1];
			close(father_to_child_pipe[0]);
			size_t len = strlen(input);
			ssize_t wrote_len = write(tx, input, len);
			if (wrote_len < 0) {
				VUNIT_ASSERT_MSG(ctx, errno == EPIPE, "write");
			} else {
				VUNIT_ASSERT(ctx, (size_t)wrote_len == len);
			}
			close(tx);
		}

		int rx_stdout = -1;
		if (output != NULL) {
			rx_stdout = child_to_father_pipe_stdout[0];
			close(child_to_father_pipe_stdout[1]);
		}

		int rx_stderr = -1;
		if (error != NULL) {
			rx_stderr = child_to_father_pipe_stderr[0];
			close(child_to_father_pipe_stderr[1]);
		}

		int stat;
		ret = wait(&stat);
		VUNIT_ASSERT(ctx, ret == pid && "wait");

		if (output != NULL) {
			*output = read_all_from_pipe(ctx, rx_stdout);
			close(rx_stdout);
		}

		if (error != NULL) {
			*error = read_all_from_pipe(ctx, rx_stderr);
			close(rx_stderr);
		}

		if (!WIFEXITED(stat))
			return -1;
		else
			return WEXITSTATUS(stat);
	} else {
		if (input != NULL) {
			int rx = father_to_child_pipe[0];
			close(father_to_child_pipe[1]);
			// Redirect stdin, stdout and stderr to the created pipes
			ret = dup2(rx, STDIN_FILENO);
			VUNIT_ASSERT_NEQ_MSG(ctx, ret, -1, "dup2");
		}

		if (output != NULL) {
			int tx_stdout = child_to_father_pipe_stdout[1];
			close(child_to_father_pipe_stdout[0]);
			ret = dup2(tx_stdout, STDOUT_FILENO);
			VUNIT_ASSERT_NEQ_MSG(ctx, ret, -1, "dup2");
		}

		if (error != NULL) {
			int tx_stderr = child_to_father_pipe_stderr[1];
			close(child_to_father_pipe_stderr[0]);
			ret = dup2(tx_stderr, STDERR_FILENO);
			VUNIT_ASSERT_NEQ_MSG(ctx, ret, -1, "dup2");
		}

		char **args_to_send =
			vut_allocator_malloc(ctx->allocator, sizeof(*args_to_send), argc + 2);
		VUNIT_ASSERT_NEQ(ctx, args_to_send, NULL);

		// TODO: Get the absolute path
		char *prg_name = "vinumc";

		args_to_send[0] = prg_name;
		if (argc != 0)
			memcpy(args_to_send + 1, argv, argc * sizeof(*argv));
		args_to_send[argc + 1] = NULL;

		ret = execvp(prg_name, args_to_send);

		VUNIT_ASSERT(ctx, 0 && "unreachable");
		return -1;
	}
}

void vunit_str_to_file(struct vunit_test_ctx *ctx, const char *file_path, const char *str) {
	FILE *fp = fopen(file_path, "w+");
	VUNIT_ASSERT_NEQ(ctx, fp, NULL);

	unsigned long len = strlen(str);

	int ret;

	fwrite(str, sizeof(*str), len, fp);

	ret = fclose(fp);
	VUNIT_ASSERT_NEQ(ctx, ret, EOF);
}

char *vunit_file_to_str(struct vunit_test_ctx *ctx, const char *file_path) {
	FILE *fp = fopen(file_path, "r");
	VUNIT_ASSERT_NEQ(ctx, fp, NULL);

	int ret;

	ret = fseek(fp, 0, SEEK_END);
	VUNIT_ASSERT_NEQ(ctx, ret, -1);

	long file_size = ftell(fp);
	VUNIT_ASSERT_NEQ(ctx, file_size, -1);
	rewind(fp);

	char *ret_str = vut_allocator_calloc(ctx->allocator, file_size + 1, sizeof(*ret_str));
	VUNIT_ASSERT_NEQ(ctx, ret_str, NULL);

	fread(ret_str, sizeof(*ret_str), file_size, fp);

	ret = fclose(fp);
	VUNIT_ASSERT_NEQ(ctx, ret, EOF);

	return ret_str;
}

int vunit_run_vinumcv(struct vunit_test_ctx *ctx, char *input, char **output, char **error,
		      va_list ap) {
	char **argv = NULL;
	size_t argc = 0;
	char *curr_arg;
	while ((curr_arg = va_arg(ap, char *)) != NULL) {
		argc++;
		argv = vut_allocator_realloc(ctx->allocator, argv, sizeof(*argv), argc);
		argv[argc - 1] = curr_arg;
	}

	return vunit_run_vinumc(ctx, input, output, error, argv, argc);
}

void vunit_run_vinumc_ok(struct vunit_test_ctx *ctx, char *input, char **output, ...) {
	char *err = NULL;

	va_list ap;
	va_start(ap, output);
	int ret = vunit_run_vinumcv(ctx, input, output, &err, ap);
	va_end(ap);

	if (strlen(err) != 0) {
		fprintf(stderr, "PROGRAM STDERR:\n%s\n", err);
	}
	VUNIT_ASSERT_EQ(ctx, ret, 0);
	VUNIT_ASSERT_EQ(ctx, strlen(err), 0);
}
