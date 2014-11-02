/*
 * huffman.h
 *
 *  Created on: Nov 1, 2014
 *      Author: gustavo
 */

#ifndef INC_HUFFMAN_H_
#define INC_HUFFMAN_H_

#include "common.h"

void comprimir_huffman(uint8_t *data);
tree_t* huffman_tree(uint32_t frequency);

#endif /* INC_HUFFMAN_H_ */
