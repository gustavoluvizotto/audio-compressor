/*
 * main.c
 *
 *  Created on: Oct 28, 2014
 *      Author: gustavo
 */

#include "inc/wav.h"
#include "inc/common.h"
#include "inc/huffman.h"
#include <stdio.h>

/*
#undef TRACE(fmt, ...)
#define TRACE(fmt, ...)
*/

riff_chunk_t riff_chunk;		/* header of wav file */
fmt_chunk_t fmt_chunk;			/* header of wav file */
data_chunk_t data_chunk;		/* header of wav file */
uint32_t number_of_samples = 0;

void print_headers() {
	PRINT_STRUCT(&riff_chunk);
	PRINT_STRUCT(&fmt_chunk);
	PRINT_STRUCT(&data_chunk);

	TRACE("riff: %lu \n", sizeof(riff_chunk));
	TRACE("fmt: %lu \n", sizeof(fmt_chunk));
	TRACE("data: %lu \n", sizeof(data_chunk));
}

void system_pause() {
    printf("Press <ENTER> to exit... ");
    fflush(stdin);
    getchar();
}

result_t read_sound(char *in_file) {
	FILE *fp = NULL;			/* pointer to read and write file */
	size_t reading;
	uint32_t size;
	size_t i;
	uint32_t header_size;
	result_t result;

	fp = fopen(in_file, "rb");
	if (fp == NULL) {
		TRACE("[ERROR] Fail to open file %s\n", in_file);
		result = -ERR_FAIL;
		return result;
	}

	/* getting the size of the file */
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	/* reads header */
	reading = fread(&riff_chunk, sizeof(riff_chunk), 1, fp);
	if (reading != 1) {
		TRACE("[ERROR] Fail to read file -- riff chunk sector\n");
		result = -ERR_FAIL;
		return result;
	}
	reading = fread(&fmt_chunk, sizeof(fmt_chunk), 1, fp);
	if (reading != 1) {
		TRACE("[ERROR] Fail to read file -- fmt chunk sector\n");
		result = -ERR_FAIL;
		return result;
	}
	reading = fread(&data_chunk, sizeof(data_chunk), 1, fp);
	if (reading != 1) {
		TRACE("[ERROR] Fail to read file -- data chunk sector\n");
		result = -ERR_FAIL;
		return result;
	}

	header_size = sizeof(riff_chunk) + sizeof(fmt_chunk) + sizeof(data_chunk);

	number_of_samples = (size-header_size);

	data_sample = (uint8_t *) malloc(number_of_samples * sizeof(uint8_t));

	for (i = 0; i < number_of_samples; i++) {
		reading = fread(&data_sample[i], sizeof(uint8_t), 1, fp);
		if (reading != 1) {
			TRACE("[ERROR] Fail to read file -- sample sector\n");
			result = -ERR_FAIL;
			return result;
		}
	}

	result = -ERR_NO;
	fclose(fp);
	TRACE("Read file complete!\n");
	return result;
}

uint8_t search_for_equal_element(int list[], int size, int key) {
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
	size_t i, j;					/* index */
	int ans = 1;					/* answer of the user about compress modes */
	int modes[3];					/* modes of compress */
	uint8_t **data_adjusted;		/* the input data per channel adjusted for every compression */
	uint32_t data_size_channel;		/* lenght of *data vector per channel */
	tree_t **huffman_tree = NULL;	/* one huffman tree per channel */
	result_t result;				/* return result */
	uint16_t num_channels;

	num_channels = fmt_chunk.num_channels;
	TRACE("Number of channels in the audio: %hu\n", num_channels);

	huffman_tree = (tree_t**) malloc(num_channels * sizeof(tree_t*));
	for (i = 0; i < num_channels; i++) {
		tree_create(huffman_tree[i]);
	}

	printf("Choose the compression you want\n");
	printf("0. Exit\n1. Huffman\n2. Differences\n3. MDCT\n");

	for (i = 0; i < 3; i++) {
		modes[i] = -1;
	}

	i = 0;
	do {
		scanf("%d", &ans);
		if (search_for_equal_element(modes, 3, ans)) {
			printf("Select a different compression mode!\n");
		} else {
			modes[i++] = ans;
		}
	} while (ans != 0 && i < 3);

	data_size_channel = number_of_samples/num_channels;
	TRACE("Number of samples per channel: %u", data_size_channel);

	data_adjusted = (uint8_t**) malloc(num_channels * sizeof(uint8_t*));

	for (i = 0; i < num_channels; i++) {
		data_adjusted[i] = (uint8_t*) malloc (data_size_channel * sizeof(uint8_t));
	}
	for (i = 0; i < num_channels; i++) {
		for (j = 0; j < data_size_channel; j++) {
			data_adjusted[i][j] = data_sample[i*data_size_channel + j];
		}
	}

	for (i = 0; i < 3; i++){
		switch (modes[i]) {
		case 1:
			for (j = 0; j < num_channels; j++) {
				huffman_tree[j]->root = comprimir_huffman(data_adjusted[j], number_of_samples);
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

	for (i = 0; i < num_channels; i++) {
		in_order(huffman_tree[i]->root);
	}

	result = -ERR_NO;
	return result;
}

int main () {
	char in_file[40] = "";					/* input file to read */
	result_t result = -ERR_NO;				/* return functions */
	char mode;								/* compress/decompress mode */

	printf("Choose compress(c) or decompress(d): ");
	scanf("%c", &mode);

	if (mode == 'c') {
		printf("Enter with the path and name of the sound file (including the .wav extension: ");
	} else {
		if (mode == 'd') {
			printf("Enter with the path and name of the compressed file (including the .bin extension: ");
		} else {
			TRACE("Invalid mode!\n");
			result = -ERR_FAIL;
			return result;
		}
	}
	fflush(stdin);
	scanf("%s", in_file);

	if (mode == 'c') {
		result = read_sound(in_file);
	} else {
		TRACE("Decompress mode!");
	}

	if (result == -ERR_FAIL) {
		printf("Wrong path/file/extension or file doesn't exist!\n");
		result = -ERR_FAIL;
		system_pause();
		return result;
	}

	if (mode == 'c') {
		if (fmt_chunk.audio_format != 1) {
			printf("Compressed file!\n");
			result = -ERR_FAIL;
			system_pause();
			return result;
		}
		result = compress();
	} else {
		TRACE("Decompress mode \n");
	}

	system_pause();
	return result;
}
