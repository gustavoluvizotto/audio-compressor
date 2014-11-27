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
 */

#include "inc/differences.h"
#include <stdio.h>

/*
#undef TRACE(fmt, ...)
#define TRACE(fmt, ...)
*/

riff_chunk_t riff_chunk;		/* header of wav file */
fmt_chunk_t fmt_chunk;			/* header of wav file */
data_chunk_t data_chunk;		/* header of wav file */
uint32_t number_of_samples = 0;

void print_informations() {
	printf("Grupo\n");
	printf("Adailto Aparecido Caramano - \n");
	printf("Gustavo Luvizotto Cesar - 6783544\n");
	printf("Ivan Donisete Lonel - \n");
	printf("\n");
	printf("O usuario pode comprimir seu arquivo wav de som de no maximo 16 canais\n");
	printf("utilizando o metodo de diferencas e/ou de huffman, podendo combina-los como desejar.\n");
	printf("Nem sempre a combinacao das compressoes gera um arquivo pequeno. A melhor compressao\n");
	printf("obtida foi usando diferencas (que usa huffman implicitamente para indexacao).\n");
	printf("O wav original deve possuir 8 bits por amostra e nao deve ser comprimido.");
	printf("\n");
}

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

uint32_t update_data(char *read_file) {
	FILE *fp;
	size_t size;
	uint32_t begin;
	size_t i;
	size_t reading;
	result_t result = ERR_NO;

	fp = fopen(read_file, "rb");

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, COMMON_HEADER, SEEK_SET);			/* skip common header */
	begin = size - COMMON_HEADER;				/* begin of data */

	free(data_sample);
	data_sample = (uint8_t*) malloc((begin) * sizeof(uint8_t));

	for (i = 0; i < begin; i++) {
		reading = fread(&data_sample[i], sizeof(uint8_t), 1, fp);
		if (reading != 1) {
			TRACE("[ERROR] Fail to read file -- sample sector\n");
			fclose(fp);
			result = -ERR_FAIL;
			return result;
		}
	}

	fclose(fp);

	return begin;
}

result_t compress(char out_file[]) {
	size_t i, k;					/* index */
	int ans = 1;					/* answer of the user about compress modes */
	int modes[3];					/* modes of compress */
	uint32_t data_channel_size;		/* lenght of *data vector per channel */
	tree_t *huffman_tree = NULL;	/* one huffman tree per channel */
	result_t result;				/* return result */
	uint16_t num_channels;			/* number of channels of the sound */
	unsigned char c = 0;			/* byte to create the table shown in commmon.h */
	char **codes;					/* vector of codes used in difference compress. Huffman code + value */
	frequency_t *frequency = NULL;	/* frequency of given sample */

	num_channels = fmt_chunk.num_channels;
	TRACE("Number of channels in the audio: %hu\n", num_channels);

	data_channel_size = number_of_samples/num_channels;
	TRACE("Number of samples per channel: %u\n", data_channel_size);
	tree_create(&huffman_tree);
	codes = (char**) malloc(number_of_samples * sizeof(char*));

	for (i = 0; i < number_of_samples; i++) {
		codes[i] = NULL;
	}

	printf("Choose the compression you want\n");
	printf("0. Exit\n1. Huffman\n2. Differences\n");

	for (i = 0; i < 3; i++) {
		modes[i] = -1;
	}
	modes[3] = 0;	/* MDCT not suported */

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
			case 3:		/* MDCT not suported */
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
	} while (ans != 0 && ans != 3 && i < 2);

	if (modes[0] == 0) {
		result = -ERR_NO;
		printf("Run again and choose one compression mode\n");
		system_pause();
		return result;
	}

	TRACE("Writing the common header...\n");
	result = write_header_to_file(out_file, riff_chunk, fmt_chunk, c);
	TRACE("Common header has been written!\n");

	for (i = 0; i < 3; i++){
		switch (modes[i]) {
		case 1:
			frequency = (frequency_t*) malloc ((MAX_SAMPLE + 1) * sizeof(frequency_t));
			for (k = 0; k <= MAX_SAMPLE; k++) {
				frequency[k] = 0;
			}
			TRACE("Comprresing by Huffman...\n");
			huffman_tree->root = huffman_compress(data_sample, frequency, number_of_samples, MAX_SAMPLE);
			TRACE("Huffman compress successfull!\n");
			TRACE("Writing compressed bytes. This step may take some time, please wait...\n");
			result = write_huffman(huffman_tree->root, data_sample, frequency, out_file, number_of_samples);
			TRACE("Compressed bytes have been written!\n");
			/* update data samples to input in an other compression, if is the case */
			number_of_samples = update_data(out_file);
			break;
		case 2:
			frequency = (frequency_t*) malloc ((MAX_BITS + 1) * sizeof(frequency_t));
			for (k = 0; k <= MAX_BITS; k++) {
				frequency[k] = 0;
			}
			TRACE("Compressing by Differences...\n");
			huffman_tree->root = diff_compress(data_sample, frequency, codes, number_of_samples);
			TRACE("Difference compress successfull!\n");
			TRACE("Writing compressed bytes. This step may take some time, please wait...\n");
			write_differences(frequency, codes, out_file, number_of_samples);
			TRACE("Compressed bytes have been written!\n");
			/* update data samples to input in an other compression, if is the case */
			number_of_samples = update_data(out_file);
			break;
		case 3:
			break;
		default:
			break;
		}
	}
	printf("Compression completed!\n");
	/* free memory */
	free(data_sample);
	free(huffman_tree);

	return result;
}

