#include "v_lib.h"
#include "extern_library.h"

struct return_value ctx_get_arg(call_ctx ctx) {
	return (struct return_value){ .ptr = ctx->text, .status = true };
}
