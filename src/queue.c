/*
 * queue.c
 *
 *  Created on: Nov 1, 2014
 */

#include "../inc/queue.h"

queue_t* queue_create() {
	queue_t* queue = (queue_t*) malloc(sizeof(queue_t));

    queue->begin = NULL;
    queue->end = NULL;
    queue->count = 0;

    return queue;
}

node_t* get_last_element_from_queue(queue_t **queue) {
    if (*queue && (*queue)->begin && (*queue)->count > 0) {
        queue_node_t *p = (*queue)->end;
        node_t* n = (*queue)->end->node;
        (*queue)->count--;
        if ((*queue)->count == 1) {
        	(*queue)->end = (*queue)->begin;
        	(*queue)->begin->next = NULL;
        	(*queue)->begin->previous = NULL;
        } else {
        	if ((*queue)->count == 0) {
        		(*queue) = queue_create();
        	} else {
        		(*queue)->end = (*queue)->end->previous;
        		(*queue)->end->next = NULL;
			}
        }

        free(p);

        return n;
    } else {
    	return NULL;
    }
}

void insert_node_queue(queue_t **queue, node_t *z) {
    if (*queue && (*queue)->begin && z) {
        queue_node_t *p = (*queue)->begin;
        while (p && z->frequency < (p->node)->frequency) {
        	if (p->next != NULL)
        		p = p->next;
        	else
        		break;
        }
        queue_node_t *aux = (queue_node_t*) malloc (sizeof(queue_node_t));
        aux->node = z;
        if (p->next == NULL) {
            aux->previous = (*queue)->end;
            aux->next = NULL;
            (*queue)->end = aux;
            p->next = aux;
        } else {
        	if (p != (*queue)->begin) {
				p->previous->next = aux;
				aux->previous = p->previous;
				aux->next = p;
				p->previous = aux;
			} else {
				aux->next = p;
				aux->previous = NULL;
				p->previous = aux;
				(*queue)->begin = aux;
			}
        }
    } else {
    	if (*queue && z) {
			queue_node_t *p = (queue_node_t*) malloc (sizeof(queue_node_t));
			p->node = z;
			p->next = NULL;
			p->previous = NULL;
			(*queue)->begin = p;
			(*queue)->end = p;
		} else {
			return;
		}
    }
    (*queue)->count++;
    return;
}
