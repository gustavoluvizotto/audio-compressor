/*
 * huffman.c
 *
 *  Created on: Nov 1, 2014
 *      Author: gustavo
 */

#include "../inc/huffman.h"
#include "../inc/common.h"

void comprimir(uint8_t *data) {
	size_t size_data = sizeof(data);		/* total number of samples */
	size_t i;		/* loop indexes */
	char **samples;	/* store the samples in strings. Usefull to count the number of bits */
	uint32_t *frequency;	/* frequency of given data[i] */

	samples = (char**) malloc (size_data * sizeof(char*));
	frequency = (uint32_t*) malloc (size_data * sizeof(uint32_t));
	for (i = 0; i < size_data; i++) {
		samples[i] = (char*) malloc (MAXBITS * sizeof(char) + 1);	/* 8 bits plus \0 */
		frequency[i] = 0;
	}
	/* begin the frequency count */
	for (i = 0; i < size_data; i++) {
		itoa(data[i], samples[i]);
	}
}
