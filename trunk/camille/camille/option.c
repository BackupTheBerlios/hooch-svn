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
 * \brief Option implementation.
 *
 * \file option.c
 */

#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <gune/string.h>
#include <camille/option.h>

static char *option_type_names[NUM_OTYPES + 1] = {
	"option hierarchy",
	"string",
	"boolean",
	"number",
	"empty",
	"unknown"
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * \brief Look up the string name of an option type.
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
 * \brief Create a new option hierarchy.
 *
 * \return  The options hierarchy, or \c NULL if it failed.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa option_hier_destroy
 */
option_hier
option_hier_create(void)
{
	alist a;

	if ((a = alist_create()) == NULL)
		return NULL;

	return (option_hier)a;
}


/**
 * \brief Destroy an option hierarchy.
 *
 * \attention
 * Be sure to destroy all bindings to the options in the hierarchy first.
 * If you try to reference the option a binding belongs to, you'll
 * get into serious trouble since the address where the option once was
 * stored is now invalid.
 *
 * \param h  The hierarchy to destroy.
 *
 * \sa option_hier_create
 */
void
option_hier_destroy(option_hier h)
{
	assert(h != NULL);

	alist_destroy((alist)h, NULL, (free_func)option_destroy);
}


/**
 * \brief Insert an option into a hierarchy.
 *
 * \param h  The hierarchy under which to directly insert the option.
 * \param o  The option to insert.
 *
 * \return  The option hierarchy, or \c NULL if an error occurred.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 * - \b EINVAL if there already is an option with the given name at this
 *              level in the hierarchy.
 *
 * \sa option_hier_lookup
 */
option_hier
option_hier_insert(option_hier h, option o)
{
	alist a = (alist)h;
	gendata key, val;

	assert(h != NULL);
	assert(o != NULL);
	assert(o->name != NULL);

	key.ptr = o->name;
	val.ptr = o;

	if ((a = alist_insert_uniq(a, key, val, str_eq)) == NULL)
		return NULL;

	return h;
}


/**
 * \brief Get an option from a hierarchy by name.
 *
 * \param h     The hierarchy in which the option must be located.
 * \param name  The name of the option to return.
 *
 * \return   The requested option, or \c NULL if the option could not be found.
 *
 * \par Errno values:
 * - \b EINVAL if the option could not be found.
 *
 * \sa option_hier_insert
 */
option
option_hier_lookup(option_hier h, char *name)
{
	gendata key, val;

	assert(h != NULL);
	assert(name != NULL);

	key.ptr = name;

	if (alist_lookup((alist)h, key, str_eq, &val) == NULL)
		return NULL;

	return (option)val.ptr;
}


/**
 * \brief Create a new option.
 *
 * \param name  The (symbolic, identifier) name of the option.  This will get
 *		  copied, so you can free the original or pass \c const strings.
 * \param type  The type of option.
 * \param data  Either the value of the option or, if \p type is
 *		  \c OTYPE_HIER, the hierarchy under this.  If \p type is
 *		  \c OTYPE_STRING, the data gets copied, so you can free the
 *		  original input or pass const strings.
 *
 * \return  The created option, or \c NULL in case of error.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 * - \b EINVAL if \p type has invalid value.
 *
 * \sa option_destroy
 */
option
option_create(char *name, option_type type, option_data data)
{
	option_t *o;
	assert (name != NULL);

	if ((o = malloc(sizeof(option_t))) == NULL)
		return NULL;

	switch(type) {
		case OTYPE_STRING:
			o->data.def.ptr = str_cpy(data.def.ptr);
			break;
		case OTYPE_BOOL:
			/* FALLTHROUGH */
		case OTYPE_NUMBER:
			/* FALLTHROUGH */
		case OTYPE_HIER:
			o->data = data;
			break;
		default:
			free(o);
			errno = EINVAL;
			return NULL;
	}

	o->name = str_cpy(name);
	o->type = type;

	return o;
}


/**
 * \brief Destroy an option.
 *
 * \attention
 * Be sure to destroy all bindings to the option first.
 * If you try to reference the option a binding belongs to, you'll
 * get into serious trouble since the address where the option once was
 * stored is now invalid.
 *
 * \param o  The option to destroy.
 *
 * \sa option_create
 */
void
option_destroy(option o)
{
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
 * \brief Get an option's type.
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
 * \brief Get an option's default value.
 *
 * \param opt  The option to get the default value of.
 *
 * \return  The option's value.  (needs to be interpreted according to its type)
 *
 * \sa option_set_default
 */
gendata
option_get_default(option opt)
{
	return opt->data.def;
}


/**
 * \brief Set an option's default value.
 *
 * If the option is of type \c OTYPE_HIER, the operation is invalid and
 * will not do anything.  If the option is of type \c OTYPE_STRING, the
 * string will be copied.
 *
 * \param opt  The option to set the default value of.
 * \param def  The new default value of the option.
 *
 * \sa option_get_default
 */
void
option_set_default(option opt, gendata def)
{
	assert(opt != NULL);

	switch (opt->type) {
		case OTYPE_HIER:
			/* Do nothing */
			break;
		case OTYPE_STRING:
			opt->data.def.ptr = str_cpy(def.ptr);
			break;
		case OTYPE_BOOL:
			/* FALLTHROUGH */
		case OTYPE_NUMBER:
			opt->data.def = def;
			break;
		default:
			/* Do nothing in case of error */
			return;
	}
}
