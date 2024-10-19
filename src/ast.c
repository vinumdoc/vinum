#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "dry_bison.h"

#include "ast.h"

struct ast_node ast_node_new(const int type, char *text) {
	struct ast_node ret = {
		.type = type,
		.text = text,
	};

	return ret;
}

void ast_node_add_child(struct ast_node *dst, const size_t child) {
	assert(dst->num_childs < MAX_AST_NODE_CHILDS);

	dst->childs[dst->num_childs] = child;
	dst->num_childs++;
}

struct ast ast_new() {
	struct ast ret = {};

	return ret;
}

size_t ast_add_node(struct ast *ast, const struct ast_node node) {
	assert(ast->num_nodes < MAX_AST_NODES);

	ast->nodes[ast->num_nodes] = node;
	ast->num_nodes++;

	return ast->num_nodes - 1;
}

static const char* token_to_str(enum yytokentype token) {
	switch (token) {
		case ASSIGNMENT: return "ASSIGNMENT";
		case CALL: return "CALL";
		case ARGS: return "ARGS";
		case PROGRAM: return "PROGRAM";
		case SYMBOL: return "SYMBOL";
		case TEXT: return "TEXT";
		case WORD: return "WORD";
		default: return NULL;
	}
}

static void ast_print_rec(const struct ast *ast, const int id, const int level) {
	const struct ast_node *node = &ast->nodes[id];

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

	for(size_t i = 0; i < node->num_childs; i++) {
		ast_print_rec(ast, node->childs[i], level + 1);
	}
}

void ast_print(const struct ast *ast) {
	ast_print_rec(ast, 0, 0);
}
