/*
 * huffman.h
 *
 *  Created on: Nov 1, 2014
 *      Author: gustavo
 */

#ifndef INC_HUFFMAN_H_
#define INC_HUFFMAN_H_

#include "queue.h"

#define MAX_HUFF_CODE 128	/* max lenght of a huffman code (per data) */

node_t* huffman(uint16_t *frequency);
node_t* comprimir_huffman(uint8_t *data, uint32_t number_of_samples);
result_t write_huffman(node_t *root, uint8_t *data, char *out_file,  uint32_t number_of_samples);
char* get_code(node_t* root, uint8_t data);
void huffman_code(node_t* root, char* data, char* code);

uint16_t *frequency;					/* frequency of given sample */

#endif /* INC_HUFFMAN_H_ */
