#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"

struct ast_node ast_node_new(const int type, struct vut_sv text, struct vut_allocator *allocator) {
	struct ast_node ret = { .type = type,
				.text = text,
				.childs = VUT_VEC_INIT(struct ast_node_childs_t, allocator) };

	return ret;
}

struct ast ast_new(struct vut_allocator *allocator) {
	struct ast ret = {
		.nodes = VUT_VEC_INIT(struct ast_nodes_t, allocator),
		.allocator = allocator,
	};

	return ret;
}

ast_node_id_t ast_add_node(struct ast *ast, const struct ast_node node) {
	VUT_VEC_PUT(&ast->nodes, node);
	return ast->nodes.len - 1;
}

ast_node_id_t ast_copy_node(struct ast *ast, ast_node_id_t node_id) {
	struct ast_node no_childs_copy = VUT_VEC_AT(&ast->nodes, node_id);

	no_childs_copy.childs = VUT_VEC_INIT(struct ast_node_childs_t, ast->allocator);

	size_t node_copy_id = ast_add_node(ast, no_childs_copy);
	struct ast_node *node_copy = &VUT_VEC_AT(&ast->nodes, node_copy_id);

	struct ast_node_childs_t childs = VUT_VEC_AT(&ast->nodes, node_id).childs;
	VUT_VEC_RESERVE_EXACT(&node_copy->childs, childs.len);

	for (size_t i = 0; i < childs.len; i++) {
		size_t child_copy_id = ast_copy_node(ast, VUT_VEC_AT(&childs, i));
		ast_add_child(ast, node_copy_id, child_copy_id);
	}

	return node_copy_id;
}

ast_node_id_t ast_get_num_child(const struct ast *ast, ast_node_id_t id) {
	struct ast_node *parent = &VUT_VEC_AT(&ast->nodes, id);
	return parent->childs.len;
}

ast_node_id_t ast_get_nth_child(const struct ast *ast, ast_node_id_t id, ast_node_id_t n) {
	struct ast_node *parent = &VUT_VEC_AT(&ast->nodes, id);
	return VUT_VEC_AT(&parent->childs, n);
}

void ast_add_child(struct ast *ast, ast_node_id_t parent_id, ast_node_id_t child_id) {
	struct ast_node *parent = &VUT_VEC_AT(&ast->nodes, parent_id);
	VUT_VEC_PUT(&parent->childs, child_id);
}

void ast_set_nth_child(struct ast *ast, ast_node_id_t parent_id, ast_node_id_t n,
		       ast_node_id_t child_id) {
	struct ast_node *parent = &VUT_VEC_AT(&ast->nodes, parent_id);
	assert(parent->childs.len > n);
	parent->childs.base[n] = child_id;
}

int ast_get_type(const struct ast *ast, ast_node_id_t id) {
	struct ast_node *node = &VUT_VEC_AT(&ast->nodes, id);
	return node->type;
}

void ast_set_type(struct ast *ast, ast_node_id_t id, int type) {
	struct ast_node *node = &VUT_VEC_AT(&ast->nodes, id);
	node->type = type;
}

struct vut_sv ast_get_text(const struct ast *ast, ast_node_id_t id) {
	struct ast_node *node = &VUT_VEC_AT(&ast->nodes, id);
	return node->text;
}

void ast_set_text(const struct ast *ast, ast_node_id_t id, struct vut_sv text) {
	struct ast_node *node = &VUT_VEC_AT(&ast->nodes, id);
	node->text = text;
}

const char *token_to_str(enum yytokentype token) {
	switch (token) {
	case ASSIGNMENT:
		return "ASSIGNMENT";
	case CALL:
		return "CALL";
	case ARGS:
		return "ARGS";
	case ARG_REF_ALL_ARGS:
		return "ARG_REF_ALL_ARGS";
	case PROGRAM:
		return "PROGRAM";
	case SYMBOL:
		return "SYMBOL";
	case FUNCTION:
		return "FUNCTION";
	case TEXT:
		return "TEXT";
	case LITERAL:
		return "LITERAL";
	default:
		return NULL;
	}
}

static void ast_print_rec(const struct ast *ast, const int id, const int level) {
	const struct ast_node *node = &VUT_VEC_AT(&ast->nodes, id);

	for (int i = 0; i < level; i++) {
		printf("    ");
	}

	if (node->type < 256) {
		printf("\"%c\"", (char)node->type);
	} else {
		printf("%s", token_to_str(node->type));
	}

	if (node->type == TEXT || node->type == SYMBOL || node->type == LITERAL) {
		printf("(" VUT_SV_FMT ")", VUT_SV_ARG(node->text));
	}
	printf("\n");

	for (size_t i = 0; i < node->childs.len; i++) {
		ast_print_rec(ast, VUT_VEC_AT(&node->childs, i), level + 1);
	}
}

void ast_print(const struct ast *ast) {
	ast_print_rec(ast, 0, 0);
}

static void ast_dot_rec(const struct ast *ast, FILE *stream, const size_t id) {
	const struct ast_node *node = &VUT_VEC_AT(&ast->nodes, id);

	fprintf(stream, "\t%zu [label = \"", id);

	if (node->type < 256) {
		fprintf(stream, "\"%c\"", (char)node->type);
	} else {
		fprintf(stream, "%s", token_to_str(node->type));
	}

	if (node->text.base != NULL) {
		fprintf(stream, "(" VUT_SV_FMT ")", VUT_SV_ARG(node->text));
	}

	fprintf(stream, "\"]\n");

	for (size_t i = 0; i < node->childs.len; i++) {
		size_t child_id = VUT_VEC_AT(&node->childs, i);
		fprintf(stream, "\t%zu -> %zu\n", id, child_id);
		ast_dot_rec(ast, stream, child_id);
	}
}

void ast_dot(const struct ast *ast, FILE *stream) {
	fprintf(stream, "digraph {\n");
	fprintf(stream, "\trankdir = LR\n");
	ast_dot_rec(ast, stream, 0);
	fprintf(stream, "}\n");
}
