#ifndef __VINUMC_TEST_HELPERS_H__
#define __VINUMC_TEST_HELPERS_H__

#include <vunit/vunit.h>

#include "libvinumc.h"

char *compile_program(struct vunit_test_ctx *ctx, const char *prg_cstr);
char *compile_program_with_libraries(struct vunit_test_ctx *ctx, const char *prg_cstr, ...);

#endif // __VINUMC_TEST_HELPERS_H__
