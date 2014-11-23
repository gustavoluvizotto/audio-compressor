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
		diff[i] =  data[i] - data[i-1];
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


node_t* diff_compress(uint8_t *data, uint16_t *_frequency, char **codes, uint32_t num_samples) {
	size_t i;
	node_t* root = NULL;
	uint8_t *sss;
	int16_t *diff = NULL;
	char *huffman_code, *diff_code;

	diff = (int16_t*) malloc(num_samples * sizeof(int16_t));
	to_differences(data, diff, num_samples);

	sss = (uint8_t*) malloc(num_samples * sizeof(uint8_t));
	for (i = 0; i < num_samples; i++) {
		sss[i] = 0;
	}

	for (i = 0; i < num_samples; i++) {
		sss[i] = get_sss(diff[i]);
	}
	root = huffman_compress(sss, _frequency, num_samples, MAX_BITS+1);
	
	for (i = 0; i < num_samples; i++) {
		huffman_code = get_code(root, sss[i])	;

		codes[i] = (char*) malloc((strlen(huffman_code) + sss[i] + 1) * sizeof(char));
		memset(codes[i], '\0', (strlen(huffman_code) + sss[i] + 1) * sizeof(char));
		
		/* If the code exceeds the expected number of bits, then use the SSS code with greater frequency */
		if (strlen(huffman_code) + sss[i] > sizeof(diff_code_t) * 8) {
			strcpy(codes[i], get_code(root, 0/*max_frequency(_frequency, MAX_SAMPLE)*/));
		} else {
		   	strncpy(codes[i], huffman_code, strlen(huffman_code) * sizeof(char));
		   	
		   	diff_code = (char*) malloc((sss[i] + 1) * sizeof(char));
		   	memset(diff_code, '\0', (sss[i] + 1) * sizeof(char));

			get_value_code(diff_code, diff[i]);
			
			strncat(codes[i], diff_code, strlen(diff_code) * sizeof(char));
		}
			
		free(huffman_code);
	}

	return root;
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

void write_differences(uint16_t *_frequency, char **codes, char *out_file,  uint32_t num_samples) {
	unsigned char c;					/* Used to store each byte to be written on file */
	size_t i, j;						/* Indexes */
	uint8_t k;							/* Index to write in the file */
	uint8_t bits = 0;					/* Used to control the number of bits in byte c */
	uint32_t count = 0;					/* Count number of frequencies differents from zero */
	size_t size;
	FILE *fp;
	uint32_t count_stored_codes = 0;
	char* huffman_code = NULL;

	/* getting the size of the file before writting the huffman header and data. It is
	 * useful to store the number of stuffing bits in the header that has gotten in the
	 * final of write proccess. */
	fp = fopen(out_file, "rb");
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	fclose(fp);

	fp = fopen(out_file, "rb+");
	if (fp == NULL) {
		TRACE("Error opening file.\n");
		fclose(fp);
		return;
	}
	fseek(fp, 0, SEEK_END);

	/* Writing Differences header */
	fwrite(&bits, sizeof(uint8_t), 1, fp);					/* stuff bits */
	fwrite(&count_stored_codes, sizeof(uint32_t), 1, fp);	/* number of written bytes */

	for (k = 0; k < MAX_BITS; k++) {
		if (_frequency[k] > 0) {
			count++;
		}
	}
	fwrite(&count, sizeof(uint32_t), 1, fp);				/* counter of pair (huffman_code, SSS) */

	/* pair (huffman_code, SSS) */
	for (k = 0; k <= MAX_BITS; k++) {
		if (_frequency[k] > 0) {
			fwrite(&k, sizeof(uint8_t), 1, fp);
			fwrite(&_frequency[k], sizeof(uint16_t), 1, fp);
		}
	}

	/* For each sample, write the Huffman code and sample itself */
	c = 0;
	for(i = 0; i < num_samples; i++) {
		/* Write the huffman code in char c. Shift every character of code in bits of c */
		for(j = 0; j < strlen(codes[i]); j++) {
			c <<= 1;
			if(codes[i][j] == '1')
				c += 1;
			bits++;
			if(bits == 8) {		/* if complete an entire byte, write in the output file */
				fwrite(&c, sizeof(unsigned char), 1, fp);
				count_stored_codes++;
				c = 0;
				bits = 0;
			}
		}
	}

	/* If bits < 8, then complete with zeros the rest of char c */
	if(bits != 0) {
		i = bits;
		while(i < 8) {
			c <<= 1;
			i++;
		}
		fwrite(&c, sizeof(unsigned char), 1, fp); 		/* write the last c byte */
		count_stored_codes++;
		fseek(fp, size, SEEK_SET);	 					/* jump and stop on the huffman header (number of bits field) */
		fwrite(&bits, sizeof(uint8_t), 1, fp); 			/* write the number of bits of the last c byte without stuffing 0 */
		fwrite(&count_stored_codes, sizeof(uint32_t), 1, fp);
	}

	/* free memory and close file */
	free(huffman_code);
	fclose(fp);

	return;
}

uint8_t binary_to_byte(char *code) {
	size_t i;
	uint8_t byte = 0;

	for (i = 0; i < strlen(code); i++) {
		byte <<= 1;
		if(code[i] == '1') {
			byte += 1;
		}
	}

	return byte;
}

