/*
 * main.c
 *
 *  Created on: Oct 28, 2014
 *      Author: gustavo
 */

#include "inc/wav.h"
#include "inc/common.h"
#include "inc/huffman.h"

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

void system_pause() {
    printf("Press <ENTER> to exit... ");
    getchar();
}

uint8_t search_for_equal_element(int *list, int size, int key) {
	  uint8_t found = FALSE;
	  int i;

	  for ( i = 0; i < size; i++ ) {
	    if ( key == list[i] )
	      break;
	  }
	  if ( i < size ) {
	    found = TRUE;
	  }

	  return found;
}

result_t compress() {
	result_t result;			/* return results */
	size_t i, j;				/* index */
	int8_t ans;					/* answer of the user about compress modes */
	int8_t modes[3];			/* modes of compress */
	uint8_t is_equal = FALSE;	/* check if a compress has been choosed*/
	uint8_t **data_adjusted;	/* the input data per channel adjusted for every compression */
	size_t data_size_channel;	/* lenght of *data vector per channel */

	printf("Choose the compression you want\n");
	printf("0. Exit\n1. Huffman\n2. Differences\n3. MDCT\n");

	for (i = 0; i < 3; i++) {
		modes[i] = 0;
	}

	for (i = 0; ans != 0 && i < 3;) {
		scanf("%d", ans);
		is_equal = search_for_equal_element(modes, 3, ans);
		if (is_equal == TRUE) {
			printf("Type a different compression!\n");
		} else {
			modes[i] = ans;
			i++;
		}
	}

	data_adjusted = (uint8_t**) malloc (fmt_chunk.num_channels * sizeof(uint8_t*));
	data_size_channel = sizeof(data)/fmt_chunk.num_channels;
	for (i = 0; i < fmt_chunk.num_channels; i++) {
		data_adjusted[i] = (uint8_t*) malloc (data_size_channel * sizeof(uint8_t));
		for (j = 0; j < data_size_channel; j++) {
			data_adjusted[i][j] = data[i*data_size_channel + j];
		}
	}

	for (i = 0; i < 3; i++){
		switch (modes[i]) {
		case 1:
			for (i = 0; i < fmt_chunk.num_channels; i++) {
				comprimir_huffman(data_adjusted[i]);
			}
			break;
		case 2:

			break;

		case 3:

			break;
		default:
			break;
		}
	}
	return result;
}

int main (int argc, char* argv[]) {
	FILE *fp = NULL;
	char *in_file = "resources/pig.wav";
	result_t result;
	char mode;								/* compress/decompress */

	printf("Choose compress(c) or decompress(d): ");
	scanf("%c", mode);

	printf("Enter with the path and name of the sound/compressed file (including de extension -- .wav or .bin): ");
	scanf("%s", in_file);

	if (mode == 'c') {
		result = read_sound(in_file, fp);
	}

	if (result == -ERR_FAIL) {
		printf("Wrong path/file/extension or file doesn't exist!\n");
		goto ERR_FILE;
	}

	if (mode == 'c') {
		if (fmt_chunk.audio_format != 1) {
			printf("Compressed file!\n");
			goto COMPRESSED_FILE;
		}
	}

	if (mode == 'c') {
		result = compress();
	} else {
		if (mode == 'd') {
			printf("Decompress mode \n");
		} else {
			printf("Invalid mode!\n");
			goto INVALID_MODE;
		}
	}
	system_pause();
	return result;

INVALID_MODE:
COMPRESSED_FILE:
ERR_FILE:
	result = -ERR_FAIL;
	system_pause();
	return result;
}
