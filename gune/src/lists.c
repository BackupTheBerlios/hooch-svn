/*
 * $Id$
 *
 * Copyright (c) 2003 Peter Bex and Vincent Driessen
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the names of Peter Bex or Vincent Driessen nor the names of any
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PETER BEX AND VINCENT DRIESSEN AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file lists.c
 * Linked list, queue and stack implementation
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "lists.h"
#include "types.h"
#include "error.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Creates a new empty singly linked list.
 *
 * \return  The pointer to the new empty singly linked list.
 *
 * \sa sll_destroy dll_create
 */
sll
sll_create(void)
{
	return NULL;
}


/**
 * Destroy a singly linked list by deleting each element.
 *
 * \param ll  The list to destroy.
 *
 * \sa  sll_create dll_destroy
 */
void
sll_destroy(sll ll)
{
	while (!sll_is_empty(ll))
		ll = sll_remove_head(ll);
}


/**
 * Returns the number of elements in the given singly linked list.
 *
 * \param ll The singly linked list to get the number of elements from.
 *
 * \return  The number of elements as an unsigned int.
 *
 * \sa  dll_count
 */
unsigned int
sll_count(sll ll)
{
	int count;
	for (count = 0; ll != NULL; ++count, ll = ll->next)
		continue;
	return count;
}


/**
 * Returns whether or not the given linked list is an empty one.
 *
 * \param ll  The singly linked list to check.
 *
 * \return  A boolean indicating whether the linked list is empty.
 *
 * \sa  dll_is_empty
 */
bool
sll_is_empty(sll ll)
{
	return ll == NULL;
}


/**
 * Removes the head from a singly linked list.
 *
 * \param ll  The singly linked list.
 *
 * \return  A pointer to the new singly linked list.
 *
 * \sa  dll_remove
 */
sll
sll_remove_head(sll ll)
{
	sll begin = ll;

	ll = ll->next;
	free(begin);
	return ll;
}


/**
 * Prepends the given element to the head of the singly linked list.
 *
 * \param ll    The singly linked list to prepend the element to.
 * \param data  The element to prepend.
 *
 * \return  The new linked list or NULL in case of error.  Old linked list
 *          has become invalid, unless an error occurred.
 *
 * \sa  sll_append dll_prepend_head
 */
sll
sll_prepend_head(sll ll, void *data)
{
	sll_elem *new;

	if ((new = malloc(sizeof(sll_elem))) == NULL)
		return NULL;
	new->data = data;
	new->next = ll;
	return (sll)new;
}


/**
 * Check if an iterator points to the end of the singly linked list.
 *
 * \param li  The iterator to check
 *
 * \return  A boolean indicating whether the iterator points to the end.
 */
bool
sll_eof(sll_iter li)
{
	return li == NULL;
}


/**
 * Get an iterator which points to the nth element of a linked list.
 *
 * \param ll  The singly linked list for which to get the nth element element.
 * \param n   The number of the element, starting at 0.
 *
 * \return  An iterator which points to the last element.
 *
 * \sa  dll_at sll_next sll_prev sll_first dll_last
 */
sll_iter
sll_at(sll ll, unsigned int n)
{
#ifdef BOUNDS_CHECKING
	if (ll == NULL) {
		fprintf(stderr, "Gune: sll_at: Attempting to get iterator "
			"on empty list\n");
		return NULL;
	}
#endif

	for (; n > 0; --n) {
#ifdef BOUNDS_CHECKING
		if (ll->next == NULL) {
			fprintf(stderr, "Gune: sll_at: Out of bounds: %u\n", n);
			return NULL;
		}
#endif
		ll = ll->next;
	}

	return (sll_iter)ll;
}



/**
 * Get an iterator which points to the first element of a singly linked list.
 *
 * \param ll    The singly linked list for which to get first element.
 *
 * \return  An iterator which points to the first element.
 *
 * \sa sll_first dll_first
 */
sll_iter
sll_first(sll ll)
{
	return (sll_iter)ll;
}


/**
 * Get an iterator which points to the last element of a singly linked list.
 *
 * \param ll    The singly linked list for which to get last element.
 *
 * \return  An iterator which points to the last element.
 *
 * \sa sll_last dll_first
 */
