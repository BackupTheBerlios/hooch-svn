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
 * \file addrbook.c
 * Address book manipulation functions
 */

#include <stdlib.h>
#include <camille/addrbook.h>

addrbook_t * const ERROR_ADDRBOOK = (void *)error_dummy_func;

/* XXX There's prolly a cleaner way to do this */
/* Parser data */
addrbook curr_addrbook;
contact curr_contact;
extern FILE *yyin;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Parse an address book file.
 *
 * \param fp  The file to parse.
 *
 * \return  The file's contents, as addrbook, or ERROR_ADDRBOOK if the file
 *	     could not be parsed.
 */
addrbook
addrbook_parse_file(FILE *fp)
{
	yyin = fp;
	curr_addrbook = addrbook_create();
	yyparse();
	return curr_addrbook;
}


/**
 * Create a new, empty, addressbook
 *
 * \return  The newly created addressbook, or ERROR_ADDRBOOK if out of memory.
 *
 * \sa addrbook_destroy
 */
addrbook
addrbook_create(void)
{
	addrbook_t *ab;

	if ((ab = malloc(sizeof(addrbook_t))) == NULL)
		return ERROR_ADDRBOOK;

	if ((ab->contacts = ht_create(CONTACTS_RANGE, str_hash))
	    == ERROR_HT) {
		free(ab);
		return ERROR_ADDRBOOK;
	}

	if ((ab->groups = ht_create(GROUPS_RANGE, str_hash))
	    == ERROR_HT) {
		ht_destroy(ab->contacts, NULL, NULL);
		free(ab);
		return ERROR_ADDRBOOK;
	}

	if ((ab->defaults = alist_create()) == ERROR_ALIST) {
		ht_destroy(ab->groups, NULL, NULL);
		ht_destroy(ab->contacts, NULL, NULL);
		free(ab);
		return ERROR_ADDRBOOK;
	}

	return (addrbook)ab;
}


/**
 * Free all memory allocated for an addressbook and all contacts, groups and
 * defaults associated with it.
 *
 * \param ab  The addressbook to destroy.
 *
 * \sa addrbook_create
 */
void
addrbook_destroy(addrbook ab)
{
	/* Casting to free_func is ok here */
	/* 
	 * We don't free the hash keys because these ptrs are copies
	 * of the `name' entries in the contacts/groups.  Their _destroy
	 * funcs handle that for us.
	 */
	ht_destroy(ab->contacts, NULL, (free_func)contact_destroy);
	ht_destroy(ab->groups, NULL, (free_func)group_destroy);
	alist_destroy(ab->defaults, NULL, NULL);
	free(ab);
}


/**
 * Add a contact to an addressbook, or update an existing contact.
 *
 * \param ab  The addressbook to add the contact to.
 * \param ct  The contact to add to the addressbook.
 *
 * \return  The addressbook
 *
 * \sa addrbook_add_group, addrbook_del_contact
 */
addrbook
addrbook_add_contact(addrbook ab, contact ct)
{
	/* XXX a better name is addrbook_insert_contact??? */
	/*
	 * Give this some thought.  Note that if we do this, we should also
	 * actually change addrbook_del_contact into addrbook_delete_contact
	 * This is more consistent with some Gune functions (ht/alist, but
	 * not, for example, queue), but do you really ``insert''
	 * something into an addressbook?
	 */
	gendata key, value;

	key.ptr = contact_name(ct);
	value.ptr = ct;

	/* Hmm, this key/value gendata nonsense should be easier */
	ht_insert(ab->contacts, key, value, str_eq, (free_func)contact_destroy);

	return ab;
}


/**
 * Delete a contact from an addressbook.
 *
 * \param ab    The addressbook to delete the contact from.
 * \param name  The name of the contact to delete.
 *
 * \return  The addressbook
 *
 * \sa addrbook_del_group, addrbook_add_contact
 */
addrbook
addrbook_del_contact(addrbook ab, char *name)
{
	gendata key;
	key.ptr = name;

	ht_delete(ab->contacts, key, str_eq, NULL, (free_func)contact_destroy);

	return ab;
}


/**
 * Add a group to an addressbook, or update an existing group.
 *
 * \param ab  The addressbook to add the group to.
 * \param gr  The group to add to the addressbook.
 *
 * \return  The addressbook
 *
 * \sa addrbook_add_contact, addrbook_del_group
 */
addrbook
addrbook_add_group(addrbook ab, group gr)
{
	/* XXX a better name is addrbook_insert_group??? */
	gendata key, value;

	key.ptr = group_name(gr);
	value.ptr = gr;

	/* Hmm, this key/value gendata nonsense should be easier */
	ht_insert(ab->groups, key, value, str_eq, (free_func)group_destroy);

	return ab;
}


/**
 * Delete a group from an addressbook.
 *
 * \param ab    The addressbook to delete the group from.
 * \param name  The name of the group to delete.
 *
 * \return  The addressbook
 *
 * \sa addrbook_del_contact, addrbook_add_group
 */
addrbook
addrbook_del_group(addrbook ab, char *name)
{
	gendata key;
	key.ptr = name;

	ht_delete(ab->groups, key, str_eq, NULL, (free_func)group_destroy);

	return ab;
}


#ifdef DEBUG
/**
 * Prints a dump of an addressbook.
 *
 * \param ab  The addressbook to print.
 */
void
addrbook_dump(addrbook ab)
{
	/* Dump contacts, groups etc */
}
#endif /* DEBUG */
