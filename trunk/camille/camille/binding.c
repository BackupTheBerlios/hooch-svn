/*
 * $Id: contact.c 122 2004-10-24 17:25:52Z sjamaan $
 *
 * Copyright (c) 2004 Peter Bex and Vincent Driessen
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
 * \brief Option bindings implementation.
 *
 * \file binding.c
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <gune/error.h>
#include <gune/misc.h>
#include <camille/binding.h>

typedef struct {
	bind_walk_func func;
	gendata userdata;
} bind_walk_struct;

#ifdef DEBUG
static void binding_walk(gendata *, gendata *, gendata);
static void binding_dump(binding);
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * \brief Create a binding for an option.
 *
 * \param opt    The option to bind.
 * \param t      The requested type of the option.
 * \param value  The value of the option (ignored if \p t == \c OTYPE_EMPTY).
 *
 * \return  The binding, or \c NULL in case of error.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 * - \b EINVAL if option type does not match requested type.
 *
 * \sa binding_destroy
 */
binding
binding_create(option opt, option_type t, gendata value)
{
	binding_t *bnd;

	assert(opt != NULL);

	if (t != opt->type && t != OTYPE_EMPTY) {
		errno = EINVAL;
		return NULL;
	}

	if ((bnd = malloc(sizeof(binding_t))) == NULL)
		return NULL;

	bnd->option = opt;
	bnd->empty = (t == OTYPE_EMPTY);

	if (t != OTYPE_EMPTY)
		bnd->value = value;

	return (binding)bnd;
}


/**
 * \brief Destroy a binding.
 *
 * If the binding's option's type is \c OTYPE_STRING and the binding is
 * not empty, the binding's value is freed.
 *
 * \param bnd  The binding to destroy.
 *
 * \sa binding_create
 */
void
binding_destroy(binding bnd)
{
	if (bnd->option->type == OTYPE_STRING && !bnd->empty)
		free(bnd->value.ptr);
	free(bnd);
}


/**
 * \brief Get the value of a binding.
 *
 * \attention
 * This function logs an error at WARN_ERROR level if the binding is
 * empty (which will exit the application).
 * Therefore, always check with the binding_empty() function whether
 * the binding is empty or not.
 *
 * \param bnd  The binding of which to get the value.
 *
 * \return  The value of the binding.
 *
 * \sa binding_empty
 */
gendata
binding_get_value(binding bnd)
{
	assert(bnd != NULL);

	if (bnd->empty)
		log_entry(WARN_ERROR, "Cannot read value of an empty binding.");

	return bnd->value;
}


/**
 * \brief Check if a binding is empty.
 *
 * \param bnd  The binding to check for empty.
 *
 * \return  Zero if the binding is not empty, nonzero if it is empty.
 *
 * \sa binding_get_value
 */
int
binding_empty(binding bnd)
{
	assert(bnd != NULL);
	return bnd->empty;
}


/**
 * \brief Get the option of a binding.
 *
 * \param bnd  The binding to get the option of.
 *
 * \return  The option the binding applies to.
 */
option
binding_get_option(binding bnd)
{
	assert(bnd != NULL);
	return bnd->option;
}


/**
 * Create a binding list.
 *
 * \return  The created binding list, or \c NULL in case of error.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa bind_list_destroy
 */
bind_list
bind_list_create(void)
{
	alist al;

	if ((al = alist_create()) == NULL)
		return NULL;

	return (bind_list)al;
}


/**
 * \brief Check if a binding list is empty.
 *
 * \param bl  The binding list to check for emptiness.
 *
 * \return  0 if not empty, nonzero if empty.
 */
int
bind_list_empty(bind_list bl)
{
	return alist_empty((alist)bl);
}


/**
 * \brief Insert a binding in a binding list.
 *
 * Overwrites the binding if it already was present.
 *
 * \param bl   The binding list in which to insert the binding.
 * \param bnd  The binding to insert.
 *
 * \return  The original bind_list, or \c NULL if the data could not be
 *            inserted.  Original bind_list is still valid in case of error.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa bind_list_insert_uniq
 */
bind_list
bind_list_insert(bind_list bl, binding bnd)
{
	gendata akey, avalue;
	alist al;

	assert(bl != NULL);
	assert(bnd != NULL);

	akey.ptr = bnd->option;
	avalue.ptr = bnd;

	if ((al = alist_insert((alist)bl, akey, avalue, ptr_eq, NULL,
				(free_func)binding_destroy)) == NULL)
		return NULL;

	return (bind_list)al;
}


/**
 * \brief Insert a binding in a binding list (no replace).
 *
 * If the variable is already bound (in the supplied list), it is an error.
 *
 * \param bl   The binding list in which to insert the binding.
 * \param bnd  The binding to insert.
 *
 * \return  The original bind_list, or \c NULL if the data could not be
 *            inserted.  Original bind_list is still valid in case of error.
 *
 * \par Errno values:
 * - \b EINVAL if the variable is already bound.
 * - \b ENOMEM if out of memory.
 *
 * \sa bind_list_insert
 */
bind_list
bind_list_insert_uniq(bind_list bl, binding bnd)
{
	gendata akey, avalue;
	alist al;

	assert(bl != NULL);
	assert(bnd != NULL);

	akey.ptr = bnd->option;
	avalue.ptr = bnd;

	if ((al = alist_insert_uniq((alist)bl, akey, avalue, ptr_eq)) == NULL)
		return NULL;

	return (bind_list)al;
}


/**
 * \brief Destroy a binding list and every binding in it.
 *
 * \param bl  The binding list to destroy.
 *
 * \sa bind_list_create
 */
