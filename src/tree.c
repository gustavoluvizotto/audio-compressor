/*
 * tree.c
 *
 *  Created on: Nov 1, 2014
 *      Author: gustavo
 */

#include "../inc/tree.h"

void tree_create(tree_t *tree) {
	tree->root = NULL;
	return;
}

void create_node_tree(node_t* new_node) {

    new_node->frequency = 0;
    new_node->sample = NULL;
    new_node->right = NULL;
    new_node->left = NULL;

	return;
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
