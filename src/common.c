/*
 * common.c
 *
 *  Created on: Nov 1, 2014
 *      Author: gustavo
 */

#include "../inc/common.h"

/* reverse:  reverse string s in place */
void reverse(char s[]) {
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

char *byte_to_binary(uint8_t x){
    char *b;
    uint8_t z;

    b = (char*) malloc(sizeof(char) * (MAX_BITS + 1));
    memset(b, '\0', sizeof(char) * (MAX_BITS + 1));

    for (z = 128; z > 0; z >>= 1){
        strncat(b, ((x & z) == z) ? "1" : "0", sizeof(char));
    }

    return b;
}

result_t write_header_to_file(char *out_file, riff_chunk_t riff_chunk, fmt_chunk_t fmt_chunk, unsigned char c) {
	FILE *fp;
	size_t writing;
	result_t result;

	fp = fopen(out_file, "wb");
	if (fp == NULL) {
		TRACE("Error to open the out file!\n");
		result = -ERR_FAIL;
		return result;
	}

	c <<= 4;
	c += fmt_chunk.num_channels - 1;
	writing = fwrite(&c, sizeof(unsigned char), 1, fp);
	if (writing != 1) {
		TRACE("Error to write in the file -- num channels and mode sector\n");
		result = -ERR_FAIL;
		fclose(fp);
		return result;
	}

	writing = fwrite(&riff_chunk.chunk_size, sizeof(uint32_t), 1, fp);
	if (writing != 1) {
		TRACE("Error to write in the file -- chunk size sector\n");
		result = -ERR_FAIL;
		fclose(fp);
		return result;
	}

	writing = fwrite(&fmt_chunk.sample_rate, sizeof(uint32_t), 1, fp);
	if (writing != 1) {
		TRACE("Error to write in the file -- sample rate sector\n");
		result = -ERR_FAIL;
		fclose(fp);
		return result;
	}

	fclose(fp);
	result = -ERR_NO;
	return result;
}

void print_struct(void const *vp, size_t n) {
    unsigned char const *p = vp;
    size_t i;
    for (i = 0; i < n; i++)
        printf("%02X\n", p[i]);
    putchar('\n');
};
