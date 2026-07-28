#ifndef __VINUMC_H__
#define __VINUMC_H__

#include <stdbool.h>

#include <vutils/allocator.h>
#include <vutils/str.h>

#include "libvinumc.h"

struct ctx {
	char *input_path;
	char *output_path;
	bool show_help;
	bool show_version;

	struct compiler_ctx compiler;

	struct vut_allocator alloc;
};

struct ctx ctx_new(struct vut_allocator allocator);

#endif // __VINUMC_H__
