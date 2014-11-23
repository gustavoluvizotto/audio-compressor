/*
 * huffman.c
 *
 *  Created on: Nov 1, 2014
 *      Author: gustavo
 *
 *  TODO Write the huffman header (frequencies) in binary mode, i.e., convert the uint16_t frequency type in binary string and discart the first zeros (save only the essential data).
 *  TODO Disalloc the memory in write huffman
 */

#include "../inc/huffman.h"
/*
#undef TRACE
#define TRACE(fmt, ...)
*/

void huffman_table(table_t *table, uint16_t count) {
	uint16_t i;

	table->rows = (row_t*) malloc(count * sizeof(row_t));
	table->lenght = count;
	for (i = 0; i < count; i++) {
		table->rows[i].code = (char*) malloc(MAX_HUFF_CODE * sizeof(char));
		table->rows[i].index = 0;
	}

	return;
}

result_t huffman_decompress(FILE *fp, table_t *table, uint16_t *_frequency, uint32_t num_samples, char** codes) {
	uint32_t count;
	size_t reading;
	size_t i, j = 0;
	uint8_t data;
	result_t result = -ERR_NO;
	char target[MAX_HUFF_CODE];			/* target code to search in the table */
	unsigned char read_byte;			/* bytes read from file */
	tree_t *huffman_tree = NULL;		/* one huffman tree */
	uint8_t bits = 0;					/* number of bits of the last byte */
	uint32_t count_stored_codes;

	huffman_tree = (tree_t*) malloc(sizeof(tree_t));
	tree_create(&huffman_tree);

	reading = fread(&bits, sizeof(uint8_t), 1,fp);
	if (reading != 1) {
		TRACE("[ERROR] Fail to read file -- number of bits of the last character\n");
		result = -ERR_FAIL;
		return result;
	}

	reading = fread(&count_stored_codes, sizeof(uint32_t), 1,fp);
	if (reading != 1) {
		TRACE("[ERROR] Fail to read file -- number of bytes of huffman code stored\n");
		result = -ERR_FAIL;
		return result;
	}

	reading = fread(&count, sizeof(uint32_t), 1, fp);
	if (reading != 1) {
		TRACE("[ERROR] Fail to read file -- frequency counter\n");
		result = -ERR_FAIL;
		return result;
	}

	for (i = 0; i < count; i++) {
		data = 0;
		fread(&data, sizeof(uint8_t), 1, fp);
		fread(&_frequency[data], sizeof(uint16_t), 1, fp);
	}

	huffman_table(table, count);

	huffman_tree->root = huffman(_frequency, MAX_SAMPLE);

	generate_table(huffman_tree->root, table, _frequency);

	/* decode every byte read from data sector in input file */
	memset(target, '\0', MAX_HUFF_CODE);
	while(j < num_samples) {
		/* read file until find a huffman code */
		fread(&read_byte, sizeof(unsigned char), 1, fp);
		for (i = 0; i < 8 && j < num_samples; i++) {
			strncat(target, (read_byte >> (7-i)) & 0x01 ? "1" : "0", sizeof(char));
			if (get_leaf(huffman_tree->root, target) != NULL) {	/* found the huffman code? */
				codes[j] = (char*) malloc((strlen(target) + 1) * sizeof(char));
				memset(codes[j], '\0', (strlen(target) + 1) * sizeof(char));
				strncpy(codes[j], target, strlen(target) * sizeof(char));
				memset(target, '\0', MAX_HUFF_CODE * sizeof(char));
				j++;
			}
		}
	}
	free(huffman_tree);

	return result;
}

int search_tree_by_code(node_t *node, char* code) {
	if (node->left == NULL || node->right == NULL) {
		return TRUE;
	}
	if (code[0] == '\0') {
		return FALSE;
	}
	if (code[0] == '1') {
		return search_tree_by_code(node->right, code+1);
	} else {
		return search_tree_by_code(node->left, code+1);
	}
}

/*
 * Search for code in the table of huffman codes.
 */
int search_code(table_t table, char *code){
	int i;
	uint16_t count = table.lenght;

	for(i = 0; i < count; i++) {
		if(strcmp(table.rows[i].code, code) == 0) {
			return (int)(table.rows[i].index);
		}
	}

	return -1;
}

/*
* Obtain the huffman code and store in the table.
*/
void generate_table(node_t *root, table_t *table, uint16_t *frequency){
	uint8_t i, j = 0;
	
	for(i = 0; i < MAX_SAMPLE; i++){
		if (frequency[i] > 0) {
			(*table).rows[j].index = i;
			memset((*table).rows[j].code, '\0', MAX_HUFF_CODE * sizeof(char));
			(*table).rows[j].code = get_code(root, i);
			j++;
		}
	}
	if (frequency[i] > 0) {
		(*table).rows[j].index = i;
		memset((*table).rows[j].code, '\0', MAX_HUFF_CODE * sizeof(char));
		(*table).rows[j].code = get_code(root, i);
	}
}

