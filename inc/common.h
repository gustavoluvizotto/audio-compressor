/*
 * common.h
 *
 *  Created on: Oct 31, 2014
 *      Author: gustavo
 */

#ifndef INC_COMMON_H_
#define INC_COMMON_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wav.h"

#undef TRACE
#define TRACE(fmt, ...) printf(fmt, ## __VA_ARGS__)

#undef	ERR_NO
#define ERR_NO 0

#undef	ERR_FAIL
#define	ERR_FAIL 1

#undef	TRUE
#define	TRUE 1

#undef	FALSE
#define	FALSE 0

#define MAX_SAMPLE 255		/* may we need to change 510? (255 (negative numbers) + 1 (zero) + 255 (positive numbers)) */
#define	MAXSIZE 1275		/* 255*5. If every sample apear in the .wav file. 5 characters will be add per sample */

#define MAX_HUFF_CODE 128	/* max lenght of a huffman code (per data) */

#define MAX_BITS 8			/* max number of bits of a data (data of 8 bits?) */

typedef int8_t result_t;

#define PRINT_STRUCT(p) print_struct((p), sizeof(*(p)))

void print_struct(void const *vp, size_t n);

typedef struct {
	unsigned bit0:1;
	unsigned bit1:1;
	unsigned bit2:1;
	unsigned bit3:1;
	unsigned bit4:1;
	unsigned bit5:1;
	unsigned bit6:1;
	unsigned bit7:1;
} byte_t;

void reverse(char s[]);
void itoa(int n, char s[]);
char *byte_to_binary(uint8_t x);
result_t write_header_to_file(char *out_file, fmt_chunk_t fmt_chunk);

#endif /* INC_COMMON_H_ */
