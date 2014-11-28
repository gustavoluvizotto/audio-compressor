/*
 * tree.c
 *
 *  Created on: Nov 1, 2014
 */

#include "../inc/tree.h"

void tree_create(tree_t **tree) {
	(*tree) = (tree_t *) malloc (sizeof(tree_t));
	(*tree)->root = NULL;
	return;
}

node_t* create_node_tree() {
	node_t* new_node = (node_t *) malloc(sizeof(node_t));

    new_node->frequency = 0;
    new_node->sample = NULL;
    new_node->right = NULL;
    new_node->left = NULL;

	return new_node;
}

void in_order(node_t *node) {
    if(node == NULL) {
    	return;
    }
    in_order(node->left);

    printf("%s: %d\n", node->sample, node->frequency);

    in_order(node->right);
	return;
}

char* get_leaf(node_t *node, char *code) {
	if (node->left == NULL || node->right == NULL) {
		return node->sample;
	}
	if (code[0] == '\0') {
		return NULL;
	}
	if (code[0] == '1') {
		return get_leaf(node->right, code+1);
	} else {
		return get_leaf(node->left, code+1);
	}
}
