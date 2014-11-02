/*
 * tree.c
 *
 *  Created on: Nov 1, 2014
 *      Author: gustavo
 */

#include "../inc/tree.h"

void tree_create(tree_t *tree) {
	tree = (tree_t *) malloc (sizeof(tree_t));
	tree->root = NULL;
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

    printf("%X: %d\n", node->sample, node->frequency);

    in_order(node->right);
	return;
}