sll_iter
sll_last(sll ll)
{
#ifdef BOUNDS_CHECKING
	if (ll == NULL) {
		fprintf(stderr, "Gune: sll_last: Attempting to get iterator "
			"on empty list\n");
		return NULL;
	}
#endif

	while (ll->next != NULL)
		ll = ll->next;

	return (sll_iter)ll;
}


/**
 * Get an iterator which points to the next element of a singly linked list.
 *
 * \param li  The iterator to use for next element.
 *
 * \return  An iterator which points to the next element.
 *
 * \sa dll_next
 */
sll_iter
sll_next(sll_iter li)
{
#ifdef BOUNDS_CHECKING
	if (li == NULL) {
		fprintf(stderr, "Gune: sll_next: Attempting to get iterator "
			"beyond last iterator.\n");
		return NULL;
	}
#endif

	return (sll_iter)li->next;
}


/**
 * Appends an element with the given data at the position after the current
 *  iterator.
 *
 * \param li    The iterator pointing to the current element.
 * \param data  A pointer to the element to insert.
 *
 * \return A pointer to an iterator which points to the new element, or NULL
 *          if out of memory.
 *
 * \sa dll_append
 */
sll_iter
sll_append(sll_iter li, void *data)
{
	sll_elem *new;

#ifdef BOUNDS_CHECKING
	if (li == NULL) {
		fprintf(stderr, "Gune: sll_append: Attempting to append "
			"after a NULL iterator\n");
		return NULL;
	}
#endif

	if ((new = malloc(sizeof(sll_elem))) == NULL)
		return NULL;
	new->data = data;
	new->next = li->next;
	li->next = new;
	return (sll_iter)new;
}


/**
 * Removes the next element from a singly linked list.
 *
 * \param li  The iterator pointing to the current element, from which the
 *             next element will be selected.
 *
 * \sa dll_remove_next
 */
void
sll_remove_next(sll_iter li)
{
	sll_elem *old;

#ifdef BOUNDS_CHECKING
	/* XXX Check if li->next happens to be NULL? */
	if (li == NULL) {
		fprintf(stderr, "Gune: sll_remove_next: Attempting to remove "
			"after a NULL iterator\n");
		return;
	}
#endif

	old = li->next;

	if (li->next != NULL) {
		li->next = old->next;
		free(old);
	}
}


/**
 * Gets the element at the given index from the singly linked list.
 *
 * \param ll     The singly linked list to look in.
 * \param index  The index of the element to get.
 *
 * \param A pointer to the element.
 *
 * \sa dll_getelem
 */
void *
sll_getelem(sll ll, unsigned int index)
{
	int i;

#ifdef BOUNDS_CHECKING
	if (index >= sll_count(ll)) {
		printf("Gune (in sll_getelem): Index %d out of bounds.\n",
		       index);
		exit(1);
	}
#endif

	for (i = 0; i < index; ++i)
		ll = ll->next;
	return ll->data;
}


#ifdef DEBUG
/**
 * Prints a dump of a singly linked list.
 * The element data is formatted according to the supplied format string.  This
 * function is intended for testing and debug purposes only.
 *
 * \param ll   The singly linked list to print.
 * \param fmt  The format string in printf(3) format.
 *
 * \sa dll_dump
 */
void
sll_dump(sll ll, char *fmt)
{
	for (; ll != NULL; ll = ll->next) {
		printf(fmt, (int)ll->data);
		printf(" -> ");
	}
	printf("NULL\n");
}
#endif



/**
 * Creates a new empty doubly linked list.
 *
 * \return  The pointer to the new empty doubly linked list.
 *
 * \sa dll_destroy sll_create
 */
dll
dll_create(void)
{
	return NULL;
}


/**
 * Destroy a doubly linked list by deleting each element.
 *
 * \param ll  The list to destroy.
 *
 * \sa  dll_create sll_destroy
 */
void
dll_destroy(dll ll)
{
	while (!dll_is_empty(ll))
		ll = dll_remove_head(ll);
}


/**
 * Returns the number of elements in the given doubly linked list.
 *
 * \param ll The doubly linked list to get the number of elements from.
 *
 * \return  The number of elements as an unsigned int.
 *
 * \sa  sll_count
 */
unsigned int
dll_count(dll ll)
{
	int count;
	for (count = 0; ll != NULL; ++count, ll = ll->next)
		continue;
	return count;
}