node_t* huffman(uint16_t *__frequency, uint8_t bound) {
    int i;														/* index */
    char *aux = (char*) malloc (sizeof(char) * (MAXSIZE + 1));	/* intermediare sample */
    node_t *x, *y;												/* min frequencies from queue */
    node_t *z;													/* inserted node */
    queue_t *queue = queue_create();							/* queue consists of the leaf elements of the tree */

    /*
     * In this step, we insert in the queue every frequency element. The sample element
     * is /SAMPLE/\0 format. Is our standard to store samples in the tree. In the next
     * step it will be usefull to create intermediate nodes in the tree (sum of frequencies
     * in the huffman tree).
     */
    for (i = 0; i <= bound; i++) {
        if (__frequency[i] > 0) {
            memset(aux, '\0', sizeof(char) * (MAXSIZE + 1));
        	sprintf(aux, "/%d/", i);
            z = create_node_tree();
            z->sample = (char*) malloc (sizeof(char) * MAX_LENGHT_SAMPLE);
            memset(z->sample, '\0', sizeof(char) * MAX_LENGHT_SAMPLE);
            strncpy(z->sample, aux, strlen(aux));
            z->frequency = __frequency[i];
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
node_t* huffman_compress(uint8_t *data, uint16_t *_frequency, uint32_t num_samples, uint8_t bound) {
	size_t i;								/* loop indexes */

	/* begin the frequency count */
	for (i = 0; i < num_samples; i++) {
		_frequency[data[i]]++;
	}

	return huffman(_frequency, bound);
}

result_t write_huffman(node_t *root, uint8_t *data, uint16_t *_frequency, char *out_file,  uint32_t num_samples) {
	unsigned char c;					/* Used to store each byte to be written on file */
	char* code = NULL;					/* Huffman code */
	size_t i, j;						/* Indexes */
	uint8_t k;							/* Index to write in the file */
	uint8_t bits = 0;					/* Used to control the number of bits in byte c */
	char** binaries;					/* Samples in binaries */
	result_t result;					/* Return result */
	uint32_t count = 0;					/* Count number of frequencies differents from zero */
	size_t size;
	FILE *fp;
	uint32_t count_stored_codes = 0;

	binaries = (char**) malloc(num_samples * sizeof(char*));

	/* getting the size of the file before writting the huffman header and data. It is
	 * useful to store the number of stuffing bits in the header that has gotten in the
	 * final of write proccess. */
	fp = fopen(out_file, "rb");
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	fclose(fp);

	fp = fopen(out_file, "rb+");
	if (fp == NULL) {
		TRACE("Error opening file.\n");
		fclose(fp);
		result = -ERR_FAIL;
		return result;
	}
	fseek(fp, 0, SEEK_END);

	/* Writing Huffman header */
	fwrite(&bits, sizeof(uint8_t), 1, fp);
	fwrite(&count_stored_codes, sizeof(uint32_t), 1, fp);
	for (k = 0; k < MAX_SAMPLE; k++) {
		if (_frequency[k] > 0) {
			count++;
		}
	}
	if (_frequency[k] > 0) {
		count++;
	}
	fwrite(&count, sizeof(uint32_t), 1, fp);
	for (k = 0; k < MAX_SAMPLE; k++) {
		if (_frequency[k] > 0) {
			fwrite(&k, sizeof(uint8_t), 1, fp);
			fwrite(&_frequency[k], sizeof(uint16_t), 1, fp);
		}
	}
	if (_frequency[k] > 0) {
		fwrite(&k, sizeof(uint8_t), 1, fp);
		fwrite(&_frequency[k], sizeof(uint16_t), 1, fp);
	}

	for (i = 0; i < num_samples; i++) {
		binaries[i] = byte_to_binary(data[i]);
	}

	/* For each sample, write the Huffman code and sample itself */
	c = 0;
	for(i = 0; i < num_samples; i++) {
		code = get_code(root, data[i]); /* Obtain the huffman code for data[i] */
		TRACE("Code: %8s, Datum: %02X\n", code, data[i]);
		/* Write the huffman code in char c. Shift every character of code in bits of c */
		for(j = 0; j < strlen(code); j++) {
			c <<= 1;
			if(code[j] == '1')
				c <<= 1;
			bits++;
			if(bits == 8) {		/* if complete an entire byte, write in the output file */
				fwrite(&c, sizeof(unsigned char), 1, fp);
				count_stored_codes++;
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
		count_stored_codes++;
		fseek(fp, size, SEEK_SET);	 					/* jump and stop on the huffman header (number of bits field) */
		fwrite(&bits, sizeof(uint8_t), 1, fp); 			/* write the number of bits of the last c byte without stuffing 0 */
		fwrite(&count_stored_codes, sizeof(uint32_t), 1, fp);
	}
	/* free memory and close file */
	for (i = 0; i < num_samples; i++) {
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
	sprintf(string_data, "/%d/", data);

	huffman_code(root, string_data, code);

	return code;
}

/*
 * Recursive function that get the huffman code of data and put into code.
 * Runs the huffman tree, add (strcat) "0" for left branch and "1" to right branch.
 */
void huffman_code (node_t* root, char* data, char* code) {
    if (root == NULL || code == NULL) {
    	return;
    }
    if (strstr(root->sample, data) == NULL) {
    	return;
    }
    if (count_ocurrencies(root->sample, '/') == 2) {
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
