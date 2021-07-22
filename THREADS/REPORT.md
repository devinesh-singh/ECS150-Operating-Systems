# Project 2 Report
Queue.c: 
  - we implemented the queue using a doubly linked list to allow operations in O(1) and to make the iterate function easier to implement so we can use the previous node to change prev->next->next easily.
  - the node struct contains information regarding the data, while the queue struct only has data about the front node, read node, and length of the queue (incremented after create)
  1. New Node: initialize its data to the value of k, while the next and previous are NULL, enqueue will manage its next and previous pointers 
  2. Queue Create: initialize next and previous to NULL (empty queue), set the length to 0
  2. queue_destory: check if its empty: if so, we can free the queue, else error. there must not be any nodes in the queue
  3. queue_enqueue: if queue doesn't exist, error. otherwise we would set it as the rear node of the queue by setting queue->rear->next = temp, and queue->rear = temp. increment length of queue ( to be returned by queue_length)
  4. queue_dequeue: set queue->front to temp, the original front's next node becomes the new front and free. decrement length of queue
  5. queue_delete: iterate through different cases while the node isn't NULL:
  - first node: must change the pointers to the original front's next node and set its prevous to NULL
  - last node: change its previous node to the queue's rear node (make the second to last node the last node and change its previous to the previous of it)
  - otherwise: change both the next and previous of an internal node
 
  Test Cases:
- tested each function individually with a simple test case (corner cases: empty queue, 1 element in queue)
- test_queue_simple: queue and deque a single node (test if it works on a single element)
- test_delete: also tests a single node
- queue_destory: create and destroy the queue (empty queue)
- test_null_data_destroy: check if it fails to destroy nothing
- test_queue_empty_deque: check if it fails on an empty queue
- test_non-empty_queue_destory: it must return an error since data and node exists. 
- test_queue_empty_dequeue: must return an error since there are no nodes in the queue
- test_empty_queue_length: make sure its length is 0
- test_queue_length: check to see if it returns 1 when we enqueue and 0 once we dequeue.
- test_iterator: taken from the project description. slightly modified. creates an array of 10 elements, enqueues them all. we want to iterate to check if each data value is incremented by 1 and confirm location 0 has value 2. 
- confirm location 3 has data value 5 :)
---------------------------------------
thread.c:
 - we created an enum that stores the possible states of a thread
 - we created a struct as the TCB which contains the state, tid, args, function, and stack
 - uthread_init: we need to create a thread if one doesn't already exist. 
 - 
 1. uthread_join: first check if the tid is 0 or is itself - if so, error
    disable preemption, because you need to have preemption to join
    iterate through the ready threads to find a thread ready to be run. if thread is in zombie state, then it must yield until another thread joins to collect its data. once collected, destroy its contents
2. uthread_search_thread: a helper function that finds a thread based on the tid
3. uthread_exit: set the state of the thread to zombie, put in queue, and yield until another thread acquires it return value
4. uthread_yield: set the running thread state to ready. then enqueue the thread to be scheduled again. set the next thread in the queue and run it using a context switch
5. uthread_create: if you're trying to create a thread that has already been created, error. (use the helper function uthread_init).otherwise increment tid and initialize all the thread values. then set it in the queue.

Test Cases:
---------------------------------------
preempt.c:
1. preempt_handler: forces the thread to yield the current thread
2. preempt_enable: enable signals
3. preempt_disable: disable signals
4. preempt_start:initialize signals, add signals, 
------
Resources:
https://www.geeksforgeeks.org/queue-linked-list-implementation/
https://www.edureka.co/blog/thread-yield-in-java/
https://stackoverflow.com/questions/53405013/how-does-thread-join-work-conceptually
https://www.google.com/search?client=safari&rls=en&biw=1440&bih=837&sxsrf=ACYBGNRUGkpfVzbQzNUc1pYKniweU3NIcA%3A1572411783880&ei=hxm5Xa2rNZXB7gL87rb4Bw&q=preempt+thread+implement&oq=preempt+thread+imp&gs_l=psy-ab.1.0.33i160l2.2593.8163..9563...1.4..0.99.1519.19......0....1..gws-wiz.......0i71j35i39j0i67j0i131j0j0i20i263j0i22i30.2ak7EMu-Vro
