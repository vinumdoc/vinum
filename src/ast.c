#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"

struct ast_node ast_node_new(const int type, char *text) {
	struct ast_node ret = {
		.type = type,
		.text = text,
	};

	return ret;
}

void ast_node_add_child(struct ast_node *dst, const size_t child) {
	VEC_PUT(&dst->childs, child);
}

struct ast ast_new() {
	struct ast ret = {};

	return ret;
}

size_t ast_add_node(struct ast *ast, const struct ast_node node) {
	VEC_PUT(&ast->nodes, node);
	return ast->nodes.len - 1;
}

size_t ast_copy_node(struct ast *ast, size_t node_id) {
	struct ast_node *node = &VEC_AT(&ast->nodes, node_id);

	struct ast_node no_childs_copy = *node;
	no_childs_copy.childs = (struct ast_node_childs_t){0};

	size_t node_copy_id = ast_add_node(ast, no_childs_copy);
	struct ast_node *node_copy = &VEC_AT(&ast->nodes, node_copy_id);

	VEC_RESERVE(&node_copy->childs, node->childs.len);

	for(size_t i = 0; i < node->childs.len; i++) {
		size_t child_copy_id = ast_copy_node(ast, VEC_AT(&node->childs, i));
		ast_node_add_child(node_copy, child_copy_id);
	}

	return node_copy_id;
}

static const char* token_to_str(enum yytokentype token) {
	switch (token) {
		case ASSIGNMENT: return "ASSIGNMENT";
		case CALL: return "CALL";
		case ARGS: return "ARGS";
		case ARG_REF_ALL_ARGS: return "ARG_REF_ALL_ARGS";
		case PROGRAM: return "PROGRAM";
		case SYMBOL: return "SYMBOL";
		case TEXT: return "TEXT";
		case WORD: return "WORD";
		default: return NULL;
	}
}

static void ast_print_rec(const struct ast *ast, const int id, const int level) {
	const struct ast_node *node = &VEC_AT(&ast->nodes, id);

	for(int i = 0; i < level; i++) {
		printf("    ");
	}

	if (node->type < 256) {
		printf("\"%c\"", (char)node->type);
	} else {
		printf("%s", token_to_str(node->type));
	}

	if (node->type == WORD || node->type == SYMBOL) {
		printf("(%s)", node->text);
	}
	printf("\n");

	for(size_t i = 0; i < node->childs.len; i++) {
		ast_print_rec(ast, VEC_AT(&node->childs, i), level + 1);
	}
}

void ast_print(const struct ast *ast) {
	ast_print_rec(ast, 0, 0);
}

static void ast_dot_rec(const struct ast *ast, FILE *stream, const size_t id) {
	const struct ast_node *node = &ast->nodes[id];

	fprintf(stream, "\t%zu [label = \"", id);

	if (node->type < 256) {
		fprintf(stream, "\"%c\"", (char)node->type);
	} else {
		fprintf(stream, "%s", token_to_str(node->type));
	}

	if (node->text != NULL) {
		fprintf(stream, "(%s)", node->text);
	}

	fprintf(stream, "\"]\n");

	for (size_t i = 0; i < node->num_childs; i++) {
		size_t child_id = node->childs[i];
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
