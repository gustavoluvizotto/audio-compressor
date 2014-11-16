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

/*
* Structure to store the huffman codes table for decompression.
*/
typedef struct{
	char* code;
	uint32_t index;
} table_t;

node_t* huffman(uint16_t *frequency);
node_t* huffman_compress(uint8_t *data, uint32_t num_samples);
result_t write_huffman(node_t *root, uint8_t *data, char *out_file,  uint32_t num_samples);
char* get_code(node_t* root, uint8_t data);
void huffman_code(node_t* root, char* data, char* code);
int count_ocurrencies(char* str, char c);
result_t huffman_decompress(FILE *fp, table_t* table, uint32_t num_samples, char** codes);
void generate_table(node_t *root, table_t *table, uint16_t *frequency);
uint8_t search_code(table_t *table, char *code);

uint16_t *frequency;					/* frequency of given sample */

#endif /* INC_HUFFMAN_H_ */
