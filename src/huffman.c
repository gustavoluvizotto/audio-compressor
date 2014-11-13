/*
 * huffman.c
 *
 *  Created on: Nov 1, 2014
 *      Author: gustavo
 *
 *  TODO Write the huffman header (frequencies) in binary mode, i.e., convert the uint16_t frequency type in binary string and discart the first zeros (save only the essential data).
 */

#include "../inc/huffman.h"
/*
#undef TRACE
#define TRACE(fmt, ...)
*/
node_t* huffman(uint16_t *frequency) {
    int i;														/* index */
    char *aux = (char*) malloc (sizeof(char) * (MAXSIZE + 1));	/* intermediare sample */
    node_t *x, *y;												/* min frequencies from queue */
    node_t *z;													/* inserted node */
    queue_t *queue = queue_create();							/* queue are the leaves elements of the tree */

    /*
     * In this step, we insert in the queue every frequency element. The sample element
     * is /SAMPLE/\0 format. Is our standard to store samples in the tree. In the next
     * step it will be usefull to create intermediate nodes in the tree (sum of frequencies
     * in the huffman tree).
     */
    for (i = 0; i <= MAX_SAMPLE; i++) {
        if (frequency[i] > 0) {
            memset(aux, '\0', sizeof(char) * (MAXSIZE + 1));
        	sprintf(aux, "/%d", i);
            z = create_node_tree();
            z->sample = (char*) malloc (sizeof(char) * MAX_LENGHT_SAMPLE);
            memset(z->sample, '\0', sizeof(char) * MAX_LENGHT_SAMPLE);
            strncpy(z->sample, aux, strlen(aux));
            z->frequency = frequency[i];
            insert_node_queue(&queue, z);
        }
    }

    /*
     * construct the huffman tree bottom up. Get two min frequency elements from queue,
     * concatenate to create a sub-root element and points to the two min frequency.
     * Insert the node (sub-root tree node) in the queue and repeate the process until
     * the last element of the queue.
     */
    while (queue->count > 1) {
        memset(aux, '\0', sizeof(char) * (MAXSIZE + 1));
        x = get_last_element_from_queue(&queue);
        y = get_last_element_from_queue(&queue);
        strcpy(aux, x->sample);
        strcat(aux, y->sample);
        z = create_node_tree();
        z->sample = (char*) malloc (sizeof(char) * (strlen(aux) + 1));
        memset(z->sample, '\0', sizeof(char) * (strlen(aux) + 1));
        strncpy(z->sample, aux, strlen(aux));
        z->frequency = x->frequency + y->frequency;
        z->left = x;
        z->right = y;
        insert_node_queue(&queue, z);
    }

    free(aux);

    /*
     * last element of a queue is the last element inserted. I.e., the root element of
     * the huffman tree
     */
    return get_last_element_from_queue(&queue);
}

/*
 * @param data needs to be the sample, difference vector (num of bits of difference vector)
 * or mdct vector.
 * @brief Compress using huffman code.
 * @return the Huffman tree.
 */
node_t* comprimir_huffman(uint8_t *data, uint32_t number_of_samples) {
	size_t i;								/* loop indexes */

	frequency = (uint16_t*) malloc (MAX_SAMPLE * sizeof(uint16_t));
	for (i = 0; i < MAX_SAMPLE; i++) {
		frequency[i] = 0;
	}

	/* begin the frequency count */
	for (i = 0; i < number_of_samples; i++) {
		frequency[data[i]]++;
	}

	return huffman(frequency);
}

/*
 *
 */
result_t write_huffman(node_t *root, uint8_t *data, char *out_file,  uint32_t number_of_samples) {
	unsigned char c;					/* Used to store each byte to be written on file */
	char* code = NULL;					/* Huffman code */
	size_t i, j;						/* Indexes */
	uint8_t k;							/* Index to write in the file */
	uint8_t bits = 0;					/* Used to control the number of bits in byte c */
	char** binaries;					/* Samples in binaries */
	result_t result;					/* Return result */

	binaries = (char**) malloc(number_of_samples * sizeof(char*));

	FILE *fp = fopen(out_file, "wb");
	if (fp == NULL) {
		TRACE("Error opening file.\n");
		result = -ERR_FAIL;
		return result;
	}

	/* Skipping the common header */
	if (fseek(fp, 0, SEEK_END) != 0) {
		TRACE("Error moving cursor to end of file.\n");
		result = -ERR_FAIL;
		return result;
	}

	/* Writing Huffman header */
	for (k = 0; k < MAX_SAMPLE; k++) {
		if (frequency[k] == 0) {
			fwrite(&frequency[k], sizeof(uint16_t), 1, fp);
			fwrite(&k, sizeof(uint8_t), 1, fp);
		}
	}

	for (i = 0; i < number_of_samples; i++) {
		binaries[i] = byte_to_binary(data[i]);
	}

	/* For each sample, write the Huffman code and sample itself */
	c = 0;
	for(i = 0; i < number_of_samples; i++) {
		code = get_code(root, data[i]); /* Obtain the huffman code for data[i] */
		TRACE("Code: %8s, Datum: %02X\n", code, data[i]);
		/* Write the huffman code in char c. Shift every character of code in bits of c */
		for(j = 0; j < strlen(code); j++) {
			if(code[j] == '1') {
				c <<= 1;
				c += 1;
			} else
				c <<= 1;
			bits++;
			if(bits == 8) {		/* if complete an entire byte, write in the output file */
				fwrite(&c, sizeof(unsigned char), 1, fp);
				c = 0;
				bits = 0;
			}
		}
	}

	/* If bits < 8, then complete with zeros the rest of char c */
	if(bits != 0) {
		i = bits;
		while(i < 8) {
			c <<= 1;
			i++;
		}
		fwrite(&c, sizeof(unsigned char), 1, fp); 		/* write the last c byte */
		fwrite(&bits, sizeof(uint8_t), 1, fp); 			/* write the number of bits of the last c byte without stuffing 0 */
	}

	/* free memory and close file */
	for (i = 0; i < number_of_samples; i++) {
		free(binaries[i]);
	}
	free(binaries);
	free(code);
	fclose(fp);

	result = -ERR_NO;
	return result;
}

/*
 * Obtains the Huffman code of a sample
 */
char* get_code (node_t* root, uint8_t data) {
	char* code = (char*) malloc(sizeof(char) * MAX_HUFF_CODE);
	char* string_data = (char*) malloc(sizeof(char) * MAX_LENGHT_SAMPLE);

	memset(code, '\0', sizeof(char) * MAX_HUFF_CODE);
	memset(string_data, '\0', sizeof(char) * MAX_LENGHT_SAMPLE);
	sprintf(string_data, "/%d", data);

	huffman_code(root, string_data, code);

	return code;
}

/*
 * Recursive function that get the huffman code of a data and put into code.
 * Runs the huffman tree, add (strcat) "0" for left branch and "1" to right branch.
 */
void huffman_code (node_t* root, char* data, char* code) {
    if (root == NULL || code == NULL) {
    	return;
    }
    if (strstr(root->sample, data) == NULL) {
    	return;
    }
    if (count_ocurrencies(root->sample, '/') == 1) {
    	return;
    } else {
        if (strstr(root->left->sample, data)) {
            strncat(code, "0", strlen("0"));
            huffman_code(root->left, data, code);
        } else if (strstr(root->right->sample, data)) {
            strncat(code, "1", strlen("1"));
            huffman_code(root->right, data, code);
        }
    }
}

int count_ocurrencies(char* str, char c) {
	int i;

	for (i = 0; str[i] != '\0'; str[i] == c ? i++ : *str++);

	return i;
}
