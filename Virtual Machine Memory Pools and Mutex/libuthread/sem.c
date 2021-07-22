#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "thread.h"

TCBRef_t current = NULL;
queue_t ready, blocked;

typedef enum
{
	TH_STATE_RUNNING,
	TH_STATE_READY,
	TH_STATE_BLOCKED,
	TH_STATE_ZOMBIE
} ThreadState_t;

struct semaphore {
	/* TODO Phase 1 */
	int count;
};

sem_t sem_create(size_t count)
{
	/* TODO Phase 1 */
	count = 0;
}

int sem_destroy(sem_t sem)
{
	/* TODO Phase 1 */
	// sem->blocked > 0 is wrong
	if (sem == NULL || sem->blocked > 0)
		return -1;
	
	free(sem);
	return 0;
}

int sem_down(sem_t sem)
{
	/* TODO Phase 1 */
	blocked = queue_create();
	// if the count is 0, push it into a queue of waiting semaphores
	if(count == 0){
		// push into a queue of waiting threads (found in queue.c) and put in a blocked state (proj 2)
		queue_enqueue(blocked, (void)* current);
	}
	count--;
}

int sem_up(sem_t sem)
{
	/* TODO Phase 1 */
	// check if there are waiting threads (wake it up if there is a waiting thread)
	// else increment count;
	count++;
}

int sem_getvalue(sem_t sem, int *sval)
{
	/* TODO Phase 1 */
}

