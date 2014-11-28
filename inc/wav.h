/*
 * wav.h
 *
 *  Created on: Oct 30, 2014
 */

#ifndef INC_WAV_H_
#define INC_WAV_H_

#include <stdint.h>

#pragma pack (push, 1) 	/* exact fit; without padding*/
typedef struct {
	char chunk_id[4];           /* big      */
	uint32_t chunk_size;        /* little   */
	char format[4];             /* big      */
} riff_chunk_t;
#pragma pack (pop)		/* back to the normal format pack mode */

#pragma pack (push, 1)
typedef struct {
	char sub_chunk1_id[4];      /* big      */
    uint32_t sub_chunk1_size;   /* little   */
    uint16_t audio_format;      /* little   */
    uint16_t num_channels;      /* little   */
    uint32_t sample_rate;       /* little   */
    uint32_t byte_rate;         /* little   */
    uint16_t block_align;       /* little   */
    uint16_t bits_per_sample;   /* little   */
} fmt_chunk_t;
#pragma pack (pop)

#pragma pack (push, 1)
typedef struct {
	char sub_chunk2_id[4];      /* big      */
    uint32_t sub_chunk2_size;   /* little   */
} data_chunk_t;
#pragma pack (pop)

uint8_t *data_sample;			/* little  	*/

#endif /* INC_WAV_H_ */
