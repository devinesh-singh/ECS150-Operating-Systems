// resource:
// http://btechsmartclass.com/data_structures/queue-using-linked-list.html
// http://btechsmartclass.com/data_structures/queue-using-linked-list.html
//https : //web.cs.dal.ca/~jamie//course/CS/3171/Materials/KR_1e/Code/Chapter3/doc/queue-frame.html
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "queue.h"
#include "context.h"

/**
 * Using double linked list structure which keeps track of the front
 * and rear node/entries in the queue as well as previous and next nodes to current.
*/
struct queue
{
    struct Node *front;
    struct Node *rear;
    int length;
};

struct Node
{
    void *data;
    struct Node *prev;
    struct Node *next;
};

/**
 * newNode - Private helper function to create a new node item
 * 
 * create a new node
*/
struct Node *newNode(void *k)
{
    struct Node *temp = (struct Node *)malloc(sizeof(struct Node));
    temp->data = k;
    temp->next = NULL;
    temp->prev = NULL;
    return temp;
}

queue_t queue_create(void)
{
    /* make space for the queue initialize the front and end to NULL
    */
    struct queue *q = (struct queue *)malloc(sizeof(struct queue));
    q->front = q->rear = NULL;
    q->length = 0;
    return q;
}

int queue_destroy(queue_t queue)
{
    // if queue is NULL or not empty
    if (queue == NULL || queue->length > 0)
        return -1;
    free(queue);
    return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
    if (queue == NULL || data == NULL)
        return -1; // queue or data don't exist

    // if the queue is empty, front = rear = temp Node
    struct Node *temp = newNode(data);
    if (queue->length == 0)
    {
        temp->prev = temp->next = NULL;
        queue->front = queue->rear = temp;
        queue->length++;
        return 0;
    }
    // put temp at end of queue and increase length of queue
    queue->length++;
    queue->rear->next = temp;
    temp->prev = queue->rear;
    queue->rear = temp;
    return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
    // If queue is empty/NULL or data are NULL
    if (queue == NULL || data == NULL || queue->length == 0)
        return -1;

    struct Node *temp = queue->front;
    queue->front = temp->next;
    if (queue->front) // values exist in queue, pop front of queue
        queue->front->prev = NULL;
    else // front of queue is NULL, queue is empty
        queue->rear = NULL;

    queue->length--;
    if (data)
        *data = temp->data;
    free(temp);
    return 0;
}

int queue_delete(queue_t queue, void *data)
{
    // if queue or data are NULL
    if (queue->length > 0 || queue == NULL || data == NULL)
        return -1;

    struct Node *current = queue->front;
    while (current != NULL)
    {
        if (current->data == data)
        { // data is the same as one searching for, delete it
            if (queue->length == 1)
            { // making the queue empty
                queue->front = NULL;
                queue->rear = NULL;
            }
            else
            { // move ptrs around current in queue with multiple items
                if (current == queue->front) // deleting first item, need to move front ptr
                {
                    // set front to next in queue, make previous of front NULL
                    queue->front = queue->front->next;
                    queue->front->prev = NULL;
                }
                else if (current == queue->rear) // deleting last item, need to move rear ptr
                {
                    // set rear to prev in queue, make next of rear NULL
                    queue->rear = queue->rear->prev;
                    queue->rear->next = NULL;
                }
                else // item in middle of queue being removed
                {
                    // set previous's next to one after current
                    current->prev->next = current->next;
                    // set next's prev to one before current
                    current->next->prev = current->prev;
                }
            }
            // perform deletion of item from queue and shorten length
            free(current);
            queue->length--;
            return 0;
        }
        current = current->next;
    }
    return -1; // didnt find @data in the queue
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
    if (queue == NULL || func == NULL)
        return -1;

    struct Node *current = queue->front;
    int funcRet = 0;
    while (current != NULL)
    {
        funcRet = func(current->data, arg);
        if (funcRet == 1)
            break;
        current = current->next;
    }
    if (funcRet == 1 && data != NULL)
        *data = current->data;
    return 0;
}

int queue_length(queue_t queue)
{
    if (queue == NULL)
        return -1;
    return queue->length;
}