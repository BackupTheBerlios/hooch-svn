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
 */
addrbook
addrbook_create(void)
{
	addrbook_t *ab;

	if ((ab = malloc(sizeof(addrbook_t))) == NULL)
		return ERROR_ADDRBOOK;

	if ((ab->contacts = ht_create(CONTACTS_RANGE, str_hash, str_eq))
	    == ERROR_HT) {
		free(ab);
		return ERROR_ADDRBOOK;
	}

	if ((ab->groups = ht_create(GROUPS_RANGE, str_hash, str_eq))
	    == ERROR_HT) {
		ht_destroy(ab->contacts, NULL, NULL);
		free(ab);
		return ERROR_ADDRBOOK;
	}

	if ((ab->defaults = alist_create(str_eq)) == ERROR_ALIST) {
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
 */
void
addrbook_destroy(addrbook ab)
{
	/* XXX TODO FIXME Change NULL parameters into contact_destroy etc */
	ht_destroy(ab->contacts, NULL, NULL);
	ht_destroy(ab->groups, NULL, NULL);
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
 */
addrbook
addrbook_add_contact(addrbook ab, contact ct)
{
	return ab;
}
