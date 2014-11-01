/*
 * common.h
 *
 *  Created on: Oct 31, 2014
 *      Author: gustavo
 */

#ifndef INC_COMMON_H_
#define INC_COMMON_H_

#undef TRACE
#define TRACE(fmt, ...) printf(fmt, ## __VA_ARGS__)

#undef	ERR_NO
#define ERR_NO 0

#undef	ERR_FAIL
#define	ERR_FAIL 1

#define PRINT_STRUCT(p) print_struct((p), sizeof(*(p)))

void print_struct(void const *vp, size_t n) {
    unsigned char const *p = vp;
    size_t i;
    for (i = 0; i < n; i++)
        printf("%02X\n", p[i]);
    putchar('\n');
};

typedef int8_t result_t;

#endif /* INC_COMMON_H_ */
