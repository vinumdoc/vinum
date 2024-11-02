#ifndef __AST_H__
#define __AST_H__

#include <stddef.h>

#include "dry_bison.h"

#define MAX_AST_NODE_CHILDS 50

struct ast_node {
	int type;
	size_t childs[MAX_AST_NODE_CHILDS];
	size_t num_childs;

	char *text;
};

struct ast_node ast_node_new(const int type, char *value);
#define ast_node_new_nvl(type) (ast_node_new((type), NULL))

void ast_node_add_child(struct ast_node *dst, const size_t child);

#define MAX_AST_NODES 1024

struct ast {
	struct ast_node nodes[MAX_AST_NODES];
	size_t num_nodes;
};

struct ast ast_new();
size_t ast_add_node(struct ast *ast, const struct ast_node node);
void ast_print(const struct ast *ast);

#endif // __AST_H__
