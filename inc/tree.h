/*
 * tree.h
 *
 *  Created on: Nov 1, 2014
 *      Author: gustavo
 */

#ifndef INC_TREE_H_
#define INC_TREE_H_

#include <stdint.h>

typedef struct node_s {
    char *sample;
    uint16_t frequency;
    struct node_s *left, *right;
} node_t;

typedef struct tree_s {
    node_t* root;
} tree_t;

void tree_create(tree_t **tree);	/* init tree */
node_t* create_node_tree();
void in_order(node_t *);		/* in order print */

#endif /* INC_TREE_H_ */
