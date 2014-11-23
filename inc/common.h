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

#define MAX_LENGHT_SAMPLE 6

#define	MAXSIZE MAX_SAMPLE*MAX_LENGHT_SAMPLE	/* If every sample apear in the .wav file. 5 characters will be add per sample. For Huffman codification */

#define MAX_BITS 8			/* max number of bits of a data (data of 8 bits?) */

typedef int8_t result_t;

#define PRINT_STRUCT(p) print_struct((p), sizeof(*(p)))

void print_struct(void const *vp, size_t n);

void reverse(char s[]);
char *byte_to_binary(uint8_t x);
int string_to_int(char *a);

/*
 * Combinations of codification modes and the binary representative
 *    H 0001
 *    D 0010
 *    M 0011
 *   HD 0100
 *  HDM 0101
 *   HM 0110
 *  HMD 0111
 *   DH 1000
 *  DHM 1001
 *   DM 1010
 *  DMH 1011
 *   MD 1100
 *  MDH 1101
 *   MH 1110
 *  MHD 1111
 *
 *  H = Huffman
 *  D = Differences
 *  M = MDCT
 */
result_t write_header_to_file(char *out_file, riff_chunk_t riff_chunk, fmt_chunk_t fmt_chunk, unsigned char c);

#endif /* INC_COMMON_H_ */