void
bind_list_destroy(bind_list bl)
{
	alist_destroy((alist)bl, NULL, (free_func)binding_destroy);
}


/*
 * Simple stub which calls user-supplied binding walker.  Internal use only.
 */
static void
bind_walk(gendata *key, gendata *value, gendata walk)
/* ARGSUSED */
{
	bind_walk_struct *data = walk.ptr;

	data->func((binding)value->ptr, data->userdata);
}


/**
 * \brief Walk a binding list.
 *
 * Walk over all bindings in a binding list, calling a user-specified
 * function on each binding.
 *
 * \attention
 * While using this function, it is not allowed to remove entries other than
 * the current entry.  It is allowed to change the contents of the binding.
 *
 * \param bl    The binding list to walk.
 * \param walk  The function which will process the bindings.
 * \param data  Any data you wish to have passed to the walk function.
 */
void
bind_list_walk(bind_list bl, bind_walk_func walk, gendata data)
{
	gendata ourdata;
	bind_walk_struct func_data;

	func_data.func = walk;
	func_data.userdata = data;

	ourdata.ptr = &func_data;
	alist_walk((alist)bl, bind_walk, ourdata);
}


/**
 * \brief Bind an option to a value and insert it into a binding list.
 *
 * This is a quick way to create a binding on-the-fly while insert it
 * into a list.
 *
 * \param bl     Binding list in which to insert the new binding.
 * \param opt    The option to bind.
 * \param t      The requested type of the option.
 * \param value  The value of the option (ignored if \p t == \c OTYPE_EMPTY)
 *
 * \return  The new bindings list, or \c NULL in case of error.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 * - \b EINVAL if option type does not match requested type.
 */
bind_list
option_bind(bind_list bl, option opt, option_type t, gendata value)
{
	binding bnd;

	assert(bl != NULL);

	if ((bnd = binding_create(opt, t, value)) == NULL)
		return NULL;

	if ((bl = bind_list_insert_uniq(bl, bnd)) == NULL)
		binding_destroy(bnd);

	return bl;
}


/**
 * \brief Unbind an option.
 *
 * \param bl   The binding list in which to look.
 * \param opt  The option to unbind.
 *
 * \return  The binding list.
 */
bind_list
option_unbind(bind_list bl, option opt)
{
	gendata key;

	key.ptr = opt;

	bl = (bind_list)alist_delete((alist)bl, key, ptr_eq, NULL,
				      (free_func)binding_destroy);

	return bl;
}


/**
 * \brief Merge two bind lists (no replace).
 *
 * If the variable is already bound (in the supplied list), it is an error.
 *
 * \note
 * This function is \f$ O(n \cdot m) \f$ with \f$ n \f$ the length of
 * \p base and \f$ m \f$ the length of \p rest.
 *
 * \param base	      The binding list to insert the data in.
 * \param rest        The binding list to be merged into \p base.
 *
 * \return  The \p base bind_list, merged with \p rest, or NULL in case of
 *	     error.  If an error occurred, the \p base list is still valid, but
 *           it is undefined which items from the \p rest list will have been
 *           merged into the list and which haven't.
 *	    The \p rest bind_list will have been modified so it still contains
 *	     the entries which had matching options in the \p base bind_list.
 *	    The \p rest bind_list will thus still be valid.
 */
bind_list
bind_list_merge_uniq(bind_list base, bind_list rest)
{
	return (bind_list)alist_merge_uniq((alist)base, (alist)rest, ptr_eq);
}


/**
 * \brief Merge two bind lists.
 *
 * \note
 * This function is \f$ O(n \cdot m) \f$ with \f$ n \f$ the length of
 * \p base and \f$ m \f$ the length of \p rest.
 *
 * \param base	      The binding list to insert the data in.
 * \param rest        The binding list to be merged into \p base.
 *
 * \return  The \p base bind_list, merged with \p rest, or NULL in case of
 *	     error.  If an error occurred, the \p base list is still valid, but
 *           it is undefined which items from the \p rest list will have been
 *           merged into the list and which haven't.  The \p rest list will
 *	     not be valid after the function has finished.
 */
bind_list
bind_list_merge(bind_list base, bind_list rest)
{
	return (bind_list)alist_merge((alist)base, (alist)rest, ptr_eq, NULL,
					(free_func)binding_destroy);
}


#ifdef DEBUG
/**
 * \brief Print a dump of a bind list.
 *
 * \param  The bind list to print.
 */
void
bind_list_dump(bind_list bl)
{
	gendata dummy_data;
	dummy_data.num = 0;
	alist_walk((alist)bl, binding_walk, dummy_data);
}


/*
 * Call binding dumping function for all bindings in the bindings list.
 */
static void
binding_walk(gendata *key, gendata *value, gendata data)
/* ARGSUSED */
{
	binding_dump((binding)value->ptr);
}


/*
 * Dump a binding.
 */
static void
binding_dump(binding bnd)
{
	gendata data = bnd->value;

	printf("%s = ", bnd->option->name);
	if (bnd->empty) {
		printf("(empty), default = ");
		data = bnd->option->data.def;
	}
	switch (bnd->option->type) {
		case OTYPE_BOOL:
			if (data.num == 0)
				printf("false");
			else
				printf("true");
			break;
		case OTYPE_STRING:
			printf("'%s'", (char *)data.ptr);
			break;
		case OTYPE_NUMBER:
			printf("%i", data.num);
			break;
		default:
			printf("unknown");
	}
	printf("\n");
}
#endif