/* writes in an intermediate file num_samples of data_sample */
void write_intermediate(char *bin_file, uint32_t num_samples) {
	FILE *fp;
	size_t i;

	fp = fopen(bin_file, "rb+");

	fseek(fp, COMMON_HEADER, SEEK_SET);

	for(i = 0; i < num_samples; i++) {
		fwrite(&data_sample[i], sizeof(uint8_t), 1, fp);
	}

	fclose(fp);

	return;
}

result_t decompress(char* in_file) {
	result_t result = -ERR_NO;				/* control return functions */
	FILE *fp, *out_fp;						/* in and out file pointer */
	unsigned char modes_and_channels;		/* for read the byte who stores modes of compression and the channels of wav */
	uint8_t mode;							/* modes of compression */
	int modes[3];							/* modes of compression */
	size_t i, j, k;							/* loop indexes */
	size_t writing;							/* control of writable bytes */
	char** codes = NULL;					/* vector of data codes */
	table_t table;							/* huffman table */
	char* out_file;							/* name of the output file */
	frequency_t *frequency = NULL;			/* frequency of given sample */
	int16_t *differences = NULL;			/* differences vector (differences decompression) */
	uint8_t *aux_data_sample = NULL;		/* intermediate data_sample */
	int16_t sign = 1;						/* sign byte to perform one's complement */
	uint32_t num_samples;					/* number of samples of each compression mode */
	uint32_t header_counter = 0;			/* counts the number of bytes of a header (difference or huffman) */

	/* creating template wav file */
	memset(riff_chunk.chunk_id, '\0', 4 * sizeof(char));
	strncpy(riff_chunk.chunk_id, "RIFF", 4 * sizeof(char));
	memset(riff_chunk.format, '\0', 4 * sizeof(char));
	strncpy(riff_chunk.format, "WAVE", 4 * sizeof(char));
	memset(fmt_chunk.sub_chunk1_id, '\0', 4 * sizeof(char));
	strncpy(fmt_chunk.sub_chunk1_id, "fmt ", 4 * sizeof(char));
	fmt_chunk.sub_chunk1_size = 16;
	fmt_chunk.bits_per_sample = 0x0008;
	fmt_chunk.audio_format = 1;
	memset(data_chunk.sub_chunk2_id, '\0', 4 * sizeof(char));
	strncpy(data_chunk.sub_chunk2_id, "data", 4 * sizeof(char));

	fp = fopen(in_file, "rb");
	if (fp == NULL) {
		printf("[ERROR] Fail to open file %s\n", in_file);
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

	fclose(fp);

	/* re creating the modes of compression based on the table shown in common.h */
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

	for (i = 0; i < 3; i++) {
		switch(modes[2-i]) {
			case 1:				/* Huffman compression */
				TRACE("Huffman decompressing...\n");
				fp = fopen(in_file, "rb");
				/* skipping common header... */
				fseek(fp, COMMON_HEADER, SEEK_SET);
				/* and getting the number of samples of this compression */
				fread(&num_samples, sizeof(uint32_t), 1, fp);

				data_sample = (uint8_t*) malloc(num_samples * sizeof(uint8_t));

				frequency = (frequency_t*) malloc ((MAX_SAMPLE + 1) * sizeof(frequency_t));
				for (k = 0; k <= MAX_SAMPLE; k++) {
					frequency[k] = 0;
				}
				codes = (char**) malloc(num_samples * sizeof(char*));
				table.rows = NULL;
				for (k = 0; k < num_samples; k++) {
					codes[k] = (char*) malloc(MAX_CODE * sizeof(char));
				}

				header_counter = huffman_decompress(fp, &table, frequency, num_samples, codes);
								 	 	 	 	   /* data              frequency             bits               count            num_samples*/
				header_counter = header_counter * (sizeof(uint8_t) + sizeof(frequency_t)) + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t);
				for (k = 0; k < num_samples; k++) {
					data_sample[k] = (uint8_t) search_code(table, codes[k]);
				}
				TRACE("Huffman decompress ok!\n");
				fclose(fp);
				TRACE("Updating the .bin file...\n");
				/* write in the bin file the data of the next compression to pass through the next decompressor, if it have */
				write_intermediate(in_file, num_samples);
				TRACE(".bin file has been updated!\n");
				break;
			case 2:				/* Differences compression */
				TRACE("Differences decompressing...\n");
				fp = fopen(in_file, "rb");
				/* skipping common header... */
				fseek(fp, COMMON_HEADER, SEEK_SET);
				/* and getting the number of samples of this compression */
				fread(&num_samples, sizeof(uint32_t), 1, fp);

				data_sample = (uint8_t*) malloc(num_samples * sizeof(uint8_t));

				frequency = (frequency_t*) malloc ((MAX_BITS + 1) * sizeof(frequency_t));
				for (k = 0; k <= MAX_BITS; k++) {
					frequency[k] = 0;
				}
				codes = (char**) malloc(num_samples * sizeof(char*));
				for (k = 0; k < num_samples; k++) {
					codes[k] = (char*) malloc(MAX_CODE * sizeof(char));
				}

				differences = (int16_t*) malloc(num_samples * sizeof(int16_t));
				aux_data_sample = (uint8_t*) malloc(num_samples * sizeof(uint8_t));
				for (j = 0; j < num_samples; j++) {
					aux_data_sample[j] = 0;
				}

				header_counter = differences_decompress(fp, frequency, num_samples, codes);
	 	 	 	   	   	   	   	   	   	   	   	   /* data              frequency             bits               count          num_samples */
				header_counter = header_counter * (sizeof(uint8_t) + sizeof(frequency_t)) + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t);
				for (k = 0; k < num_samples; k++) {
					sign = 1;
					if (codes[k][0] == '0') {			/* check if we need to complement the code */
						perform_one_complement(codes[k]);
						sign = -1;
					}
					/* creating the differences vector */
					differences[k] = binary_string_to_int16(codes[k]);
					differences[k] = sign * differences[k];
				}
				from_differences(aux_data_sample, differences, num_samples);
				for (k = 0; k < num_samples; k++) {
					data_sample[k] = aux_data_sample[k];
				}
				TRACE("Differences decompress ok!\n");
				fclose(fp);
				TRACE("Updating the .bin file...\n");
				/* write in the bin file the data of the next compression to pass through the next decompressor, if it have */
				write_intermediate(in_file, num_samples);
				TRACE(".bin file has been updated!\n");
				break;
			case 3:				/* MDCT compression not suported */
				break;
			default:			/* Exit case (0) */
				break;
		}
	}

	/* writing the final decompressed file! */
	num_samples = data_chunk.sub_chunk2_size;
	out_file = (char*) malloc((strlen(in_file) - 3) * sizeof(char));
	memset(out_file, '\0', (strlen(in_file) - 3) * sizeof(char));
	strncpy(out_file, in_file, (strlen(in_file) - 4) * sizeof(char));

	out_fp = fopen(out_file, "wb");
	if (out_fp == NULL) {
		TRACE("[ERROR] Fail to open file %s\n", out_file);
		result = -ERR_FAIL;
		return result;
	}

	printf("Writing the common header of wav file...\n");
	printf("Writing the riff chunk...\n");
	writing = fwrite(&riff_chunk, sizeof(riff_chunk), 1, out_fp);
	if (writing != 1) {
		TRACE("[ERROR] Fail to write in the file -- riff sector\n");
		result = -ERR_FAIL;
		return result;
	}
	printf("Riff chunk has been written!\n");
	printf("Writing the format chunk...\n");
	writing = fwrite(&fmt_chunk, sizeof(fmt_chunk), 1, out_fp);
	if (writing != 1) {
		TRACE("[ERROR] Fail to write in the file -- fmt sector\n");
		result = -ERR_FAIL;
		return result;
	}
	printf("Format chunk has been written!\n");
	printf("Writing the data chunk...\n");
	writing = fwrite(&data_chunk, sizeof(data_chunk), 1, out_fp);
	if (writing != 1) {
		TRACE("[ERROR] Fail to write in the file -- data sector\n");
		result = -ERR_FAIL;
		return result;
	}
	printf("Data chunk has been written!\n");
	printf("Common header of wav file has been written!\n");

	printf("Writing the data samples...\n");
	for (i = 0; i < num_samples; i++) {
		writing = fwrite(&data_sample[i], sizeof(uint8_t), 1, out_fp);
		if (writing != 1) {
			TRACE("[ERROR] Fail to write in the file -- sample sector\n");
			result = -ERR_FAIL;
			return result;
		}
	}
	printf("Data samples has been written!\n");

	if (differences != NULL)
		free(differences);
	fclose(out_fp);
	printf("Decompress proccess done! Try to listen the generated sound =)\n");
	return result;
}

