/*
 * $Id: contacts.c 106 2004-10-15 15:16:14Z sjamaan $
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
 * \file contact_id.c
 * Contact id manipulation functions
 */

#include <assert.h>
#include <stdlib.h>
#include <gune/error.h>
#include <gune/string.h>
#include <camille/contact_id.h>
#include <camille/option.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Create a contact id with a given name, initialising all data to the defaults.
 *
 * \param name  The (symbolic, identifier) name of the contact id.  This will
 *		  get copied, so you can free the original or pass const
 *		  strings.
 * \param bl    The initial bindings for the id.
 *
 * \return  The new contact id, or ERROR_PTR if there was an error.
 *	      errno = ENOMEM if out of memory.
 *
 * \sa contact_id_destroy
 */
contact_id
contact_id_create(const char *name, bind_list bl)
{
	contact_id_t *id;

	assert(name != NULL);

	if ((id = malloc(sizeof(contact_id_t))) == NULL)
		return ERROR_PTR;

	if ((id->name = str_cpy(name)) == NULL) {
		free(id);
		return ERROR_PTR;
	}

	id->bindings = bl;

	return (contact_id)id;
}


/**
 * Destroy a contact id.
 *
 * \param id  The contact id to destroy.
 *
 * \sa contact_id_create
 */
void
contact_id_destroy(contact_id id)
{
	assert(id != ERROR_PTR);
	assert(id != NULL);
	bind_list_destroy(id->bindings);
	free(id->name);
	free(id);
}


/**
 * Retrieve the name of a contact id.
 *
 * \param id  The contact id to get the name of.
 */
/*
 * XXX Actually we would like this to return a const ptr... We can't unless we
 *  we want to cast around while using the name as hash key.
 */
char *
contact_id_get_name(contact_id id)
{
	assert(id != ERROR_PTR);
	assert(id != NULL);
	return id->name;
}


/**
 * Retrieve the bindings of a contact id.  This may be modified directly.
 *
 * \param id  The contact id to get the bindings of.
 */
bind_list
contact_id_get_bindings(contact_id id)
{
	assert(id != ERROR_PTR);
	assert(id != NULL);
	return id->bindings;
}


#ifdef DEBUG
/**
 * Prints a dump of a contact id.
 *
 * \param id  The contact id to print.
 */
void
contact_id_dump(contact_id id)
{
	assert(id != ERROR_PTR);
	assert(id != NULL);
	printf("Id %s:\n", id->name);
	bind_list_dump(id->bindings);
}
#endif /* DEBUG */
