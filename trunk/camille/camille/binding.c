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
 * \file binding.c
 * Binding of options.
 */

#include <assert.h>
#include <errno.h>
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
 * Create a binding for a certain option.
 *
 * \param opt    The option to bind.
 * \param t      The requested type of the option.
 * \param value  The value of the option (ignored if t == OTYPE_EMPTY).
 *
 * \return  The binding, or ERROR_PTR in case of error.
 *	      errno = ENOMEM if out of memory.
 *	      errno = EINVAL if option type does not match requested type.
 *
 * \sa binding_destroy
 */
binding
binding_create(option opt, option_type t, gendata value)
{
	binding_t *bnd;

	assert(opt != ERROR_PTR);
	assert(opt != NULL);

	if (t != opt->type && t != OTYPE_EMPTY) {
		errno = EINVAL;
		return ERROR_PTR;
	}

	if ((bnd = malloc(sizeof(binding_t))) == NULL)
		return ERROR_PTR;

	bnd->option = opt;
	bnd->empty = (t == OTYPE_EMPTY);

	if (t != OTYPE_EMPTY)
		bnd->value = value;

	return (binding)bnd;
}


/**
 * Destroy a binding of an option.  If the binding's option's type is
 *  OTYPE_STRING and the binding is not empty, the binding's value is freed.
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
 * Get the value of a binding if it's not empty.
 *
 * \param bnd  The binding of which to get the value.
 *
 * \return  The value of the binding.  If the binding is empty, the
 *	     result is undefined.
 *
 * \sa binding_empty
 */
gendata
binding_get_value(binding bnd)
{
	assert(bnd != ERROR_PTR);
	return bnd->value;
}


/**
 * Check if a binding is empty.
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
	assert(bnd != ERROR_PTR);
	return bnd->empty;
}


/**
 * Get the option a binding gives a value.
 *
 * \param bnd  The binding to get the option of.
 *
 * \return  The option the binding applies to.
 */
option
binding_get_option(binding bnd)
{
	assert(bnd != ERROR_PTR);
	return bnd->option;
}


/**
 * Create a binding list.
 *
 * \return  The created binding list, or ERROR_PTR in case of error.
 *	      errno = ENOMEM if out of memory.
 *
 * \sa bind_list_destroy
 */
bind_list
bind_list_create(void)
{
	alist al;

	if ((al = alist_create()) == ERROR_PTR)
		return ERROR_PTR;

	return (bind_list)al;
}


/**
 * Insert a binding in a binding list uniquely.  If the variable is already
 *  bound (in the supplied list), it is an error.
 *
 * \param bl   The binding list in which to insert the binding.
 * \param bnd  The binding to insert.
 *
 * \return  The original bind_list, or ERROR_PTR if the data could not be
 *            inserted.  Original bind_list is still valid in case of error.
 *          errno = EINVAL if the variable is already bound.
 *	    errno = ENOMEM if out of memory.
 *
 *
 * \sa bind_list_insert
 */
bind_list
bind_list_insert_uniq(bind_list bl, binding bnd)
{
	gendata akey, avalue;
	alist al;

	assert(bl != ERROR_PTR);
	assert(bnd != ERROR_PTR);

	akey.ptr = bnd->option;
	avalue.ptr = bnd;

	if ((al = alist_insert_uniq((alist)bl, akey, avalue, ptr_eq))
	     == ERROR_PTR)
		return ERROR_PTR;

	return (bind_list)al;
}


/**
 * Destroy a binding list and every binding in it.
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
 * Walk a binding list, calling a user-specified function on each binding.
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
 * Bind an option to a value and insert it into a binding list.
 *
 * \param bl     Binding list in which to insert the new binding.
 * \param opt    The option to bind.
 * \param t      The requested type of the option.
 * \param value  The value of the option (ignored if t == OTYPE_EMPTY)
 *
 * \return  The new bindings list, or ERROR_PTR in case of error.
 *	      errno = ENOMEM if out of memory.
 *	      errno = EINVAL if option type does not match requested type.
 */
bind_list
option_bind(bind_list bl, option opt, option_type t, gendata value)
{
	binding bnd;

	assert(bl != ERROR_PTR);

	if ((bnd = binding_create(opt, t, value)) == ERROR_PTR)
		return ERROR_PTR;

	if ((bl = bind_list_insert_uniq(bl, bnd)) == ERROR_PTR)
		binding_destroy(bnd);

	return bl;
}


/**
 * Unbind an option.
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


#ifdef DEBUG
/**
 * Prints a dump of a bind list
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