/**
 * Returns whether or not the given linked list is an empty one.
 *
 * \param ll  The doubly linked list to check.
 *
 * \return  A boolean indicating whether the linked list is empty.
 *
 * \sa  sll_is_empty
 */
bool
dll_is_empty(dll ll)
{
	return ll == NULL;
}


/**
 * Removes the head from a doubly linked list.
 *
 * \param ll  The doubly linked list.
 *
 * \return  A pointer to the new doubly linked list.
 *
 * \sa  sll_remove
 */
dll
dll_remove_head(dll ll)
{
	dll begin = ll;

	ll = ll->next;
	ll->prev = NULL;
	free(begin);
	return ll;
}


/**
 * Prepends the given element to the head of the doubly linked list.
 *
 * \param ll    The doubly linked list to prepend the element to.
 * \param data  The element to prepend.
 *
 * \return  The new linked list or NULL in case of error.  Old linked list
 *          has become invalid, unless an error occurred.
 *
 * \sa  dll_append sll_prepend_head
 */
dll
dll_prepend_head(dll ll, void *data)
{
	dll_elem *new;

	if ((new = malloc(sizeof(dll_elem))) == NULL)
		return NULL;
	new->data = data;
	new->next = ll;
	new->prev = NULL;
	ll->prev = new;
	return (dll)new;
}


/**
 * Check if an iterator points to the end of the doubly linked list.
 *
 * \param li  The iterator to check
 *
 * \return  A boolean indicating whether the iterator points to the end.
 */
bool
dll_eof(dll_iter li)
{
	return li == NULL;
}


/**
 * Get an iterator which points to the nth element of a linked list.
 *
 * \param ll  The doubly linked list for which to get the nth element element.
 * \param n   The number of the element, starting at 0.
 *
 * \return  An iterator which points to the last element.
 *
 * \sa  sll_at dll_next dll_prev dll_first dll_last
 */
dll_iter
dll_at(dll ll, unsigned int n)
{
#ifdef BOUNDS_CHECKING
	if (ll == NULL) {
		fprintf(stderr, "Gune: dll_at: Attempting to get iterator "
			"on empty list\n");
		return NULL;
	}
#endif

	for (; n > 0; --n) {
#ifdef BOUNDS_CHECKING
		if (ll->next == NULL) {
			fprintf(stderr, "Gune: sll_at: Out of bounds: %u\n", n);
			return NULL;
		}
#endif
		ll = ll->next;
	}

	return (dll_iter)ll;
}



/**
 * Get an iterator which points to the first element of a doubly linked list.
 *
 * \param ll    The doubly linked list for which to get first element.
 *
 * \return  An iterator which points to the first element.
 *
 * \sa dll_first sll_first
 */
dll_iter
dll_first(dll ll)
{
	return (dll_iter)ll;
}


/**
 * Get an iterator which points to the last element of a doubly linked list.
 *
 * \param ll    The doubly linked list for which to get last element.
 *
 * \return  An iterator which points to the last element.
 *
 * \sa sll_last dll_first
 */
dll_iter
dll_last(dll ll)
{
#ifdef BOUNDS_CHECKING
	if (ll == NULL) {
		fprintf(stderr, "Gune: dll_last: Attempting to get iterator "
			"on empty list\n");
		return NULL;
	}
#endif

	while (ll->next != NULL)
		ll = ll->next;

	return (dll_iter)ll;
}


/**
 * Get an iterator which points to the next element of a doubly linked list.
 *
 * \param li  The iterator to use for next element.
 *
 * \return  An iterator which points to the next element.
 *
 * \sa dll_next
 */
dll_iter
dll_next(dll_iter li)
{
#ifdef BOUNDS_CHECKING
	if (li == NULL) {
		fprintf(stderr, "Gune: dll_next: Attempting to get iterator "
			"beyond last iterator.\n");
		return NULL;
	}
#endif

	return (dll_iter)li->next;
}


/**
 * Appends an element with the given data at the position after the current
 *  iterator.
 *
 * \param li    The iterator pointing to the current element.
 * \param data  A pointer to the element to insert.
 *
 * \return A pointer to an iterator which points to the new element, or NULL
 *          if out of memory.
 *
 * \sa sll_append
 */
