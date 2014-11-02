/*
 * huffman.c
 *
 *  Created on: Nov 1, 2014
 *      Author: gustavo
 */

#include "../inc/huffman.h"
#include "../inc/common.h"

/*
 * @param data needs to be the sample, difference vector or mdct vector
 */
void comprimir_huffman(uint8_t *data, char **samples) {
	size_t size_data = sizeof(data);		/* total number of samples */
	size_t i;		/* loop indexes */
	char **samples;	/* store the samples in strings. Usefull to count the number of bits */
	uint32_t *frequency;	/* frequency of given data[i] */
	tree_t huffman_tree;

	samples = (char**) malloc (size_data * sizeof(char*));
	for (i = 0; i < size_data; i++) {
		samples[i] = (char*) malloc (MAX_SAMPLE * sizeof(char) + 1);	/* 255 negative numbers + 255 positive numbers  plus 0 */
	}

	frequency = (uint32_t*) malloc (MAX_SAMPLE * sizeof(uint32_t));
	for (i = 0; i < MAX_SAMPLE; i++) {
		frequency[i] = 0;
	}

	/* begin the frequency count */
	for (i = 0; i < size_data; i++) {
		itoa(data[i], samples[i]);
		frequency[data[i]+MAX_SAMPLE/2]++;
	}

	huffman_tree = huffman(frequency);
}

node_t* huffman(uint32_t *frequency) {
    int i;
    char *aux = (char*) malloc (sizeof(char) * MAXSIZE + 1);
    node_t *x, *y, *z;
    queue_t *queue;
    queue = (queue_t*) malloc(sizeof(queue_t));
    queue_create(queue);
    for (i = 1; i <= MAX_SAMPLE; i++) {
        if (frequency[i] > 0) {
        	if(i < 10) {
				strcpy(aux, "/0");
        		itoa(i, aux + 2, 10);
        	} else {
        		strcpy(aux, "/");
        		itoa(i, aux + 1, 10);
        	}
        	strcat(aux, "/");
            z = criaNode();
            z->sample = (char*) malloc (sizeof(char) * 4 + 1); /*  #XX#\0 */
            sprintf(z->sample, "%s", aux);
            z->frequency = frequency[i];
            insert_node_queue(queue, z);
        }
    }
    while (queue->count > 1) {
		x = get_last_element_from_queue(queue);
        y = get_last_element_from_queue(queue);
        strcpy(aux, x->palavra);
        strcat(aux, y->palavra);
        z = insert_node_tree();
        z->palavra = (char*) malloc(sizeof(char) * strlen(aux) + 1);
        strcpy(z->palavra, aux);
        z->frequencia = x->frequencia + y->frequencia;
        z->esquerda = x;
        z->direita = y;
        insert_node_queue(queue, z);
    }

    free(aux);

    return get_last_element_from_queue(queue);
}
