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

%{

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <gune/string.h>
#include <camille/addrbook.h>
#include <camille/contact.h>
#include <camille/contact_id.h>
#include <camille/group.h>
#include <camille/option.h>

#ifdef DEBUG
#define TRACE printf("Reduce at line %d\n", __LINE__);
#else
#define TRACE
#endif

/* Bool indicating whether we read a defaults block */
static int read_defaults = 0;

typedef struct temp_contact_t {
	bind_list primary_bl;
	alist ids;
} temp_contact;

extern addrbook curr_addrbook;
option_hier curr_opthier;

extern int yylex(void);

extern int lineno;

static addrbook try_add_contact(addrbook, contact);
static addrbook try_add_group(addrbook, group);
static alist try_insert_contact_id(alist, contact_id);
static contact try_create_primary_id(contact, bind_list);
static binding try_bind(option_hier, char *, option_type, gendata);
static bind_list try_insert_binding(bind_list, binding);
static void defaults_walker(binding, gendata);

%}

%union {
	char *identifier;
	char *string;
	int boolean;
	int integer;
	bind_list bindlist;
	binding bnd;
	contact_id id;
	contact cnt;
	temp_contact tmp_cnt;
	group grp;
	addrbook abook;
};

%token <identifier> IDENTIFIER;
%token <string> STRING;
%token <boolean> BOOLEAN;
%token <integer> INTEGER;

%token CONTACT, IDENTITY, DEFAULTS, GROUP;
%token EMPTY;

/* Statement blocks */
%type <bindlist> defaults_body
%type <bindlist> identity_body
%type <bindlist> group_body
%type <bnd> statement
%type <tmp_cnt> contact_body
%type <id> identity
%type <cnt> contact_block
%type <grp> group_block
%type <abook> blocks

/* Field names */
%type <identifier> field

%start start

%%

/*
 * Some conventions:
 * ----------------
 * identifiers are in all lowercase. In case of long names, an underscore is
 * used in order to separate words for readibility, like: long_identifier
 *
 * TOKENS are in all uppercase.
 *
 */

blocks:	contact_block blocks
		{
			$$ = try_add_contact($2, $1);
		}
	| defaults_block blocks
		{
			/* XXX TODO FIXME */
			/* Add a set_defaults func to addrbook.c or soemthing */
			$$ = $2;
		}
	| group_block blocks
		{
			$$ = try_add_group($2, $1);
		}
	| /* Empty */
		{
			$$ = curr_addrbook;
		}
	;

contact_block:
	CONTACT IDENTIFIER '{' contact_body '}'
		{
			$$ = contact_create($2, $4.ids);
			$$ = try_create_primary_id($$, $4.primary_bl);
			free($2);
		}
	;

contact_body:
	identity contact_body
		{
			$$.primary_bl = $2.primary_bl;
			$$.ids = try_insert_contact_id($2.ids, $1);
		}
	| statement contact_body
		{
			$$.primary_bl = try_insert_binding($2.primary_bl, $1);
			$$.ids = $2.ids;
		}
	| /* Empty */
		{
			$$.primary_bl = bind_list_create();
			$$.ids = alist_create();
		}
	;

identity:
	IDENTITY IDENTIFIER '{' identity_body '}'
		{
			$$ = contact_id_create($2);

			contact_id_add_bindings($$, $4);
			free($2);
			/* TODO: Check that this identity has name and address?? */
		}
	;

identity_body:
	statement identity_body
		{
			$$ = try_insert_binding($2, $1);
		}
	| /* Empty */
		{
			$$ = bind_list_create();
		}
	;

group_block:
	GROUP IDENTIFIER '{' group_body '}'
		{
			$$ = group_create($2, $4);
			free($2);
		}
	;

group_body:
	statement group_body
		{
			$$ = try_insert_binding($2, $1);
		}
	| /* Empty */
		{
			$$ = bind_list_create();
		}
	;

defaults_block:
	DEFAULTS '{' defaults_body '}'
		{
			gendata dummy_data;

			if (read_defaults) {
				yyerror("Error: More than one defaults-block"
				         " is not allowed.");
			} else {
				read_defaults = 1;
				dummy_data.num = 0;
				bind_list_walk($3, defaults_walker, dummy_data);
			}
			bind_list_destroy($3);
		}
	;

defaults_body:
	statement defaults_body
		{
			$$ = try_insert_binding($2, $1);
		}
	| /* Empty */
		{
			$$ = bind_list_create();
		}
	;

statement:
	';'
		{
			/*
			 * Just pretend we encountered an error binding.
			 */
			$$ = NULL;
		}
	| field '=' BOOLEAN ';'
		{
			gendata d;
			d.num = $3;

			$$ = try_bind(curr_opthier, $1, OTYPE_BOOL, d);
		}
	| field '=' INTEGER ';'
		{
			gendata d;
			d.num = $3;

			$$ = try_bind(curr_opthier, $1, OTYPE_NUMBER, d);
		}
	| field '=' STRING ';'
		{
			gendata d;
			d.ptr = $3;

			$$ = try_bind(curr_opthier, $1, OTYPE_STRING, d);
		}
	| field '=' EMPTY ';'
		{
			gendata dummy;
			dummy.ptr = NULL;		/* Shut up compiler */

			$$ = try_bind(curr_opthier, $1, OTYPE_EMPTY, dummy);
		}
	;

field:
	IDENTIFIER	{ $$ = $1; }
	;

start:
	blocks
	;

%%

void
yyerror(char *err, ...) {
	va_list ap;
	va_start(ap, err);

	printf("camille: line %i: ", lineno);
	vprintf(err, ap);
	printf("\n");

	va_end(ap);
}