dll_iter
dll_append(dll_iter li, void *data)
{
	dll_elem *new;

#ifdef BOUNDS_CHECKING
	if (li == NULL) {
		fprintf(stderr, "Gune: dll_append: Attempting to append "
			"after a NULL iterator\n");
		return NULL;
	}
#endif

	if ((new = malloc(sizeof(dll_elem))) == NULL)
		return NULL;
	new->data = data;
	new->next = li->next;
	li->next = new;
	new->prev = li;
	if (new->next != NULL)
		new->next->prev = new;
	return (dll_iter)new;
}


/**
 * Removes the next element from a doubly linked list.
 *
 * \param li  The iterator pointing to the current element, from which the
 *             next element will be selected.
 *
 * \sa sll_remove_next
 */
void
dll_remove_next(dll_iter li)
{
	dll_elem *old;

#ifdef BOUNDS_CHECKING
	/* XXX Check if li->next happens to be NULL? */
	if (li == NULL) {
		fprintf(stderr, "Gune: dll_remove_next: Attempting to remove "
			"after a NULL iterator\n");
		return;
	}
#endif

	old = li->next;

	if (li->next != NULL) {
		if (old->next != NULL)
			old->next->prev = li;

		li->next = old->next;
		free(old);
	}
}


/**
 * Gets the element at the given index from the doubly linked list.
 *
 * \param ll     The doubly linked list to look in.
 * \param index  The index of the element to get.
 *
 * \param A pointer to the element.
 *
 * \sa dll_getelem
 */
void *
dll_getelem(dll ll, unsigned int index)
{
	int i;

#ifdef BOUNDS_CHECKING
	if (index >= dll_count(ll)) {
		printf("Gune (in dll_getelem): Index %d out of bounds.\n",
		       index);
		exit(1);
	}
#endif

	for (i = 0; i < index; ++i)
		ll = ll->next;
	return ll->data;
}


#ifdef DEBUG
/**
 * Prints a dump of a doubly linked list.
 * The element data is formatted according to the supplied format string.  This
 * function is intended for testing and debug purposes only.
 *
 * \param ll   The doubly linked list to print.
 * \param fmt  The format string in printf(3) format.
 *
 * \sa sll_dump
 */
void
dll_dump(dll ll, char *fmt)
{
#ifdef BOUNDS_CHECKING
	if (ll->prev != NULL)
		fprintf(stderr, "Gune: dll_dump: First element of doubly "
			"linked list has non-NULL prev pointer\n");
#endif

	for (; ll != NULL; ll = ll->next) {
#ifdef BOUNDS_CHECKING
		if (ll->prev != NULL && ll->prev->next != ll)
			fprintf(stderr, "Gune: dll_dump: Linked list is not "
				"consistent\n");
#endif
		printf(fmt, (int)ll->data);
		printf(" -> ");
	}
	printf("NULL\n");
}
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Create a new, empty, stack and return the pointer to it.
 *
 * \return  The pointer to the new stack object, or NULL if out of memory.
 */
stack
stack_new(void)
{
	stack s;
	
	if ((s = malloc(sizeof(stack_t))) != NULL)
		s->head = NULL;
	return s;
}


/**
 * Pop the top element off the stack and return it.
 *
 * \param s  The stack object to pop the element off.
 *
 * \return   The element that was popped off.
 *
 * \see stack_peek
 */
void *
stack_pop(stack s)
{
	void *res;
	stack_elem *hd;

	assert(s != NULL);

	if (stack_is_empty(s))
		log_entry(WARN_ERROR, "Cannot pop from an empty stack.");

	hd = s->head;
	res = hd->data;
	s->head = hd->next;
	free(hd);
	return res;
}


/**
 * Peek at the top element on the stack and return it, without actually
 * popping it.
 *
 * \param s  The stack object to peek at.
 *
 * \return   The element that is on top of the stack.
 *
 * \see stack_pop
 */
void *
stack_peek(stack s)
{
	/*
	 * NOTE: Should we return NULL?  Pop is an illegal operation on
	 * an empty stack, but peeking isnt (?).
	 * Also, it currently is possible to push NULL pointers as data,
	 * so returning NULL on an empty stack would be ambiguous.
	 */
	assert(s != NULL);

	if (stack_is_empty(s))
		log_entry(WARN_ERROR, "Cannot peek at top element of an"
			   " empty stack.");

	return s->head->data;
}


