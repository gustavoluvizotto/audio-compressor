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
	size_t i;							/* loop index */
	node_t* root = NULL;				/* root node for huffman tree */
	uint8_t *sss;						/* number of bits of a given data */
	int16_t *diff = NULL;				/* difference vector */
	char *huffman_code, *diff_code;		/* huffman code and difference code (one's complement) */

	diff = (int16_t*) malloc(num_samples * sizeof(int16_t));
	to_differences(data, diff, num_samples);

	sss = (uint8_t*) malloc(num_samples * sizeof(uint8_t));
	for (i = 0; i < num_samples; i++) {
		sss[i] = 0;
	}

	for (i = 0; i < num_samples; i++) {
		sss[i] = get_sss(diff[i]);
	}
	root = huffman_compress(sss, _frequency, num_samples, MAX_BITS);
	
	for (i = 0; i < num_samples; i++) {
		huffman_code = get_code(root, sss[i]);

		codes[i] = (char*) malloc((strlen(huffman_code) + sss[i] + 1) * sizeof(char));
		memset(codes[i], '\0', (strlen(huffman_code) + sss[i] + 1) * sizeof(char));
		
		/* If the code exceeds the expected number of bits, then use the SSS code with greater frequency */
		if (strlen(huffman_code) + sss[i] > sizeof(diff_code_t) * 8) {
			strcpy(codes[i], get_code(root, 0));
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
	FILE *fp;							/* file pointer */
	char* huffman_code = NULL;			/* huffman bit code */

	fp = fopen(out_file, "rb+");
	if (fp == NULL) {
		TRACE("Error opening file.\n");
		fclose(fp);
		return;
	}
	fseek(fp, 9, SEEK_SET);		/* skipping the common header */

	/* Writing Differences header */
	fwrite(&num_samples, sizeof(uint32_t), 1, fp);
	fwrite(&bits, sizeof(uint8_t), 1, fp);					/* stuff bits */

	for (k = 0; k <= MAX_BITS; k++) {
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
	/* difference header has been written */

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
		fseek(fp, 9 + sizeof(uint32_t), SEEK_SET);		/* jump (number of samples too) and stop on the huffman header (number of bits field) */
		fwrite(&bits, sizeof(uint8_t), 1, fp); 			/* write the number of bits of the last c byte without stuffing 0 */
	}

	/* free memory and close file */
	free(huffman_code);
	fclose(fp);

	return;
}

int16_t binary_to_byte(char *code) {
	size_t i;
	int16_t byte = 0;

	for (i = 0; i < strlen(code); i++) {
		byte <<= 1;
		if(code[i] == '1') {
			byte += 1;
		}
	}

	return byte;
}

uint32_t differences_decompress(FILE *fp, uint16_t *_frequency, uint32_t num_samples, char** codes) {
	uint32_t count;						/* counter of data and frequency */
	size_t reading;						/* fread return control */
	int i;								/* loop control */
	size_t k, j = 0;					/* loop control */
	uint8_t data;						/* data */
	char target[MAX_HUFF_CODE];			/* target code to search in the table */
	uint8_t read_byte;					/* bytes read from file */
	tree_t *huffman_tree = NULL;		/* one huffman tree */
	uint8_t bits = 0;					/* number of bits of the last byte */
	char *sample = NULL;				/* sample in char */
	uint8_t sss = 0;					/* number of bits of a value of a sample */
	uint8_t xgh = FALSE;				/* xgh stands for eXtreme Go Horse */
	char *buffer;						/* string buffer */

	huffman_tree = (tree_t*) malloc(sizeof(tree_t));
	tree_create(&huffman_tree);

	/* reading the differences header */
	reading = fread(&bits, sizeof(uint8_t), 1,fp);
	if (reading != 1) {
		TRACE("[ERROR] Fail to read file -- number of bits of the last character\n");
		return 0;
	}

	reading = fread(&count, sizeof(uint32_t), 1, fp);
	if (reading != 1) {
		TRACE("[ERROR] Fail to read file -- frequency counter\n");
		return 0;
	}

	for (k = 0; k < count; k++) {
		data = 0;
		fread(&data, sizeof(uint8_t), 1, fp);
		fread(&_frequency[data], sizeof(uint16_t), 1, fp);
	}
	/* differences header has been read */

	huffman_tree->root = huffman(_frequency, MAX_BITS);

	/* decode every byte read from data sector in input file */
	memset(target, '\0', MAX_HUFF_CODE * sizeof(char));
	while(j < num_samples) {
		/* read file until find a huffman code */
		fread(&read_byte, sizeof(uint8_t), 1, fp);
		buffer = byte_to_binary(read_byte);
		for (i = 0; i < 8 && j < num_samples; i++) {
			if (!xgh) {
				strncat(target, &buffer[i], sizeof(char));
				sample = get_leaf(huffman_tree->root, target);
				if (sample != NULL) {	/* found the huffman code? */
					/*codes[j] = (char*) malloc((strlen(sample) - 1) * sizeof(char));*/
					memset(codes[j], '\0', (strlen(sample) - 1) * sizeof(char));
					strncpy(codes[j], (sample+1), (strlen(sample) - 2) * sizeof(char));
					/*TRACE("huff: %s\n", target);*/
					memset(target, '\0', MAX_HUFF_CODE * sizeof(char));
				}
			}
			/* found a sample */
			if (sample != NULL) {
				if (!xgh) {	/* a new code value code? */
					sss = (uint8_t) string_to_int(codes[j]);
					memset(codes[j], '\0', (strlen(sample) - 1) * sizeof(char));
				}
				for (k = xgh ? k : 0; k < sss; k++) {	/* keep interpreting the rest of bits to get the value of bits of difference compression */
					if (xgh) {
						i = -1;
						xgh = FALSE;
					}
					i++;
					if (i > 7) {
						xgh = TRUE;
						break;
					}
					strncat(codes[j], &buffer[i], sizeof(char));
					if (i==7) {
						xgh = TRUE;
						k++;
						break;
					}
				}
				if (k == sss) {
					xgh = FALSE;
					j++;
				}
			}
		}
	}
	free(huffman_tree);

	return count;
}

void perform_one_complement(char *code) {
	size_t i;

	for (i = 0; i < strlen(code); i++) {
		if (code[i] == '0') {
			code[i] = '1';
		} else {
			code[i] = '0';
		}
	}
	return;
}
