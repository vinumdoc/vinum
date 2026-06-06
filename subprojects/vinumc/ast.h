#ifndef __AST_H__
#define __AST_H__

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <vutils/allocator.h>
#include <vutils/sv.h>
#include <vutils/vec.h>

#include "dry_bison.h"

typedef uint32_t ast_node_id_t;

struct ast_node_childs_t VUT_VEC_DEF(ast_node_id_t);

struct ast_node {
	int type;
	struct vut_sv text;

	struct ast_node_childs_t childs;
};

struct ast_nodes_t VUT_VEC_DEF(struct ast_node);

struct ast {
	struct ast_nodes_t nodes;

	struct vut_allocator allocator;
	struct vut_allocator child_arena;
};

struct ast ast_new(struct vut_allocator allocator);
void ast_free(struct ast *ast);

ast_node_id_t ast_node_new(struct ast *ast, const int type, struct vut_sv text);
#define ast_node_new_nvl(ast, type) (ast_node_new((ast), (type), (struct vut_sv){ 0 }))

const char *token_to_str(enum yytokentype token);
ast_node_id_t ast_copy_node(struct ast *ast, ast_node_id_t node_id);

ast_node_id_t ast_get_num_child(const struct ast *ast, ast_node_id_t id);
ast_node_id_t ast_get_nth_child(const struct ast *ast, ast_node_id_t id, ast_node_id_t n);
void ast_add_child(struct ast *ast, ast_node_id_t father_id, ast_node_id_t child_id);
void ast_set_nth_child(struct ast *ast, ast_node_id_t father_id, ast_node_id_t n,
		       ast_node_id_t child_id);

int ast_get_type(const struct ast *ast, ast_node_id_t id);
void ast_set_type(struct ast *ast, ast_node_id_t id, int type);

struct vut_sv ast_get_text(const struct ast *ast, ast_node_id_t id);
void ast_set_text(const struct ast *ast, ast_node_id_t id, struct vut_sv text);

void ast_print(const struct ast *ast);
void ast_dot(const struct ast *ast, FILE *stream);

#endif // __AST_H__
