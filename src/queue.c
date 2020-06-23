/*
* A dynamic queue. Each element (node) in the list holds up to one
* void pointer. The queue is FIFO - first element to be enqueued in the queue
* will be the first to dequeue.
*
* Author: Buster Hultgren Wärn <dv17bhn@cs.umu.se>
*
* Final build: 2018-10-26
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "queue.h"
#include "saferMemHandler.h"

struct node {

	void *value;
	node *next;
};

struct queue {

	int size;
	node *first;
	node *last;
};

/*
* description: Creates and allocates memory for an empty queue.
* return: The queue.
*/
queue *queueEmpty (void) {

	queue *q = smalloc(sizeof(*q));
	q -> first = NULL;
	q -> last = NULL;
	q -> size = 0;
	return q;
}

/*
* description: Checks if queue contains any elements.
* param[in]: q - The queue.
* return: If true; 1, else 0.
*/
int queueIsEmpty (queue *q) {

	return q -> size == 0;
}

/*
* description: Gets the number of elements in quue.
* param[in]: q - The queue.
* return: The number of elements in the queue.
*/
int queueGetSize (queue *q) {

	return q -> size;
}

/*
* description: Gets the value of the first element in the queue.
* param[in]: q - The queue.
* return: Void pointer to the value in the queue.
*/
void *queueFront (queue *q) {

	if (q -> first != NULL) {

		return q -> first -> value;
	}
	return NULL;
}

/*
* description: Adds an element to the last place in the queue.
* param[in]: q - The queue.
* param[in]: value - Void pointer to the value the element will hold.
*/
void queueEnqueue (queue *q, void *value) {

	if (q -> first == NULL) {

		q -> first = smalloc(sizeof(node));
		q -> first -> next = NULL;
		q -> last = q -> first;
	} else {

		q -> last -> next = smalloc(sizeof(node));
		q -> last = q -> last -> next;
		q -> last -> next = NULL;
	}

	q -> last -> value = value;
	q -> size++;
}

/*
* description: Removes the first element in the queue, letting the next element
* become the first. Will NOT free any memory.
* param[in]: q - The queue.
*/
void queueDequeue (queue *q) {

	if (q -> first != NULL) {

		if (q -> size == 1) {

			sfree(q -> first);
			q -> first = NULL;
			q -> last = NULL;
		} else {

			node *tempNode = q -> first -> next;

			sfree(q -> first);
			q -> first = tempNode;
		}
		q -> size--;
	}
}

/*
* description: Frees all memory allocated by the queue, inluding the queue. The
* value will NOT be free'd.
* param[in]: q - The queue
*/
void queueKill (queue *q) {

	while (q -> first != NULL) {

		node *pos = q -> first -> next;
		q -> first = q -> first -> next;
		sfree(pos);
	}
	sfree(q);
}
