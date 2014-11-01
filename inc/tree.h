/*
 * tree.h
 *
 *  Created on: Nov 1, 2014
 *      Author: gustavo
 */

#ifndef INC_TREE_H_
#define INC_TREE_H_

#include "common.h"

typedef struct node_s {
    uint8_t sample;
    uint32_t frequency;
    struct node_s *left, *right;
} node_t;

typedef struct {
    node_t* root;
} tree_t;

void tree(tree_t *tree);	/* init tree */
node_t* insert_node(uint32_t frequency, uint8_t sample);
void in_order(node_t *);	/* in order print */

#endif /* INC_TREE_H_ */
