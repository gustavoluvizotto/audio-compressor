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
node_t* comprimir_huffman(uint8_t *data, uint32_t number_of_samples);
result_t write_huffman(node_t *root, uint8_t *data, uint32_t *frequency, char *out_file);
char* get_code(node_t* p, uint8_t data);
void huffman_code(node_t* p, char* data, char* code);

#endif /* INC_HUFFMAN_H_ */
