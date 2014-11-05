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

/* itoa:  convert n to characters in s */
void itoa(int n, char s[]) {
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

char *byte_to_binary(uint8_t x){
    static char b[9];
    b[0] = '\0';
    uint8_t z;

    for (z = 128; z > 0; z >>= 1){
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

result_t write_header_to_file(char *out_file, fmt_chunk_t fmt_chunk) {
	FILE *fp;
	size_t writing;
	result_t result;

	fp = fopen(out_file, "wb");
	if (fp == NULL) {
		TRACE("Error to open the out file!\n");
		result = -ERR_FAIL;
		return result;
	}

	writing = fwrite(&fmt_chunk.num_channels, sizeof(uint16_t), 1, fp);
	if (writing != 1) {
		TRACE("Error to write in the file -- num channels sector\n");
		result = -ERR_FAIL;
		return result;
	}

	writing = fwrite(&fmt_chunk.sample_rate, sizeof(uint32_t), 1, fp);
	if (writing != 1) {
		TRACE("Error to write in the file -- sample rate sector\n");
		result = -ERR_FAIL;
		return result;
	}

	writing = fwrite(&fmt_chunk.byte_rate, sizeof(uint32_t), 1, fp);
	if (writing != 1) {
		TRACE("Error to write in the file -- byte rate sector\n");
		result = -ERR_FAIL;
		return result;
	}

	writing = fwrite(&fmt_chunk.block_align, sizeof(uint16_t), 1, fp);
	if (writing != 1) {
		TRACE("Error to write in the file -- block align sector\n");
		result = -ERR_FAIL;
		return result;
	}

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