/**
 * Push the given data onto the given stack s.
 *
 * \param s     The stack object to push onto.
 * \param data  The data to push onto the stack.
 * 
 * \return      The given stack s.
 */
stack
stack_push(stack s, void *data)
{
	stack_elem *el;

	assert(s != NULL);
	
	if ((el = malloc(sizeof(stack_elem))) != NULL) {
		el->data = data;
		el->next = s->head;
		s->head = el;
	}
	return s;
}


/**
 * Check whether the stack is empty.
 *
 * \param s  The stack to check.
 *
 * \return   Whether or not the stack is empty.
 */
bool
stack_is_empty(stack s)
{
	assert(s != NULL);

	return s->head == NULL;
}


/**
 * Free memory allocated by the given stack. Note that the memory allocated
 * by the elements within the stack are NOT freed by this function.
 *
 * \param s  The stack to free.
 */
void
stack_free(stack s)
{
	stack_elem *el;

	assert(s != NULL);

	while (!stack_is_empty(s)) {
		el = s->head;
		s->head = s->head->next;
		free(el);
	}
	free(s);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Create a new queue object and returns a pointer to it.
 *
 * \return  A pointer to the new queue object.
 */
queue
queue_new(void)
{
	queue q;

	q = malloc(sizeof(queue_t));
	q->bgn = NULL;
	q->end = NULL;
	q->count = 0;
	return q;
}


/**
 * Enqueue data in the queue. Data is put as the last element in the row
 * following the FIFO principle.
 *
 * \param q     The given queue.
 * \param data  The data to add to the queue.
 *
 * \return      The pointer to the given queue.
 */
queue
queue_enqueue(queue q, void *data)
{
	queue_elem *el;

	assert(q != NULL);

	el = malloc(sizeof(queue_elem));
	el->data = data;
	/* el->next = NULL <-- not necessary */
	q->end->next = el;
	q->end = el;
	++q->count;

	return q;
}


/**
 * Remove the element at the head of the queue from the queue. This function
 * throws an error if the queue is empty. Therefore, always check with the
 * queue_empty() function to see whether a queue is empty or not.
 *
 * \param q  The queue to dequeue the data from.
 *
 * \return   The data dequeued from the queue.
 *
 * \see queue_empty, queue_peek
 */
void *
queue_dequeue(queue q)
{
	void *res;
	queue_elem *el;

	assert(q != NULL);

	if (queue_is_empty(q))
		log_entry(WARN_ERROR, "Cannot dequeue from an empty queue.");

	el = q->bgn;
	res = el->data;
	q->bgn = q->bgn->next;
	free(el);
	--q->count;

	return res;
}


/**
 * Look at the element at the head of the queue without dequeueing it.
 * This function throws an error if the queue is empty. Therefore, always
 * check with the queue_empty() function to see whether a queue is empty or
 * not.
 *
 * \param q  The queue to peek at.
 *
 * \return   The data at the head of the queue.
 *
 * \see queue_empty, queue_dequeue
 */
void *
queue_peek(queue q)
{
	/*
	 * NOTE: Should we return NULL?  Dequeue is an illegal operation on
	 * an empty queue, but peeking isnt (?).
	 * Also, it currently is possible to enqueue NULL pointers as data,
	 * so returning NULL on an empty queue would be ambiguous.
	 */
	assert(q != NULL);

	if (queue_is_empty(q))
		log_entry(WARN_ERROR, "Cannot peek at the head of an "
			   "empty queue.");

	return q->bgn->data;
}

/**
 * Check whether the queue is empty.
 *
 * \param q  The queue to check for emptiness.
 *
 * \return   Whether the queue is empty.
 */
bool
queue_is_empty(queue q)
{
	assert(q != NULL);

	return q->count == 0;
}


/**
 * Free all memory allocated for the queue. Note that the data stored
 * within the queue is NOT freed.
 *
 * \param q  The queue to free.
 */
void
queue_free(queue q)
{
	queue_elem *el;

	assert(q != NULL);

	while (!queue_is_empty(q)) {
		el = q->bgn;
		q->bgn = q->bgn->next;
		--q->count;
		free(el);
	}
	free(q);
}
