/*
* A dynamic queue. Each element (node) in the list holds up to one
* void pointer. The queue is FIFO - first element to be enqueued in the queue
* will be the first to dequeue.
*
* Author: Buster Hultgren Wärn <dv17bhn@cs.umu.se>
*
* Final build: 2018-10-26
*/

#ifndef __QUEUE__
#define __QUEUE__

typedef struct node node;

typedef struct queue queue;

/*
* description: Creates and allocates memory for an empty queue.
* return: The queue.
*/
queue *queueEmpty (void);

/*
* description: Checks if queue contains any elements.
* param[in]: q - The queue.
* return: If true; 1, else 0.
*/
int queueIsEmpty (queue *q);

/*
* description: Gets the number of elements in quue.
* param[in]: q - The queue.
* return: The number of elements in the queue.
*/
int queueGetSize (queue *q);

/*
* description: Gets the value of the first element in the queue.
* param[in]: q - The queue.
* return: Void pointer to the value in the queue.
*/
void *queueFront (queue *q);

/*
* description: Adds an element to the last place in the queue.
* param[in]: q - The queue.
* param[in]: value - Void pointer to the value the element will hold.
*/
void queueEnqueue (queue *q, void *value);

/*
* description: Removes the first element in the queue, letting the next element
* become the first. Will NOT free any memory.
* param[in]: q - The queue.
*/
void queueDequeue (queue *q);

/*
* description: Frees all memory allocated by the queue, inluding the queue. The
* value will NOT be free'd.
* param[in]: q - The queue
*/
void queueKill (queue *q);


#endif //__QUEUE__
