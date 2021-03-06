/*
 * queue
 *
 *  Created on: Nov 1, 2014
 */

#ifndef INC_QUEUE_H_
#define INC_QUEUE_H_

#include "tree.h"

typedef struct block {
    node_t* node;
    struct block *next, *previous;
} queue_node_t;

typedef struct {
    uint32_t count;
    queue_node_t *begin, *end;
} queue_t;

queue_t* queue_create();
node_t* get_last_element_from_queue(queue_t **queue);
void insert_node_queue(queue_t **queue, node_t *z);

#endif /* INC_QUEUE_H_ */
