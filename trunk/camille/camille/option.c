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
 * \file option.c
 * Option storage
 */

#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <gune/error.h>
#include <gune/misc.h>
#include <gune/string.h>
#include <camille/option.h>

option_t * const ERROR_OPTION = (void *)error_dummy_func;
opt_inst_t * const ERROR_OPT_INST = (void *)error_dummy_func;
bind_list_t * const ERROR_BIND_LIST = (void *)error_dummy_func;
option_hier_t * const ERROR_OPTION_HIER = (void *)error_dummy_func;

static char *option_type_names[NUM_OTYPES + 1] = {
	"option hierarchy",
	"string",
	"boolean",
	"number",
	"unknown"
};

#ifdef DEBUG
void opt_inst_walk(gendata *, gendata *);
void opt_inst_dump(opt_inst);
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Lookup the string name of an option type.
 *
 * \param t  The option type to look up.
 *
 * \return  The option type's name.
 */
char *
option_type_name(option_type t)
{
	if (t > NUM_OTYPES)
		t = NUM_OTYPES;

	return option_type_names[t];
}


/**
 * Create a new option hierarchy.
 *
 * \return  The options hierarchy, or ERROR_OPTION_HIER if it failed.
 *	      errno = ENOMEM if out of memory.
 *
 * \sa option_hier_destroy
 */
option_hier
option_hier_create(void)
{
	alist a;

	if ((a = alist_create()) == ERROR_ALIST)
		return ERROR_OPTION_HIER;

	return (option_hier)a;
}


/**
 * Destroy an option hierarchy.
 *
 * \param h  The hierarchy to destroy.
 *
 * \sa option_hier_create
 */
void
option_hier_destroy(option_hier h)
{
	assert(h != ERROR_OPTION_HIER);

	alist_destroy((alist)h, NULL, (free_func)option_destroy);
}


/**
 * Insert option into hierarchy.
 *
 * \param h  The hierarchy under which to directly insert the option.
 * \param o  The option to insert.
 *
 * \return  The option hierarchy, or ERROR_OPTION_HIER if an error occurred.
 *	      errno = ENOMEM if out of memory.
 *	      errno = EINVAL if the option already exists at this place in the
 *				hierarchy.
 *
 * \sa option_hier_lookup
 */
option_hier
option_hier_insert(option_hier h, option o)
{
	alist a = (alist)h;
	gendata key, val;

	assert(h != ERROR_OPTION_HIER);
	assert(o != ERROR_OPTION);
	assert(o != NULL);
	assert(o->name != NULL);

	key.ptr = o->name;
	val.ptr = o;

	if ((a = alist_insert_uniq(a, key, val, str_eq)) == ERROR_ALIST)
		return ERROR_OPTION_HIER;

	return h;
}


/**
 * Get an option from a hierarchy by name.
 *
 * \param h     The hierarchy in which the option must be located.
 * \param name  The name of the option to return.
 *
 * \return   The requested option, or ERROR_OPTION if the option could not
 *	       be found.
 *	        errno = EINVAL if the option could not be found.
 *
 * \sa option_hier_insert
 */
option
option_hier_lookup(option_hier h, char *name)
{
	gendata key, val;

	assert(h != ERROR_OPTION_HIER);

	key.ptr = name;

	if (alist_lookup((alist)h, key, str_eq, &val) == ERROR_ALIST)
		return ERROR_OPTION;

	return (option)val.ptr;
}


/**
 * Create a new option.
 *
 * \param name  The (symbolic, identifier) name of the option.  This will get
 *		  copied, so you can free the original or pass const strings.
 * \param type  The type of option.
 * \param data  Either the value of the option or, if the type is
 *		  OTYPE_HIER, the hierarchy under this.  If the type is
 *		  OTYPE_STRING, the data gets copied, so you can free the
 *		  original input or pass const strings.
 *
 * \return  The created option, or ERROR_OPTION in case of error.
 *	      errno = ENOMEM if out of memory.
 *
 * \sa option_destroy
 */
option
option_create(char *name, option_type type, option_data data)
{
	option_t *o;
	assert (name != NULL);

	if ((o = malloc(sizeof(option_t))) == NULL)
		return ERROR_OPTION;

	o->name = str_cpy(name);
	o->type = type;

	if (type == OTYPE_STRING)
		o->data.def.ptr = str_cpy(data.def.ptr);
	else
		o->data = data;

	return o;
}