int main () {
	char in_file[40] = "";					/* input file to read */
	result_t result = -ERR_NO;				/* return functions */
	char mode;								/* compress/decompress mode */
	char out_file[40] = "";					/* output file to write .bin or the final .wav */

	printf("Choose compress(c), decompress(d) or help (h) for further informations\n");
	printf("Option: ");
	/*scanf("%c", &mode);*/
	mode = 'd';

	if (mode == 'c') {
		printf("Enter with the path and name of the sound file (including the .wav extension): ");
	} else {
		if (mode == 'd') {
			printf("Enter with the path and name of the compressed file (including the .bin extension): ");
		} else {
			printf("Invalid mode!\n");
			result = -ERR_FAIL;
			return result;
		}
	}
	fflush(stdin);
	/*scanf("%s", in_file);*/
	strcpy(in_file, "resources/up.wav.bin");
	if (mode == 'c') {
		TRACE("Compress mode!\n");
		result = read_sound(in_file);
	} else {
		TRACE("Decompress mode!\n");
	}

	if (result == -ERR_FAIL) {
		printf("Wrong path/file/extension or file doesn't exist!\n");
		result = -ERR_FAIL;
		system_pause();
		return result;
	}

	if (fmt_chunk.bits_per_sample != 8 && mode == 'c') {
		printf("Bits per sample not valid. This application accepts only 8 bits per sample!\n");
		result = -ERR_FAIL;
		system_pause();
		return result;
	}

	if (fmt_chunk.num_channels > 16 && mode == 'c') {
		printf("Number of channels exceed the limits. Max is 16!\n");
		result = -ERR_FAIL;
		system_pause();
		return result;
	}


	if (mode == 'c') {
		if (fmt_chunk.audio_format != 1) {
			printf("Compressed file! Try one that is not compressed.\n");
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
