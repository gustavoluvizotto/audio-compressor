/*
 * huffman.c
 *
 *  Created on: Nov 1, 2014
 *      Author: gustavo
 */

#include "../inc/huffman.h"

node_t* huffman(uint32_t *frequency) {
    int i;													/* index */
    char *aux = (char*) malloc (sizeof(char) * MAXSIZE + 1);	/* intermediare sample */
    node_t *x, *y;												/* min frequencies from queue */
    node_t *z;													/* inserted node */
    queue_t *queue = NULL;												/* queue are the leaves elements of the tree */

    queue_create(queue);

    /*
     * In this step, we insert in the queue every frequency element. The sample element
     * is /SAMPLE/\0 format. Is our standard to store samples in the tree. In the next
     * step it will be usefull to create intermediate nodes in the tree (sum of frequencies
     * in the huffman tree).
     */
    for (i = 0; i <= MAX_SAMPLE; i++) {
        if (frequency[i] > 0) {
        	sprintf(aux, "/%d", i);
        	strncat(aux, "\0", 1);
            z = create_node_tree();
            z->sample = (char*) malloc (sizeof(char) * 4 + 1);
            sprintf(z->sample, "%s", aux);
            z->frequency = frequency[i];
            insert_node_queue(queue, z);
        }
    }

    /*
     * construct the huffman tree bottom up. Get two min frequency elements from queue,
     * concatenate to create a sub-root element and points to the two min frequency.
     * Insert the node (sub-root tree node) in the queue and repeate the process until
     * the last element of the queue.
     */
    while (queue->count > 1) {
		x = get_last_element_from_queue(queue);
        y = get_last_element_from_queue(queue);
        strcpy(aux, x->sample);
        strcat(aux, y->sample);
        z = create_node_tree();
        z->sample = (char*) malloc (sizeof(char) * strlen(aux) + 1);
        strcpy(z->sample, aux);
        z->frequency = x->frequency + y->frequency;
        z->left = x;
        z->right = y;
        insert_node_queue(queue, z);
    }

    free(aux);

    /*
     * last element of a queue is the last element inserted. I.e., the root element of
     * the huffman tree
     */
    return get_last_element_from_queue(queue);
}

/*
 * @param data needs to be the sample, difference vector (num of bits of difference vector)
 * or mdct vector
 */
void comprimir_huffman(uint8_t *data) {
	size_t size_data = sizeof(data);		/* total number of samples */
	size_t i;								/* loop indexes */
	uint32_t *frequency;					/* frequency of given data[i] */
	tree_t *huffman_tree = NULL;			/* huffman tree */

	tree_create(huffman_tree);

	frequency = (uint32_t*) malloc (MAX_SAMPLE * sizeof(uint32_t));
	for (i = 0; i < MAX_SAMPLE; i++) {
		frequency[i] = 0;
	}

	/* begin the frequency count */
	for (i = 0; i < size_data; i++) {
		frequency[data[i]]++;
	}

	huffman_tree->root = huffman(frequency);
}
