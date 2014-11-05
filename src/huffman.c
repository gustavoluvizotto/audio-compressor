/*
 * huffman.c
 *
 *  Created on: Nov 1, 2014
 *      Author: gustavo
 */

#include "../inc/huffman.h"

node_t* huffman(uint32_t *frequency) {
    int i;														/* index */
    char *aux = (char*) malloc (sizeof(char) * MAXSIZE + 1);	/* intermediare sample */
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
        	sprintf(aux, "/%d", i);
        	strncat(aux, "\0", sizeof(char));
            z = create_node_tree();
            z->sample = (char*) malloc (sizeof(char) * 5);
            sprintf(z->sample, "%s", aux);
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
		x = get_last_element_from_queue(&queue);
        y = get_last_element_from_queue(&queue);
        strcpy(aux, x->sample);
        strcat(aux, y->sample);
        z = create_node_tree();
        z->sample = (char*) malloc (sizeof(char) * (strlen(aux) + 1));
        strcpy(z->sample, aux);
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
	uint32_t *frequency;					/* frequency of given data[i] */

	frequency = (uint32_t*) malloc (MAX_SAMPLE * sizeof(uint32_t));
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
result_t write_huffman(node_t *root, uint8_t *data, uint32_t *frequency, char *out_file) {
	unsigned char c;					/* Used to store each byte to be written on file */
	char* code;							/* Huffman code */
	size_t i, j;						/* Indexes */
	size_t size = sizeof(data);			/* Size of the samples */
	int bits = 0;						/* Used to control the number of bits in byte c */
	char** binaries;					/* Samples in binaries */
	result_t result;					/* Return result */

	code = (char*) malloc(MAX_HUFF_CODE * sizeof(char));

	binaries = (char**) malloc(size * sizeof(char*));

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
	for (i = 0; i < size; i++) {
		if (frequency[i] == 0) {
			fwrite(&frequency[i], sizeof(uint32_t), 1, fp);
			fwrite(&i, sizeof(size_t), 1, fp);
		}
	}

	for (i = 0; i < size; i++) {
		binaries[i] = byte_to_binary(data[i]);
	}

	/* For each sample, write the Huffman code and sample itself */
	c = 0;
	for(i = 0; i < size; i++) {
		strncpy(code, "\0", sizeof(char));
		code = get_code(root, data[i]); /* Obtain the huffman code for data[i] */
		/* Write the huffman code in char c. Shift every character of code in bits of c */
		for(j = 0; j < strlen(code) && j != '\0'; j++) {
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
		/* Write the sample in binary mode, like in the huffman code */
		for(j = 0; j < data[i]; j++) {
			if(binaries[i][j] == '1') {
				c <<= 1;
				c += 1;
			} else
				c <<= 1;
			bits++;
			if(bits == 8) {
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
		fwrite(&bits, sizeof(unsigned char), 1, fp); 	/* write the number of bits of the last c byte without stuffing 0 */
	}

	/* free memory and close file */
	for (i = 0; i < size; i++) {
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
char* get_code (node_t* p, uint8_t data) {
	char* code = NULL;
	char* string_data = (char*) malloc(sizeof(char) * 5);

	sprintf(string_data, "/%d", data);
	strncat(string_data, "\0", sizeof(char));

	huffman_code(p, string_data, code);

	return code;
}

/*
 * Recursive function that get the huffman code of a data and put into code.
 * Runs the huffman tree, add (strcat) "0" for left branch and "1" to right branch.
 */
void huffman_code (node_t* p, char* data, char* code) {
    if (p == NULL || code == NULL) {
    	return;
    }
    if (strstr(p->sample, data) == NULL) {
    	return;
    }
    if (strlen(p->sample) <= 5) {
    	return;
    } else {
        if (strstr(p->left->sample, data)) {
            strcat(code, "0");
            huffman_code(p->left, data, code);
        } else if (strstr(p->right->sample, data)) {
            strcat(code, "1");
            huffman_code(p->right, data, code);
        }
    }
}
