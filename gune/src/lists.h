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

/*
 * Lists, queues and stacks
 */
#ifndef GUNE_LISTS_H
#define GUNE_LISTS_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Singly linked list implementation */
typedef struct sll_elem {
	void *data;
	struct sll_elem *next;
} sll_elem;
typedef sll_elem *	sll;
typedef sll_elem *	sll_iter;

/** SLL creation/deletion functions */
sll sll_create(void);
void sll_destroy(sll);
unsigned int sll_count(sll);
bool sll_is_empty(sll);

/** SLL exceptions for head */
sll sll_remove_head(sll);
sll sll_prepend_head(sll, void *);

/** SLL iterator functions */
bool sll_eof(sll_iter);
sll_iter sll_at(sll, unsigned int);
sll_iter sll_first(sll);
sll_iter sll_last(sll);
sll_iter sll_next(sll_iter);
sll_iter sll_append(sll_iter, void *);
void sll_remove_next(sll_iter);
void *sll_get_data(sll_iter);
void sll_set_data(sll_iter);

#ifdef DEBUG
void sll_dump(sll, char *);
#endif

/** Doubly linked list implementation */
typedef struct dll_elem {
	void *data;
	struct dll_elem *prev;
	struct dll_elem *next;
} dll_elem;
typedef dll_elem *	dll;
typedef dll_elem *	dll_iter;

/** DLL creation/deletion functions */
dll dll_create(void);
void dll_destroy(dll);
unsigned int dll_count(dll);
bool dll_is_empty(dll);

/** DLL exceptions for head */
dll dll_remove_head(dll);
dll dll_prepend_head(dll, void *);

/** DLL iterator functions */
bool dll_eof(dll_iter);
dll_iter dll_at(dll, unsigned int);
dll_iter dll_first(dll);
dll_iter dll_last(dll);
dll_iter dll_next(dll_iter);
dll_iter dll_append(dll_iter, void *);
void dll_remove_next(dll_iter);
void *dll_get_data(dll_iter);
void dll_set_data(dll_iter);

#ifdef DEBUG
void dll_dump(dll, char *);
#endif


/** Stack implementation */
typedef struct stack_elem {
	void *data;
	struct stack_elem *next;
} stack_elem;

typedef struct stack_t {
	struct stack_elem *head;
} stack_t, *stack;

stack stack_new(void);
void *stack_pop(stack);
void *stack_peek(stack);
stack stack_push(stack, void *);
bool stack_is_empty(stack);
void stack_free(stack);


/** Queue implementation */
typedef struct queue_elem {
	void *data;
	struct queue_elem *next;
} queue_elem;

typedef struct queue_t {
	struct queue_elem *bgn;
	struct queue_elem *end;
	int count;
} queue_t, *queue;

queue queue_new(void);
queue queue_enqueue(queue, void *);
void *queue_dequeue(queue);
void *queue_peek(queue);
bool queue_is_empty(queue);
void queue_free(queue);

#ifdef __cplusplus
}
#endif

#endif /* GUNE_LISTS_H */
