#include <assert.h>
#include <getopt.h>
#include <locale.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <vutils/futils.h>
#include <vutils/str.h>
#include <vutils/system_allocator.h>
#include <vutils/vec.h>

#include "vinumc.h"

#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof(*(arr)))

struct ctx ctx_new(struct vut_allocator *allocator) {
	struct ctx ret = {
		.compiler = compiler_ctx_init(allocator),
		.alloc = allocator,
	};

	return ret;
}

enum flag_kind {
	FLAG_BOOLEAN,
	FLAG_ARGUMENT,
	FLAG_MULTI_ARGUMENTS,
};

struct flag {
	const char *name;
	char short_name;
	const char *help_desc;
	const char *placeholder_name;
	enum flag_kind kind;
	union {
		bool *boolean;
		char **str;
		struct str_vec *str_vec;
	} ref_as;
};

static struct flag *find_flag_by_short_name(struct flag *flags, size_t flags_len, char short_flag) {
	for (size_t i = 0; i < flags_len; i++) {
		struct flag *f = &flags[i];

		if (f->short_name == short_flag)
			return f;
	}

	return NULL;
}

static void parse_cmdline(const int argc, char **argv, struct ctx *ctx, struct flag *flags,
			  size_t flags_len) {
	char *optstring = calloc(2 * (flags_len + 1), sizeof(*optstring));
	struct option *longopts = calloc(flags_len + 1, sizeof(*longopts));

	size_t optstring_i = 0;
	for (size_t i = 0; i < flags_len; i++) {
		struct flag *f = &flags[i];

		// TODO: support short/long only flags
		assert(f->short_name != 0);
		assert(f->name != 0);

		optstring[optstring_i++] = f->short_name;

		struct option opt = { .name = f->name, .val = f->short_name };

		switch (f->kind) {
		case FLAG_MULTI_ARGUMENTS:
		case FLAG_ARGUMENT:
			opt.has_arg = required_argument;
			optstring[optstring_i++] = ':';
			break;
		case FLAG_BOOLEAN:
			opt.has_arg = no_argument;
			break;
		}

		longopts[i] = opt;
	}

	int opt;
	while ((opt = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
		struct flag *f = find_flag_by_short_name(flags, flags_len, opt);
		assert(f != NULL);

		switch (f->kind) {
		case FLAG_ARGUMENT:;
			size_t arg_len = strlen(optarg);
			*f->ref_as.str = calloc(arg_len + 1, sizeof(char));
			strcpy(*f->ref_as.str, optarg);
			break;
		case FLAG_BOOLEAN:
			*f->ref_as.boolean = true;
			break;
		case FLAG_MULTI_ARGUMENTS: {
			char *tmp = strdup(optarg);
			VUT_VEC_PUT(f->ref_as.str_vec, tmp);
		} break;
		}
	}

	free(optstring);
	free(longopts);

	// TODO: Find a way to add this as an item in the `vinumc_flags`
	if (optind < argc) {
		char *input_path = argv[optind];
		size_t input_path_len = strlen(input_path);
		ctx->input_path = calloc(input_path_len + 1, sizeof(char));
		strcpy(ctx->input_path, input_path);
	}
}

static void free_flags(struct flag *flags, size_t flags_len) {
	for (size_t i = 0; i < flags_len; i++) {
		struct flag *f = &flags[i];
		switch (f->kind) {
		case FLAG_ARGUMENT:
			free(*f->ref_as.str);
			break;
		case FLAG_MULTI_ARGUMENTS:
			// TODO create free for VEC
			break;
		default:
			break;
		}
	}
}

static struct flag *print_help(const char *prg_name, struct flag *flags, size_t flags_len) {
	printf("usage: %s ", prg_name);
	for (size_t i = 0; i < flags_len; i++) {
		struct flag *f = &flags[i];
		printf("[-%c | --%s", f->short_name, f->name);
		if (f->kind == FLAG_ARGUMENT || f->kind == FLAG_MULTI_ARGUMENTS) {
			assert(f->placeholder_name != NULL);
			printf(" <%s>", f->placeholder_name);
		}
		printf("] ");
	}
	printf("[<file-input>]\n\n");
	printf("Options:");
	for (size_t i = 0; i < flags_len; i++) {
		struct flag *f = &flags[i];
		assert(f->help_desc != NULL);
		printf("\n  -%c, --%s", f->short_name, f->name);
		if (f->kind == FLAG_MULTI_ARGUMENTS) {
			printf(" (can be specified multiple times)");
		}
		printf("\n\t%s\n", f->help_desc);
	}
	return NULL;
}

int main(int argc, char **argv) {
	setlocale(LC_ALL, "");

	struct ctx ctx = ctx_new(vut_get_system_allocator());

	struct flag vinumc_flags[] = {
		{
			.name = "output",
			.short_name = 'o',
			.help_desc = "Set file name output, if not set the output will be stdout",
			.placeholder_name = "output_path",
			.kind = FLAG_ARGUMENT,
			.ref_as.str = &ctx.output_path,
		},
		{
			.name = "help",
			.short_name = 'h',
			.help_desc = "Show help",
			.kind = FLAG_BOOLEAN,
			.ref_as.boolean = &ctx.show_help,
		},
		{
			.name = "with",
			.short_name = 'w',
			.help_desc = "Set a library to be loaded",
			.placeholder_name = "libraries",
			.kind = FLAG_MULTI_ARGUMENTS,
			.ref_as.str_vec = &ctx.compiler.libraries,
		},
	};

	parse_cmdline(argc, argv, &ctx, vinumc_flags, ARRAY_SIZE(vinumc_flags));

	if (ctx.show_help) {
		print_help("vinumc", vinumc_flags, ARRAY_SIZE(vinumc_flags));
		goto exit;
	}

	FILE *out = stdout;
	if (ctx.output_path != NULL)
		out = fopen(ctx.output_path, "w");

	struct vut_str program_str;
	if (ctx.input_path != NULL) {
		FILE *input = fopen(ctx.input_path, "r");
		program_str = vut_fut_read_all_FILE(input, ctx.alloc);
		fclose(input);
	} else {
		program_str = vut_fut_read_all_FILE(stdin, ctx.alloc);
	}

	struct vut_str str_out = compiler_compile(&ctx.compiler, &program_str);

	vut_str_free(&program_str);

	fprintf(out, VUT_STR_FMT, VUT_STR_ARG(str_out));

	vut_str_free(&str_out);

exit:
	free_flags(vinumc_flags, ARRAY_SIZE(vinumc_flags));
	free(ctx.input_path);
}
