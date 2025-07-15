#include "v_lib.h"
#include "extern_library.h"

struct return_value ctx_get_text(call_ctx ctx) {
	return (struct return_value){ .ptr = ctx->text, .status = true };
}
