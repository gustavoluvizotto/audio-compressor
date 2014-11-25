/*
 * differences.h
 *
 *  Created on: Nov 21, 2014
 *      Author: ivan
 */

#ifndef INC_DIFFERENCES_H_
#define INC_DIFFERENCES_H_

#include "huffman.h"


typedef uint16_t diff_code_t; /* Type to store the difference codes generated */

void to_differences(uint8_t* data, int16_t* diff, uint32_t size);
void from_differences(uint8_t* data, int16_t* diff, uint32_t size);
uint8_t get_sss(int16_t value);
void get_value_code(char* code, int16_t value);
node_t* diff_compress(uint8_t *data, uint16_t *frequency, char **codes, uint32_t num_samples);
uint8_t max_frequency(uint16_t* array, uint8_t lenght);
void write_differences(uint16_t *_frequency, char **codes, char *out_file,  uint32_t num_samples);
int16_t binary_to_byte(char *code);
result_t differences_decompress(FILE *fp, uint16_t *_frequency, uint32_t num_samples, char** codes);
void perform_one_complement(char *code);

#endif /* INC_DIFFERENCES_H_ */
