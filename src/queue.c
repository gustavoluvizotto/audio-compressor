/*
 * queue.c
 *
 *  Created on: Nov 1, 2014
 *      Author: gustavo
 */

#include "../inc/queue.h"

void queue_create(queue_t *queue) {
    queue->begin = NULL;
    queue->end = NULL;
    queue->count = 0;
}

node_t* get_last_element_from_queue(queue_t *queue) {
    if (queue && queue->begin && queue->count > 0) {
        queue_node_t *p = queue->end;
        node_t* n = queue->end->node;
		queue->count--;
        if (queue->count == 1) {
        	queue->end = queue->begin;
        	queue->begin->next = NULL;
        	queue->begin->previous = NULL;
        } else {
        	if (queue->count == 0) {
        		queue_create(queue);
        	} else {
				queue->end = queue->end->previous;
				queue->end->next = NULL;
			}
        }

        free(p);

        return n;
    } else {
    	return NULL;
    }
}

void insert_node_queue(queue_t *queue, node_t *n) {
    if (queue && queue->begin && n) {
        queue_node_t *p = queue->begin;
        while (p && n->frequency < p->node->frequency)
			p = p->next;
        queue_node_t *aux = (queue_node_t*) malloc (sizeof(queue_node_t));
        aux->node = n;
        if (p == NULL) {
            aux->previous = queue->end;
            aux->next = NULL;
            queue->end = aux;
        } else {
        	if (p != queue->begin) {
				p->previous->next = aux;
				aux->previous = p->previous;
				aux->next = p;
				p->previous = aux;
			} else {
				aux->next = p;
				aux->previous = NULL;
				p->previous = aux;
				queue->begin = aux;
			}
        }
    } else {
    	if (queue && n) {
			queue_node_t *p = (queue_node_t*) malloc (sizeof(queue_node_t));
			p->node = n;
			p->next = NULL;
			p->previous = NULL;
			queue->begin = p;
			queue->end = p;
		} else {
			return;
		}
    }
    queue->count++;
}
