/*
 * main.c
 *
 *  Created on: Oct 28, 2014
 *      Author: gustavo
 */

#include "inc/wav.h"
#include "inc/common.h"

/*
#undef TRACE(fmt, ...)
#define TRACE(fmt, ...)
*/
/* global */
riff_chunk_t riff_chunk;
fmt_chunk_t fmt_chunk;
data_chunk_t data_chunk;

result_t read_sound(char *in_file, FILE *fp) {
	result_t result = -ERR_NO;
	size_t reading;
	uint32_t size;
	size_t i;
	size_t header_size;

	fp = fopen(in_file, "rb");
	if (fp == NULL) {
		TRACE("[ERROR] Fail to open file %s\n", in_file);
		goto FAIL_OPEN_FILE;
	}

	/* getting the size of the file */
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	data = (uint8_t *) calloc (size, sizeof(uint8_t));

	/* reads header */
	reading = fread(&riff_chunk, sizeof(riff_chunk), 1, fp);
	if (reading != 1) {
		TRACE("[ERROR] Fail to read file -- riff chunk sector\n");
		goto FAIL_READ_FILE;
	}
	reading = fread(&fmt_chunk, sizeof(fmt_chunk), 1, fp);
	if (reading != 1) {
		TRACE("[ERROR] Fail to read file -- fmt chunk sector\n");
		goto FAIL_READ_FILE;
	}
	reading = fread(&data_chunk, sizeof(data_chunk), 1, fp);
	if (reading != 1) {
		TRACE("[ERROR] Fail to read file -- data chunk sector\n");
		goto FAIL_READ_FILE;
	}

	header_size = sizeof(riff_chunk) + sizeof(fmt_chunk) + sizeof(data_chunk);

	for (i = 0; i < size-header_size; i++) {
		reading = fread(&data[i], sizeof(data[i]), 1, fp);
		if (reading != 1) {
			TRACE("[ERROR] Fail to read file -- sample sector\n");
			goto FAIL_READ_FILE;
		}
	}

	result = -ERR_NO;
	fclose(fp);
	TRACE("Read file complete!\n");
	return result;

FAIL_OPEN_FILE:
FAIL_READ_FILE:
	result = -ERR_FAIL;
	return result;
}

void print_headers() {

	TRACE("riff: %lu \n", sizeof(riff_chunk));
	TRACE("fmt: %lu \n", sizeof(fmt_chunk));
	TRACE("data: %lu \n", sizeof(data_chunk));
}

int main (int argc, char* argv[]) {
	FILE *fp = NULL;
	char *in_file = "resources/pig.wav";
	result_t result;

	result = read_sound(in_file, fp);

	if (fmt_chunk.audio_format != 1) {
		printf("Compressed file!\n");
		result = -ERR_FAIL;
	}

    printf("Press <ENTER> to exit... ");
    getchar();

    return result;
}