/**
 * Destroy an option.
 *
 * \param o  The option to destroy.
 *
 * \sa option_create
 */
void
option_destroy(option o)
{
	assert(o != ERROR_OPTION);
	assert(o != NULL);

	free(o->name);
	switch (o->type) {
		case OTYPE_STRING:
			free(o->data.def.ptr);
			break;
		case OTYPE_HIER:
			option_hier_destroy(o->data.def.ptr);
			break;
		default:
			/* Do nothing */
			break;
	}
	free(o);
}


/**
 * Get an option's type.
 *
 * \param opt  The option to get the type of.
 *
 * \return  The option's type.
 */
option_type
option_get_type(option opt)
{
	return opt->type;
}


/**
 * Instantiate a certain option.
 *
 * \param opt    The option to instantiate.
 * \param t      The requested type of the option.
 * \param empty  Whether the option is empty (nonzero) or not (zero).
 * \param value  The value of the option (ignored if empty != 0).
 *
 * \return  The instantiation, or ERROR_OPT_INST in case of error.
 *	      errno = ENOMEM if out of memory.
 *	      errno = EINVAL if option type does not match requested type.
 *
 * \sa opt_inst_destroy
 */
opt_inst
opt_inst_create(option opt, option_type t, int empty, gendata value)
{
	opt_inst_t *inst;

	assert(opt != ERROR_OPTION);
	assert(opt != NULL);

	if (t != opt->type) {
		errno = EINVAL;
		return ERROR_OPT_INST;
	}

	if ((inst = malloc(sizeof(opt_inst_t))) == NULL)
		return ERROR_OPT_INST;

	inst->empty = empty;
	inst->value = value;

	return (opt_inst)inst;
}


/**
 * Destroy an instantiation of an option.
 *
 * \param inst  The instantiation to destroy.
 *
 * \sa opt_inst_create
 */
void
opt_inst_destroy(opt_inst inst)
{
	free(inst);
}


/**
 * Create a binding list for option instantiations.
 *
 * \return  The created binding list, or ERROR_BIND_LIST in case of error.
 *	      errno = ENOMEM if out of memory.
 *
 * \sa bind_list_destroy
 */
bind_list
bind_list_create(void)
{
	alist al;

	if ((al = alist_create()) == ERROR_ALIST)
		return ERROR_BIND_LIST;

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
	alist_destroy((alist)bl, NULL, (free_func)opt_inst_destroy);
}


/**
 * Bind an option instantiation to its value in a binding list.
 *
 * \param bl    Binding list in which to insert instantiation.
 * \param inst  Instantiation.
 *
 * \return  The new binding list, or ERROR_BIND_LIST if an error occurred.
 *	      errno = ENOMEM if out of memory.
 */
bind_list
opt_inst_bind(bind_list bl, opt_inst inst)
{
	alist al;
	gendata key, value;

	assert(bl != ERROR_BIND_LIST);
	assert(inst != ERROR_OPT_INST);

	key.ptr = inst->option;
	value.ptr = inst;

	if ((al = alist_insert((alist)bl, key, value, ptr_eq,
				(free_func)opt_inst_destroy)) == ERROR_ALIST)
		return ERROR_BIND_LIST;

	return (bind_list)al;
}

/* TODO opt_inst_unbind */

#ifdef DEBUG
/**
 * Prints a dump of a bind list
 *
 * \param  The bind list to print.
 */
void
bind_list_dump(bind_list bl)
{
	alist_walk((alist)bl, opt_inst_walk);
}


/*
 * Call opt_inst dumping function for all opt_insts in the bindings list.
 */
void
opt_inst_walk(gendata *key, gendata *value)
{
	opt_inst_dump((opt_inst)value->ptr);
}


/*
 * Dump an option instantiation.
 */
void
opt_inst_dump(opt_inst inst)
{
	printf("%s = ", inst->option->name);
	switch (inst->option->type) {
		case OTYPE_BOOL:
			if (inst->value.num == 0)
				printf("false");
			else
				printf("true");
			break;
		case OTYPE_STRING:
			printf("'%s'", (char *)inst->value.ptr);
			break;
		case OTYPE_NUMBER:
			printf("%i", inst->value.num);
			break;
		default:
			printf("unknown");
	}
	printf("\n");
}
#endif
