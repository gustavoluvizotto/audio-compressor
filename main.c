/*
 * main.c
 *
 *  Created on: Oct 28, 2014
 *      Author: gustavo
 *
 *  Definitions:
 *  	Bits per sample = 8
 *  	Block align = NumChannels
 *  	ByteRate = SampleRate * NumChannels
 *  	Max(NumChannels) = 16
 *
 *  TODO Check for non-freed memory.
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

result_t compress(char out_file[]) {
	size_t i, j;					/* index */
	int ans = 1;					/* answer of the user about compress modes */
	int modes[3];					/* modes of compress */
	uint8_t **data_adjusted;		/* the input data per channel adjusted for every compression */
	uint32_t data_channel_size;		/* lenght of *data vector per channel */
	tree_t **huffman_tree = NULL;	/* one huffman tree per channel */
	result_t result;				/* return result */
	uint16_t num_channels;
	unsigned char c = 0;

	num_channels = fmt_chunk.num_channels;
	TRACE("Number of channels in the audio: %hu\n", num_channels);

	data_channel_size = number_of_samples/num_channels;
	TRACE("Number of samples per channel: %u\n", data_channel_size);

	huffman_tree = (tree_t**) malloc(num_channels * sizeof(tree_t*));
	for (i = 0; i < num_channels; i++) {
		tree_create(&huffman_tree[i]);
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
			switch(ans) {
			case 1:
				if (i == 0) {
					c += 1;
				} else {
					if (i == 1) {
						c <<= 2;
						if (modes[i-1] == 3) {
							c += 2;
						}
					} else {
						c += 1;
					}
				}
				break;
			case 2:
				if (i == 0) {
					c += 2;
				} else {
					if (i == 1) {
						c <<= 2;
					} else {
						c += 1;
					}
				}
				break;
			case 3:
				if (i == 0) {
					c += 3;
				} else {
					if (i == 1) {
						c <<= 2;
						c += 2;
					} else {
						c += 1;
					}
				}
				break;
			default:	/* Exit case (0) */
				break;
			}
			modes[i++] = ans;
		}
	} while (ans != 0 && i < 3);

	if (modes[0] == 0) {
		result = -ERR_NO;
		TRACE("Run again and choose one compression mode\n");
		system_pause();
		return result;
	}

	data_adjusted = (uint8_t**) malloc(num_channels * sizeof(uint8_t*));

	for (i = 0; i < num_channels; i++) {
		data_adjusted[i] = (uint8_t*) malloc (data_channel_size * sizeof(uint8_t));
	}
	for (i = 0; i < num_channels; i++) {
		for (j = 0; j < data_channel_size; j++) {
			data_adjusted[i][j] = data_sample[i*data_channel_size + j];
		}
	}

	for (i = 0; i < 3; i++){
		switch (modes[i]) {
		case 1:
			for (j = 0; j < num_channels; j++) {
				TRACE("Comprresing by Huffman...\n");
				huffman_tree[j]->root = huffman_compress(data_adjusted[j], data_channel_size);
				TRACE("Huffman compress for channel %zd successfull!\n", j+1);
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
	TRACE("Writing in the output file...\n");
	result = write_header_to_file(out_file, riff_chunk, fmt_chunk, c);
	TRACE("Common header has been written!\n");
	TRACE("Writing compressed bytes. This step may take some time, please wait...\n");
	for (j = 0; j < num_channels; j++) {
		result = write_huffman(huffman_tree[j]->root, data_adjusted[j], out_file, data_channel_size);
	}
	TRACE("Compressed bytes have been written!\n");
	TRACE("Output file has been written!\n");

	return result;
}

result_t decompress(char* in_file) {
	result_t result = -ERR_NO;
	FILE *fp, *out_fp;
	unsigned char modes_and_channels;
	uint8_t mode;
	int modes[3];
	size_t i, j, k;
	size_t writing;
	uint32_t data_channel_size;
	char*** codes = NULL;
	table_t** table = NULL;
	char* out_file;

	/* creating template wav file */
	memset(riff_chunk.chunk_id, '\0', 4 * sizeof(char));
	strncpy(riff_chunk.chunk_id, "RIFF", 4 * sizeof(char));
	memset(riff_chunk.format, '\0', 4 * sizeof(char));
	strncpy(riff_chunk.format, "WAVE", 4 * sizeof(char));
	memset(fmt_chunk.sub_chunk1_id, '\0', 4 * sizeof(char));
	strncpy(fmt_chunk.sub_chunk1_id, "fmt ", 4 * sizeof(char));
	fmt_chunk.sub_chunk1_size = 16;
	fmt_chunk.bits_per_sample = 0x0800;
	fmt_chunk.audio_format = 1;
	memset(data_chunk.sub_chunk2_id, '\0', 4 * sizeof(char));
	strncpy(data_chunk.sub_chunk2_id, "data", 4 * sizeof(char));

	fp = fopen(in_file, "rb");
	if (fp == NULL) {
		TRACE("[ERROR] Fail to open file %s\n", in_file);
		result = -ERR_FAIL;
		return result;
	}

	fread(&modes_and_channels, sizeof(unsigned char), 1, fp);
	mode = (modes_and_channels & 0xF0) >> 4;
	fmt_chunk.num_channels = (modes_and_channels & 0x0F) + 1;
	fmt_chunk.block_align = fmt_chunk.num_channels;

	fread(&riff_chunk.chunk_size, sizeof(uint32_t), 1, fp);
	data_chunk.sub_chunk2_size = riff_chunk.chunk_size - 36;

	fread(&fmt_chunk.sample_rate, sizeof(uint32_t), 1, fp);
	fmt_chunk.byte_rate = fmt_chunk.sample_rate * fmt_chunk.num_channels;

	switch((mode >> 2) & 0x03) {
		case 0:
			modes[0] = mode & 0x03;
			modes[1] = 0;
			modes[2] = 0;
			break;
		case 1:
			modes[0] = 1;
			modes[1] = ((mode >> 1) & 0x01) == 0 ? 2 : 3;								/* check the third bit if is 0 then option is D else option is M */
			modes[2] = (mode & 0x01) == 0 ? 0 : (((mode >> 1) & 0x01) == 0 ? 3 : 2); 	/* what? */
			break;
		case 2:
			modes[0] = 2;
			modes[1] = ((mode >> 1) & 0x01) == 0 ? 1 : 3;								/* check the third bit if is 0 then option is H else option is M */
			modes[2] = (mode & 0x01) == 0 ? 0 : (((mode >> 1) & 0x01) == 0 ? 3 : 1); 	/* xD */
			break;
		case 3:
			modes[0] = 3;
			modes[1] = ((mode >> 1) & 0x01) == 0 ? 2 : 1;								/* check the third bit if is 0 then option is D else option is H */
			modes[2] = (mode & 0x01) == 0 ? 0 : (((mode >> 1) & 0x01) == 0 ? 1 : 2); 	/* Lol */
			break;
	}

	data_channel_size = data_chunk.sub_chunk2_size / fmt_chunk.num_channels;
	data_sample = (uint8_t*) malloc(data_chunk.sub_chunk2_size * sizeof(uint8_t));

	codes = (char***) malloc(fmt_chunk.num_channels * sizeof(char**));

	table = (table_t**) malloc(fmt_chunk.num_channels * sizeof(table_t*));
	for (i = 0; i < fmt_chunk.num_channels; i++) {
		codes[i] = (char**) malloc(data_channel_size * sizeof(char*));
		table[i] = NULL;
	}
	for (i = 0; i < fmt_chunk.num_channels; i++) {
		for (j = 0; j < data_channel_size; j++) {
			codes[i][j] = NULL;
		}
	}

	for (i = 0; i < 3; i++) {
		switch(modes[i]) {
			case 1:				/* Huffman compression */
				for (j = 0; j < fmt_chunk.num_channels; j++) {
					result = huffman_decompress(fp, table[j], data_channel_size, codes[j]);
					for (k = 0; k < data_channel_size; k++) {
						data_sample[k + j*data_channel_size] = search_code(table[j], codes[j][k]);
					}
				}
				break;
			case 2:				/* Differences compression */
				break;
			case 3:				/* MDCT compression */
				break;
			default:			/* Exit case (0) */
				break;
		}
	}

	out_file = (char*) malloc((strlen(in_file) - 3) * sizeof(char));
	memset(out_file, '\0', (strlen(in_file) - 3) * sizeof(char));
	strncpy(out_file, in_file, (strlen(in_file) - 4) * sizeof(char));

	out_fp = fopen(out_file, "wb");
	if (out_fp == NULL) {
		TRACE("[ERROR] Fail to open file %s\n", out_file);
		result = -ERR_FAIL;
		return result;
	}

	writing = fwrite(&riff_chunk, sizeof(riff_chunk), 1, out_fp);
	if (writing != 1) {
		TRACE("[ERROR] Fail to write in the file -- riff sector\n");
		result = -ERR_FAIL;
		return result;
	}
	writing = fwrite(&fmt_chunk, sizeof(fmt_chunk), 1, out_fp);
	if (writing != 1) {
		TRACE("[ERROR] Fail to write in the file -- fmt sector\n");
		result = -ERR_FAIL;
		return result;
	}
	writing = fwrite(&data_chunk, sizeof(data_chunk), 1, out_fp);
	if (writing != 1) {
		TRACE("[ERROR] Fail to write in the file -- data sector\n");
		result = -ERR_FAIL;
		return result;
	}
	for (i = 0; i < data_chunk.sub_chunk2_size; i++) {
		writing = fwrite(&data_sample[i], sizeof(uint8_t), 1, out_fp);
		if (writing != 1) {
			TRACE("[ERROR] Fail to write in the file -- sample sector\n");
			result = -ERR_FAIL;
			return result;
		}
	}

	fclose(fp);

	return result;
}

int main () {
	char in_file[40] = "";					/* input file to read */
	result_t result = -ERR_NO;				/* return functions */
	char mode;								/* compress/decompress mode */
	char out_file[40] = "";

	printf("Choose compress(c) or decompress(d): ");
	/*scanf("%c", &mode);*/
	mode = 'c';

	if (mode == 'c') {
		printf("Enter with the path and name of the sound file (including the .wav extension): ");
	} else {
		if (mode == 'd') {
			printf("Enter with the path and name of the compressed file (including the .bin extension): ");
		} else {
			TRACE("Invalid mode!\n");
			result = -ERR_FAIL;
			return result;
		}
	}
	fflush(stdin);
	/*scanf("%s", in_file);*/
	strcpy(in_file, "resources/example.wav");
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

	if (fmt_chunk.bits_per_sample != 8 && mode == 'c') {
		TRACE("Bits per sample not valid. This application accepts only 8 bits per sample!\n");
		result = -ERR_FAIL;
		system_pause();
		return result;
	}

	if (fmt_chunk.num_channels > 16 && mode == 'c') {
		TRACE("Number of channels exceed the limits. Max is 16!\n");
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
		strncat(out_file, in_file, strlen(in_file));
		strncat(out_file, ".bin", strlen(".bin"));
		result = compress(out_file);
	} else {
		result = decompress(in_file);
	}

	system_pause();
	return result;
}