/*
 * NOTE: Not actual doxygen info, since it's static.
 *
 * Try to look up and bind an option, and do some error outputting if we failed.
 *
 * \param h     The option hierarchy under which to look for the option.
 * \param name  The name of the option to instantiate.
 * \param t     Requested option type.
 * \param d     The value of the option.
 *
 * \return  The instantiation of the option.
 */
static binding
try_bind(option_hier h, char *name, option_type t, gendata d)
{
	option opt;
	option_type otype;
	binding bnd;

	if ((opt = option_hier_lookup(h, name)) == NULL) {
		yyerror("option %s not recognised", name);
		bnd = NULL;
	} else {
		if ((bnd = binding_create(opt, t, d)) == NULL) {
			if (errno == EINVAL) {
				otype = option_get_type(opt);
				yyerror("%s is of type %s", name,
				option_type_name(otype));
			} else {
				yyerror("%s while binding option %s",
					strerror(errno), name);
			}
		}
	}
	return bnd;
}


/*
 * Add a contact to an addressbook.  If that's not possible, emit
 *  an error, destroy the contact and return the old addressbook.
 */
addrbook
try_add_contact(addrbook ab, contact ct)
{
	addrbook ab_tmp;

	ab_tmp = addrbook_add_contact(ab, ct);
	if (ab_tmp == NULL) {
		if (errno == EINVAL)
			yyerror("Redefinition of contact %s",
				contact_get_name(ct));
		else
			yyerror("Error adding contact %s; %s",
				contact_get_name(ct),
				strerror(errno));
		contact_destroy(ct);
	} else {
		ab = ab_tmp;
	}

	return ab;
}


/*
 * Add a group to an addressbook.  If that's not possible, emit
 *  an error, destroy the group and return the old addressbook.
 */
addrbook
try_add_group(addrbook ab, group gr)
{
	addrbook ab_tmp;

	/* TODO: Check whether group has a members field */
	/* If so: */
	ab_tmp = addrbook_add_group(ab, gr);
	if (ab_tmp == NULL) {
		if (errno == EINVAL)
			yyerror("Redefinition of group %s", group_get_name(gr));
		else
			yyerror("Error adding group %s: %s",
				group_get_name(gr),
				strerror(errno));
		group_destroy(gr);
	} else {
		ab = ab_tmp;
	}

	return ab;
}


static void
defaults_walker(binding bnd, gendata data)
/* ARGSUSED1 */
{
	if (!binding_empty(bnd))
		option_set_default(binding_get_option(bnd),
				   binding_get_value(bnd));
}


/*
 * Insert a binding into a bind_list.  If that's not possible, emit an
 *  error, destroy the binding and return the old binding list.
 */
static bind_list
try_insert_binding(bind_list bl, binding bnd)
{
	bind_list bl_tmp;

	assert(bl != NULL);

	/*
	 * Simply skip invalid bindings.  We've emitted errors
	 *  when we encountered them, anyway.
	 */
	if (bnd != NULL) {
		bl_tmp = bind_list_insert_uniq(bl, bnd);

		if (bl_tmp == NULL) {
			char *name;
			name = option_get_name(binding_get_option(bnd));

			if (errno == EINVAL) {
				yyerror("Duplicate entry for option %s", name);
			} else {
				yyerror("%s adding %s to list",
				 	strerror(errno), name);
			}
			binding_destroy(bnd);
		} else {
			bl = bl_tmp;
		}
	}
	return bl;
}


/*
 * Insert a contact id into an alist.  If that's not possible, emit an
 *  error, destroy the contact id and return the old association list.
 */
static alist
try_insert_contact_id(alist al, contact_id id)
{
	gendata key, value;
	alist al_tmp;

	assert(al != NULL);

	/* Skip error ids */
	if (id != NULL) {
		key.ptr = contact_id_get_name(id);
		value.ptr = id;
		al_tmp = alist_insert_uniq(al, key, value, str_eq);
		if (al_tmp == NULL) {
			if (errno == EINVAL) {
				yyerror("Duplicate entry for id %s", key.ptr);
			} else {
				yyerror("%s adding %s to list",
					 strerror(errno), key.ptr);
			}
			contact_id_destroy(id);
		} else {
			al = al_tmp;
		}
	}

	return al;
}


/*
 * Create a `primary' contact id for a contact.  If that's not possible, emit
 *  an error, destroy the `primary' binding list and return the old contact.
 */
static contact
try_create_primary_id(contact ct, bind_list bl)
{
	contact ct_tmp;
	char *name;

	assert(ct != NULL);
	assert(bl != NULL);

	name = contact_get_name(ct);

	if (!bind_list_empty(bl)) {
		contact_id prim, prim_tmp;

		prim = contact_id_create("primary");
		if ((prim_tmp = contact_id_add_bindings(prim, bl)) == NULL) {
			yyerror("Out of memory creating implicit "
				"primary id for contact %s", name);
			contact_id_destroy(prim);
			bind_list_destroy(bl);
			return ct;
		} else {
			prim = prim_tmp;
		}

		/* Try to add explicit primary id bindings */
		if ((ct_tmp = contact_add_id(ct, prim)) == NULL) {
			yyerror("Implicit primary contact id disallowed when "
				"there is also an explicit primary for "
				"contact %s", name);
			/* This destroys the binding list as well */
			contact_id_destroy(prim);
		} else {
			ct = ct_tmp;
		}
	} else {
		/* Empty binding lists need to be destroyed too */
		bind_list_destroy(bl);
	}

	return ct;
}
