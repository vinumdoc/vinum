#include <vutils/system_allocator.h>

#include "extern_library.h"
#include "v_lib.h"

struct return_value ctx_get_arg(call_ctx ctx) {
	return (struct return_value){ .ptr = ctx->arg_text, .status = true };
}

struct return_value ctx_eval_symbol(call_ctx ctx, const char *name) {
	struct compiler_ctx *comp_ctx = ctx->compiler_ctx;
	struct namespace_entry *symbol_info = eval_find_symbol_on_scopes(
		&comp_ctx->eval_ctx, ctx->scope_id, vut_sv_from_cstr(name));
	struct vut_str text = vut_str_init(vut_get_system_allocator());
	eval_node(comp_ctx, symbol_info->as.ast_node_id, ctx->scope_id, &text);

	return (struct return_value){
		.ptr = vut_str_move_to_cstr(&text),
		.status = true,
		.free = true,
	};
}
