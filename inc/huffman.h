/*
 * huffman.h
 *
 *  Created on: Nov 1, 2014
 *      Author: gustavo
 */

#ifndef INC_HUFFMAN_H_
#define INC_HUFFMAN_H_

#include "queue.h"

node_t* huffman(uint32_t *frequency);
void comprimir_huffman(uint8_t *data, char **samples);

#endif /* INC_HUFFMAN_H_ */
