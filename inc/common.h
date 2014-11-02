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

#undef TRACE
#define TRACE(fmt, ...) printf(fmt, ## __VA_ARGS__)

#undef	ERR_NO
#define ERR_NO 0

#undef	ERR_FAIL
#define	ERR_FAIL 1

#define MAX_SAMPLE 510	/* 255 (negative numbers) + 1 (zero) + 255 (positive numbers) */
#define	MAXSIZE 32

typedef int8_t result_t;

void reverse(char s[]);
void itoa(int n, char s[]);

#endif /* INC_COMMON_H_ */
