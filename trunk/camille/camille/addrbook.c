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

#include <assert.h>
#include <stdlib.h>
#include <gune/string.h>
#include <gune/error.h>
#include <camille/addrbook.h>

/* XXX There's prolly a cleaner way to do this */
/* Parser data */
addrbook curr_addrbook;
extern option_hier curr_opthier;
extern FILE *yyin;

extern int yyparse(void);

#ifdef DEBUG
static void contact_walk(gendata *, gendata *, gendata);
static void group_walk(gendata *, gendata *, gendata);
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Parse an address book file.
 *
 * \param fp  The file to parse.
 * \param h   The options hierarchy to use while parsing.
 *
 * \return  The file's contents, as addrbook, or ERROR_PTR if the file
 *	     could not be parsed.
 */
addrbook
addrbook_parse_file(FILE *fp, option_hier h)
{
	yyin = fp;
	curr_addrbook = addrbook_create();
	curr_opthier = h;
	yyparse();
	return curr_addrbook;
}


/**
 * Create a new, empty, addressbook
 *
 * \return  The newly created addressbook, or ERROR_PTR if out of memory.
 *
 * \sa addrbook_destroy
 */
addrbook
addrbook_create(void)
{
	addrbook_t *ab;

	if ((ab = malloc(sizeof(addrbook_t))) == NULL)
		return ERROR_PTR;

	if ((ab->contacts = ht_create(CONTACTS_RANGE, str_hash))
	    == ERROR_PTR) {
		free(ab);
		return ERROR_PTR;
	}

	if ((ab->groups = ht_create(GROUPS_RANGE, str_hash))
	    == ERROR_PTR) {
		ht_destroy(ab->contacts, NULL, NULL);
		free(ab);
		return ERROR_PTR;
	}

	if ((ab->defaults = alist_create()) == ERROR_PTR) {
		ht_destroy(ab->groups, NULL, NULL);
		ht_destroy(ab->contacts, NULL, NULL);
		free(ab);
		return ERROR_PTR;
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
	assert(ab != ERROR_PTR);
	assert(ab != NULL);

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
 * Add a contact to an addressbook.
 *
 * \param ab  The addressbook to add the contact to.
 * \param ct  The contact to add to the addressbook.
 *
 * \return  The addressbook, or ERROR_PTR if an error occurred.
 *	     errno = EINVAL if there already was a contact with the same name
 *		      in the address book.
 *	     errno = ENOMEM if out of memory.
 *
 * \sa addrbook_add_group, addrbook_del_contact
 */
addrbook
addrbook_add_contact(addrbook ab, contact ct)
{
	gendata key, value;

	assert(ab != ERROR_PTR);
	assert(ab != NULL);
	assert(ct != ERROR_PTR);
	assert(ct != NULL);

	key.ptr = contact_get_name(ct);
	value.ptr = ct;

	/* Hmm, this key/value gendata nonsense should be easier */
	if (ht_insert_uniq(ab->contacts, key, value, str_eq) == ERROR_PTR)
		return ERROR_PTR;

	return ab;
}


/**
 * Delete a contact from an addressbook.
 *
 * \param ab    The addressbook to delete the contact from.
 * \param name  The name of the contact to delete.
 *
 * \return  The addressbook, or ERROR_PTR if the contact does not exist.
 *
 * \sa addrbook_del_group, addrbook_add_contact
 */
addrbook
addrbook_del_contact(addrbook ab, char *name)
{
	gendata key;
	key.ptr = name;

	assert(ab != ERROR_PTR);
	assert(ab != NULL);
	assert(name != NULL);

	if (ht_delete(ab->contacts, key, str_eq, NULL,
		      (free_func)contact_destroy) == ERROR_PTR)
		return ERROR_PTR;

	return ab;
}


/**
 * Add a group to an addressbook.
 *
 * \param ab  The addressbook to add the group to.
 * \param gr  The group to add to the addressbook.
 *
 * \return  The addressbook, or ERROR_PTR in case of error.
 *	     errno = EINVAL if there already was a group with the same name
 *		      in the address book.
 *	     errno = ENOMEM if out of memory.
 *
 * \sa addrbook_add_contact, addrbook_del_group
 */
addrbook
addrbook_add_group(addrbook ab, group gr)
{
	gendata key, value;

	assert(ab != ERROR_PTR);
	assert(ab != NULL);
	assert(gr != ERROR_PTR);
	assert(gr != NULL);

	key.ptr = group_get_name(gr);
	value.ptr = gr;

	/* Hmm, this key/value gendata nonsense should be easier */
	if (ht_insert_uniq(ab->groups, key, value, str_eq) == ERROR_PTR)
		return ERROR_PTR;

	return ab;
}


/**
 * Delete a group from an addressbook.
 *
 * \param ab    The addressbook to delete the group from.
 * \param name  The name of the group to delete.
 *
 * \return  The addressbook, or ERROR_PTR if the group does not exist.
 *
 * \sa addrbook_del_contact, addrbook_add_group
 */
addrbook
addrbook_del_group(addrbook ab, char *name)
{
	gendata key;
	key.ptr = name;

	assert(ab != ERROR_PTR);
	assert(ab != NULL);
	assert(name != NULL);

	if (ht_delete(ab->groups, key, str_eq, NULL,
		      (free_func)group_destroy) == ERROR_PTR)
		return ERROR_PTR;

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
	gendata dummy_data;

	assert(ab != ERROR_PTR);
	assert(ab != NULL);

	/* We don't need to pass anything */
	dummy_data.num = 0;

	/* Dump contacts, groups etc */
	ht_walk(ab->contacts, contact_walk, dummy_data);
	ht_walk(ab->groups, group_walk, dummy_data);
}

/*
 * Call contact dumping function for all contacts in the addressbook.
 */
static void
contact_walk(gendata *key, gendata *value, gendata data)
/* ARGSUSED2 */
{
	contact_dump((contact)value->ptr);
}


/*
 * Call group dumping function for all groups in the addressbook.
 */
static void
group_walk(gendata *key, gendata *value, gendata data)
/* ARGSUSED2 */
{
	group_dump((group)value->ptr);
}
#endif /* DEBUG */
