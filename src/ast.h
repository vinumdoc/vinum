#ifndef __AST_H__
#define __AST_H__

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "vec.h"

#include "dry_bison.h"

typedef uint32_t ast_node_id_t;

struct ast_node_childs_t VEC_DEF(ast_node_id_t);

struct ast_node {
	int type;
	char *text;

	struct ast_node_childs_t childs;
};

struct ast_node ast_node_new(const int type, char *value);
#define ast_node_new_nvl(type) (ast_node_new((type), NULL))

void ast_node_add_child(struct ast_node *dst, const ast_node_id_t child);

struct ast_nodes_t VEC_DEF(struct ast_node);

struct ast {
	struct ast_nodes_t nodes;
};

struct ast ast_new();

const char* token_to_str(enum yytokentype token);
ast_node_id_t ast_add_node(struct ast *ast, const struct ast_node node);
ast_node_id_t ast_copy_node(struct ast *ast, ast_node_id_t node_id);

void ast_print(const struct ast *ast);
void ast_dot(const struct ast *ast, FILE *stream);

#endif // __AST_H__
