/*
 * main.c
 *
 *  Created on: Oct 28, 2014
 *      Author: gustavo
 */

#include <stdio.h>
#include "inc/wav.h"

#undef	ERR_NO
#define ERR_NO 0

#undef	ERR_FAIL
#define	ERR_FAIL 1

typedef int8_t result_t;

/* global */
riff_chunk_t riff_chunk;
fmt_chunk_t fmt_chunk;
data_chunk_t data_chunk;

result_t read_sound(char *in_file, FILE *fp) {
	result_t result = -ERR_NO;
	size_t reading;

	fp = fopen(in_file, "rb");
	if (fp == NULL) {
		goto FAIL_OPEN_FILE;
	}

	/* reads header */
	reading = fread(&riff_chunk, sizeof(riff_chunk), 1, fp);
	if (reading != 1) {
		goto FAIL_READ_FILE;
	}
	reading = fread(&fmt_chunk, sizeof(fmt_chunk), 1, fp);
	if (reading != 1) {
		goto FAIL_READ_FILE;
	}
	reading = fread(&data_chunk, sizeof(data_chunk), 1, fp);
	if (reading != 1) {
		goto FAIL_READ_FILE;
	}

	result = -ERR_NO;
	return result;

FAIL_OPEN_FILE:
FAIL_READ_FILE:
	result = -ERR_FAIL;
	return result;
}

int main (int argc, char* argv[]) {

	return 0;
}
