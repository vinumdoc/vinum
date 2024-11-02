#include "ast.h"

void args_handle_arg_refs(struct ast *ast, size_t node_id) {
	struct ast_node *node = &ast->nodes[node_id];

	for(size_t i = node->num_childs - 1; i < node->num_childs; i++) {
		struct ast_node *text_node = &ast->nodes[node->childs[i]];

		if (text_node->type != TEXT)
			continue;

		for(size_t j = 0; j < text_node->num_childs; j++) {
			size_t child_id = text_node->childs[j];
			struct ast_node *child = &ast->nodes[child_id];

			if (child->type == ARG_REF_ALL_ARGS) {
				size_t prev_num = text_node->num_childs;
				text_node->num_childs = j;

				struct ast_node new_text_node = {.type = TEXT};

				for(size_t k = j + 1; k < prev_num; k++) {
					ast_node_add_child(&new_text_node, text_node->childs[k]);
				}

				ast_node_add_child(node, child_id);

				size_t new_text_node_id = ast_add_node(ast, new_text_node);
				ast_node_add_child(node, new_text_node_id);
				break;
			}
		}
	}
}
