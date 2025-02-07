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

	printf("TAP version 14\n");
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

static char* read_all_from_pipe(struct vunit_test_ctx *ctx, int fd) {
	static char tmp_buf[1024];

	ssize_t read_len = 0;
	size_t total_len = 0;
	char *ret = malloc(sizeof(*ret));

	while((read_len = read(fd, tmp_buf, sizeof(tmp_buf))) > 0) {
		ret = realloc(ret, (total_len + read_len) * sizeof(*ret));
		memcpy(ret + total_len, tmp_buf, sizeof(*ret) * read_len);
		total_len += read_len;
	}
	VUNIT_ASSERT_EQ(ctx, read_len, 0);

	ret[total_len + 1] = 0;
	
	return ret;
}

int vunit_run_vinumc(struct vunit_test_ctx *ctx, char* input, char **output, char **error,
		     char *const argv[], const int argc) {
	// These variable names refer to the direction of the messsages
	int father_to_child_pipe[2];
	int child_to_father_pipe_stdout[2];
	int child_to_father_pipe_stderr[2];

	int ret = pipe(father_to_child_pipe);
	VUNIT_ASSERT_NEQ_MSG(ctx, ret, -1, "pipe");

	ret = pipe(child_to_father_pipe_stdout);
	VUNIT_ASSERT_NEQ_MSG(ctx, ret, -1, "pipe");

	ret = pipe(child_to_father_pipe_stderr);
	VUNIT_ASSERT_NEQ_MSG(ctx, ret, -1, "pipe");

	pid_t pid = fork();
	VUNIT_ASSERT_NEQ_MSG(ctx, ret, -1, "pipe");

	if (pid != 0) {
		int tx = father_to_child_pipe[1];
		int rx_stdout = child_to_father_pipe_stdout[0];
		int rx_stderr = child_to_father_pipe_stderr[0];

		close(father_to_child_pipe[0]);
		close(child_to_father_pipe_stdout[1]);
		close(child_to_father_pipe_stderr[1]);

		size_t len = strlen(input);
		ssize_t wrote_len = write(tx, input, len);
		VUNIT_ASSERT(ctx, wrote_len >= 0);
		VUNIT_ASSERT(ctx, (size_t)wrote_len == len);

		close(tx);
		int stat;
		ret = wait(&stat);
		VUNIT_ASSERT(ctx, ret == pid && "wait");

		*output = read_all_from_pipe(ctx, rx_stdout);
		*error = read_all_from_pipe(ctx, rx_stderr);

		close(rx_stdout);
		close(rx_stderr);

		VUNIT_ASSERT_NEQ(ctx, WIFEXITED(stat), 0);

		return WEXITSTATUS(stat);
	} else {
		int rx = father_to_child_pipe[0];
		int tx_stdout = child_to_father_pipe_stdout[1];
		int tx_stderr = child_to_father_pipe_stderr[1];

		close(child_to_father_pipe_stdout[0]);
		close(child_to_father_pipe_stderr[0]);
		close(father_to_child_pipe[1]);

		// Redirect stdin, stdout and stderr to the created pipes
		ret = dup2(rx, STDIN_FILENO);
		VUNIT_ASSERT_NEQ_MSG(ctx, ret, -1, "dup2");

		ret = dup2(tx_stdout, STDOUT_FILENO);
		VUNIT_ASSERT_NEQ_MSG(ctx, ret, -1, "dup2");

		ret = dup2(tx_stderr, STDERR_FILENO);
		VUNIT_ASSERT_NEQ_MSG(ctx, ret, -1, "dup2");

		char**args_to_send = malloc((argc + 2) * sizeof(*args_to_send));
		VUNIT_ASSERT_NEQ(ctx, args_to_send, NULL);

		// TODO: Get the absolute path
		char *prg_name = "../vinumc";

		args_to_send[0] = prg_name;
		memcpy(args_to_send + 1, argv, argc * sizeof(*argv));
		args_to_send[argc + 1] = NULL;

		ret = execv(
			prg_name,
			args_to_send
		);

		VUNIT_ASSERT(ctx, 0 && "unreachable");
		return -1;
	}
}

int vunit_run_vinumcv(struct vunit_test_ctx *ctx, char* input, char **output, char **error,
		      va_list ap) {
	char **argv = NULL;
	size_t argc = 0;
	char *curr_arg;
	while ((curr_arg = va_arg(ap, char*)) != NULL) {
		argc++;
		argv = realloc(argv, argc * sizeof(*argv));
		argv[argc - 1] = curr_arg;
	}

	return vunit_run_vinumc(ctx, input, output, error, argv, argc);
}

void vunit_run_vinumc_ok(struct vunit_test_ctx *ctx, char* input, char **output, ...) {
	char *err = NULL;

	va_list ap;
	va_start(ap, output);
	int ret = vunit_run_vinumcv(ctx, input, output, &err, ap);
	va_end(ap);

	VUNIT_ASSERT_EQ(ctx, ret, 0);	
	VUNIT_ASSERT_EQ(ctx, strlen(err), 0);
}
