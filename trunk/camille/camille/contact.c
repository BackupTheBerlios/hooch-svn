/*
 * $Id$
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
 * \file contact.c
 * Contact list manipulation functions
 */

#include <stdlib.h>
#include <gune/gune.h>
#include <camille/contact.h>

contact_t * const ERROR_CONTACT = (void *)error_dummy_func;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Create a contact with a given name, initialising all data to the defaults.
 *
 * \param name  The (symbolic, identifier) name of the contact
 *
 * \return  The new contact, or ERROR_CONTACT if there was an error.
 *
 * \sa contact_destroy
 */
contact
contact_create(const char *name)
{
	contact_t *cont;

	if ((cont = malloc(sizeof(contact_t))) == NULL)
		return ERROR_CONTACT;

	cont->name = str_cpy(name);

	if (cont->name == NULL) {
		free(cont);
		return ERROR_CONTACT;
	}

	return (contact)cont;
}


/**
 * Destroy a contact.
 *
 * \param cont  The contact to destroy.
 *
 * \sa contact_create
 */
void
contact_destroy(contact cont)
{
	free(cont->name);
	free(cont);
}


/**
 * Retrieve the name of a contact.
 *
 * \param cont  The contact to get the name of.
 */
/*
 * XXX Actually we would like this to return a const ptr... We can't unless we
 *  we want to cast around while using the name as hash key.
 */
char *
contact_name(contact cont)
{
	return cont->name;
}


#ifdef DEBUG
/**
 * Prints a dump of a contact.
 *
 * \param ct  The contact to print.
 */
void
contact_dump(contact ct)
{
	printf("Contact %s:\n", ct->name);
}
#endif /* DEBUG */


/**
 * Add an id to a contact, or update an existing id.
 *
 * \param ct  The contact to add the id to.
 * \param id  The id to add to the contact.
 *
 * \return  The contact
 *
 * \sa contact_del_id
 */
contact
contact_add_id(contact ct, contact_id id)
{
	/* XXX a better name is contact_insert_id??? */
	gendata key, value;

	key.ptr = contact_id_name(id);
	value.ptr = id;

	/* Hmm, this key/value gendata nonsense should be easier */
	alist_insert(ct->ids, key, value, str_eq,
		     (free_func)contact_id_destroy);

	return ct;
}


/**
 * Delete an id from a contact.
 *
 * \param id    The contact to delete the id from.
 * \param name  The name of the id to delete.
 *
 * \return  The contact
 *
 * \sa contact_add_id
 */
contact
contact_del_id(contact ct, char *name)
{
	gendata key;
	key.ptr = name;

	alist_delete(ct->ids, key, str_eq, NULL, (free_func)contact_id_destroy);

	return ct;
}