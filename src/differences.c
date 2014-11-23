/*
 * differences.c
 *
 *  Created on: Nov 21, 2014
 *      Author: ivan
 */

#include "../inc/differences.h"
#include <stdlib.h>

void to_differences(uint8_t* data, int16_t* diff, uint32_t size) {
	uint32_t i;
	
	diff[0] = data[0];
	
	for (i = 1; i < size; i++) {
		diff[i] = data[i-1] - data[i];
	}
}

void from_differences(uint8_t* data, int16_t* diff, uint32_t size) {
	uint32_t i;
	
	data[0] = diff[0];
	
	for (i = 1; i < size; i++) {
		data[i] = data[i-1] + diff[i];
	}
}

uint8_t get_sss(int16_t value) {
	int8_t i;
	
	if (value < 0) {
		value *= -1;
	}

	/* counting how many left zeros we have in int16_t */
	for (i = sizeof(value) * 8 - 1; i >= 0; i--) {
		if ((value >> i) & 0x01) {		/* stop when found the first 1 */
		   break;
		}
	}
	
	return i+1;
}

void get_value_code(char* code, int16_t value) {
	size_t i;
	uint8_t sss, negative;

	if (value == 0) {
		code[0] = '\0';
	}

	sss = get_sss(value);
	negative = value < 0;  /* 0 means positive (or 0, already handled above), 1 means negative. */

	if (negative) {
		value *= -1;
	}

	for (i = 0; i < sss; i++) {
		code[i] = ((value >> (sss-i-1)) & 0x01) ^ negative ? '1' : '0';
	}
}


char** diff_compress(uint8_t *data, uint16_t *frequency, uint32_t num_samples) {
	size_t i;
	tree_t huffman_tree;
	uint8_t sss[num_samples];
	int16_t *diff = NULL;
	char *huffman_code, *diff_code;
	char **codes = NULL;

	diff = (int16_t*) malloc(num_samples * sizeof(int16_t));
	to_differences(data, diff, num_samples);

	codes = (char**) malloc(num_samples * sizeof(char*));

	for (i = 0; i < num_samples; i++) {
		sss[i] = get_sss(diff[i]);
	}

	huffman_tree.root = huffman_compress(sss, frequency, num_samples);
	
	for (i = 0; i < num_samples; i++) {
		huffman_code = get_code(huffman_tree.root, sss[i])	;

		codes[i] = (char*) malloc((strlen(huffman_code) + sss[i] + 1) * sizeof(char));
		memset(codes[i], '\0', (strlen(huffman_code) + sss[i] + 1) * sizeof(char));
		
		/* If the code exceeds the expected number of bits, then use the SSS code with greater frequency */
		if (strlen(huffman_code) + sss[i] > sizeof(diff_code_t) * 8 &&  i > 0) {
			strcpy(codes[i], get_code(huffman_tree.root, max_frequency(frequency, MAX_SAMPLE)));
		} else {
		   	strncpy(codes[i], huffman_code, strlen(huffman_code) * sizeof(char));
		   	
		   	diff_code = (char*) malloc((sss[i] + 1) * sizeof(char));
		   	memset(diff_code, '\0', (sss[i] + 1) * sizeof(char));

			get_value_code(diff_code, diff[i]);
			
			strncat(codes[i], diff_code, strlen(diff_code) * sizeof(char));
		}
			
		free(huffman_code);
	}
	
	return codes;
}

uint8_t max_frequency(uint16_t* array, uint8_t lenght) {
	uint8_t i, max = 0;
	
	for(i = 0; i < lenght; i++) {
		if (max < array[i]) {
			max = array[i];
		}
	}
	
	return max;
}
