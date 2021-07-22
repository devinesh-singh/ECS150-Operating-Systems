#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <assert.h>

#include "context.h"
#include "preempt.h"
#include "queue.h"
#include "uthread.h"

typedef enum
{
	TH_STATE_RUNNING,
	TH_STATE_READY,
	TH_STATE_BLOCKED,
	TH_STATE_ZOMBIE
} ThreadState_t;

typedef struct
{
	uthread_t tid;

	void *args;
	int retval;
	uthread_func_t func;

	void *stack;
	ThreadState_t state;
	uthread_ctx_t ctx;
} TCB_t, *TCBRef_t;

uthread_t thread_count;
TCBRef_t current = NULL;
queue_t ready, zombie;

int uthread_init(void)
{
	if (current == NULL)
	{
		thread_count = 0;
		ready = queue_create();
		zombie = queue_create();

		current = (TCBRef_t)malloc(sizeof(TCB_t));
		if (!current) return -1;
		current->tid = thread_count++;
		current->args = NULL;
		current->func = NULL;
		current->stack = uthread_ctx_alloc_stack();
		current->state = TH_STATE_RUNNING;

		preempt_start();
	}

	return 0;
}

/*
 * uthread_yield - Yield execution
 *
 * This function is to be called from the currently active and running thread in
 * order to yield for other threads to execute.
 */
void uthread_yield(void)
{
	preempt_disable();
	if (current->state == TH_STATE_RUNNING)
		current->state = TH_STATE_READY;

	if (current->state == TH_STATE_READY)
		queue_enqueue(ready, (void*)current);

	if (queue_length(ready) > 0)
	{
		TCBRef_t old_th = current;
		queue_dequeue(ready, (void**)&current);

		uthread_ctx_switch(&old_th->ctx, &current->ctx);
	}

	preempt_enable();
}

/*
 * uthread_self - Get thread identifier
 *
 * Return: The TID of the currently running thread
 */
uthread_t uthread_self(void)
{
	return current->tid;
}

/*
 * uthread_create - Create a new thread
 * @func: Function to be executed by the thread
 * @arg: Argument to be passed to the thread
 *
 * This function creates a new thread running the function @func to which
 * argument @arg is passed, and returns the TID of this new thread.
 *
 * Return: -1 in case of failure (memory allocation, context creation, TID
 * overflow, etc.). The TID of the new thread otherwise.
 */
int uthread_create(uthread_func_t func, void *arg)
{
	if (uthread_init()) return -1;

	TCBRef_t th = (TCBRef_t)malloc(sizeof(TCB_t));
	if (!th) return -1;

	th->tid = thread_count++;
	th->args = arg;
	th->func = func;
	th->stack = uthread_ctx_alloc_stack();
	th->state = TH_STATE_READY;

	preempt_disable();
	if (uthread_ctx_init(&th->ctx, th->stack, func, arg))
	{
		uthread_ctx_destroy_stack(th->stack);
		free(th);
		preempt_enable();
		return -1;
	}

	queue_enqueue(ready, (void*)th);
	preempt_enable();
	return th->tid;
}

/*
 * uthread_exit - Exit from currently running thread
 * @retval: Return value
 *
 * This function is to be called from the currently active and running thread in
 * order to finish its execution. The return value @retval is to be collected
 * from a joining thread.
 *
 * A thread which has not been 'collected' should stay in a zombie state. This
 * means that until collection, the resources associated to a zombie thread
 * should not be freed.
 *
 * This function shall never return.
 */
void uthread_exit(int retval)
{
	preempt_disable();
	current->retval = retval;
	current->state = TH_STATE_ZOMBIE;
	queue_enqueue(zombie, (void*)current);
	preempt_enable();
	uthread_yield();
	while(1);
}

int uthread_search_thread(void *data, void *arg)
{
	TCBRef_t th = (TCBRef_t)data;
	uthread_t tid = *((uthread_t*)arg);

	if (th->tid == tid)
		return 1;

	return 0;
}

/*
 * uthread_join - Join a thread
 * @tid: TID of the thread to join
 * @retval: Address of an integer that will receive the return value
 *
 * This function makes the calling thread wait for the thread @tid to complete
 * and assign the return value of the finished thread to @retval (if @retval is
 * not NULL).
 *
 * A thread can be joined by only one other thread.
 *
 * Return: -1 if @tid is 0 (the 'main' thread cannot be joined), if @tid is the
 * TID of the calling thread, if thread @tid cannot be found, or if thread @tid
 * is already being joined. 0 otherwise.
 */
int uthread_join(uthread_t tid, int *retval)
{
	if (tid == 0)
		return -1;

	if (tid == uthread_self())
		return -1;

	preempt_disable();
	TCBRef_t th = NULL;
	queue_iterate(ready, &uthread_search_thread, (void*)&tid, (void**)&th);
	if (!th) queue_iterate(zombie, &uthread_search_thread, (void*)&tid, (void**)&th);
	if (!th) return -1;
	preempt_enable();

	while(th->state != TH_STATE_ZOMBIE)
		uthread_yield();

	preempt_disable();
	if (retval) *retval = th->retval;
	uthread_ctx_destroy_stack(th->stack);
	free(th);
	preempt_enable();
	return 0;
}
