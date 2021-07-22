#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../libuthread/queue.h"

/**
 * Checks if basic queue can be created
 */
void test_create(void)
{
	queue_t q;

	q = queue_create();
	assert(q != NULL);
}

/**
 * Checks basic queue operations of enqueue and dequeue
 */
void test_queue_simple(void)
{
	queue_t q;
	int data = 3, *ptr;

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void **)&ptr);
	assert(ptr == &data);
}

/**
 * Checks deletion of item from queue
 */
void test_delete(void)
{
	queue_t q;
	int data = 3, *ptr;

	q = queue_create();
	queue_enqueue(q, &data);
	queue_delete(q, &data);
	int result = queue_dequeue(q, (void **)&ptr);
	assert(result == 0); // popping data off queue
}

/**
 * Checks destroying of empty queue
 */
void test_queue_destroy(void)
{
	queue_t q = queue_create();
	int result = queue_destroy(q);
	assert(result == 0);
}

/**
 * Checks destroy queue with items inside
 */
void test_non_empty_queue_destroy()
{
	queue_t q;
	int data = 3, *ptr;

	q = queue_create();
	queue_enqueue(q, &data);
	assert(queue_destroy(q) == -1);
}

void test_null_queue_destroy()
{
	assert(queue_destroy(NULL) == -1);
}

/**
 * Checks enqueue of NULL data into queue
 */
void test_null_data_enqueue()
{
	queue_t q;
	q = queue_create();
	assert(queue_enqueue(q, NULL) == -1); // pushing NULL data on queue
}

/**
 * Checks dequeuing on an empty queue
 */
void test_queue_empty_dequeue()
{
	queue_t q;
	int *ptr;
	q = queue_create();
	assert(queue_dequeue(q, (void **)&ptr) == -1);
}

/**
 * Checks enqueueing on null queue
 */
void test_null_queue_enqueue()
{
	int data = 3;
	assert(queue_enqueue(NULL, &data) == -1);
}

void test_null_data_dequeue()
{
	queue_t q = queue_create();
	int data = 3, *ptr;
	queue_enqueue(q, &data);
	int result = queue_dequeue(q, NULL);
	assert(result == -1);
}

/**
 * Checks dequeuing on null queue
 */
void test_null_queue_dequeue()
{
	int *ptr;
	assert(queue_dequeue(NULL, (void **)&ptr) == -1);
}

void test_empty_queue_length()
{
	queue_t q = queue_create();
	assert(queue_length(q) == 0);
}

void test_queue_length()
{
	queue_t q;
	int data = 3, *ptr;

	q = queue_create();
	queue_enqueue(q, &data);
	// check size of queue is 1 after enqueue
	assert(queue_length(q) == 1);
	queue_dequeue(q, (void **)&ptr);
	// check size of queue is 0 after dequeue
	assert(queue_length(q) == 0);
}

/* Callback function that increments items by a certain value */
static int inc_item(void *data, void *arg)
{
	int *a = (int *)data;
	int inc = (int)(long)arg;

	*a += inc;

	return 0;
}

/* Callback function that finds a certain item according to its value */
static int find_item(void *data, void *arg)
{
	int *a = (int *)data;
	int match = (int)(long)arg;

	if (*a == match)
		return 1;

	return 0;
}

void test_iterator(void)
{
	queue_t q;
	int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int i;
	int *ptr;

	/* Initialize the queue and enqueue items */
	q = queue_create();
	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
		queue_enqueue(q, &data[i]);

	/* Add value '1' to every item of the queue */
	queue_iterate(q, inc_item, (void *)1, NULL);
	assert(data[0] == 2);

	/* Find and get the item which is equal to value '5' */
	ptr = NULL;
	queue_iterate(q, find_item, (void *)5, (void **)&ptr);
	assert(ptr != NULL);
	assert(*ptr == 5);
	assert(ptr == &data[3]);
}

int main(void)
{
	test_create();
	test_queue_simple();
	test_delete();
	test_null_data_dequeue();
	test_queue_empty_dequeue();
	test_null_queue_enqueue();
	test_null_queue_dequeue();

	test_empty_queue_length();
	test_queue_length();

	test_queue_destroy();
	test_non_empty_queue_destroy();
	test_null_queue_destroy();

	test_iterator();

	return 0;
}
