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
#include <gune/error.h>
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

static contact	   curr_contact;
extern addrbook curr_addrbook;
option_hier curr_opthier;

extern int yylex(void);

extern int lineno;

static binding try_bind(option_hier, char *, option_type, gendata);
static void defaults_walker(binding, gendata);

%}

%union {
	char *identifier;
	char *string;
	int boolean;
	int integer;
	bind_list bindlist;
	binding bnd;
	alist assoclist;
	contact_id id;
};

%token <identifier> IDENTIFIER;
%token <string> STRING;
%token <boolean> BOOLEAN;
%token <integer> INTEGER;

%token CONTACT, IDENTITY, DEFAULTS, GROUP;
%token EMPTY;

/* Statement blocks */
%type <bindlist> defaults_stms
%type <bindlist> identity_stms
%type <bindlist> group_stms
%type <bindlist> stms
%type <bnd> stm
%type <assoclist> contact_body
%type <assoclist> identities
%type <id> identity

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

blocks:
	block blocks		{ }
	| /* Empty */		{ }
	;

block:	contact_block		{ }
	| defaults_block	{ }
	| group_block		{ }
	;

contact_block:
	CONTACT IDENTIFIER
		{
		}
	'{' contact_body '}'
		{
			addrbook a;

			curr_contact = contact_create($2, $5);
			free($2);

			a = addrbook_add_contact(curr_addrbook, curr_contact);
			if (a == ERROR_PTR) {
				if (errno == EINVAL)
					yyerror("Redefinition of contact %s",
						contact_get_name(curr_contact));
				else
					yyerror("Error adding contact %s; %s",
						contact_get_name(curr_contact),
						strerror(errno));
				contact_destroy(curr_contact);
			} else {
				curr_addrbook = a;
			}
		}
	;

contact_body:
	identities
		{
			/* Check that there was at least a primary identity */
			$$ = $1;
		}
	/*
	   XXX TODO Support direct identity field declarations in
	   contact_body, too, affecting the primary identity.  Note that in
	   this case, there may be no explicit primary identity, then.
	 */
	;

identities:
	identity identities
		{
			gendata key, value;

			/* Skip error ids */
			if ($1 != ERROR_PTR) {
				key.ptr = contact_id_get_name($1);
				value.ptr = $1;
				$$ = alist_insert_uniq($2, key, value, str_eq);
				if ($$ == ERROR_PTR) {
					if (errno == EINVAL) {
						yyerror("Duplicate entry for"
							 " id %s", key.ptr);
					}
					yyerror("%s adding %s to list",
						 strerror(errno), key.ptr);
					contact_id_destroy($1);
					$$ = $2;
				}
			} else {
				$$ = $2;
			}
		}
	| /* Empty */
		{
			$$ = alist_create();
		}
	;

identity:
	IDENTITY IDENTIFIER
		{
			/* curr_bind_list = contact_id_get_bindings(curr_id); */
		}
	'{' identity_stms '}'
		{
			$$ = contact_id_create($2, $5);
			free($2);
			/* TODO: Check that this identity has name and address */
		}
	;

group_block:
	GROUP IDENTIFIER
		{
			/* curr_bind_list = group_get_bindings(curr_group); */
		}
	'{' group_stms '}'
		{
			addrbook a;
			group curr_group;

			curr_group = group_create($2, $5);
			free($2);

			/* TODO: Check whether group has a members field */
			/* If so: */
			a = addrbook_add_group(curr_addrbook, curr_group);
			if (a == ERROR_PTR) {
				if (errno == EINVAL)
					yyerror("Redefinition of group %s",
						group_get_name(curr_group));
				else
					yyerror("Error adding group %s: %s",
						group_get_name(curr_group),
						strerror(errno));
				group_destroy(curr_group);
			} else {
				curr_addrbook = a;
			}
		}
	;

defaults_block:
	DEFAULTS
		{
			if (read_defaults) {
				yyerror("Error: More than one defaults-block is not allowed.");
			}
			printf("===> Make new 'default settings'\n");
		}
	'{' defaults_stms '}'
		{
			gendata data;

			if (!read_defaults) {
				read_defaults = 1;
				data.ptr = curr_opthier;
				bind_list_walk($4, defaults_walker, data);
			}
			bind_list_destroy($4);
		}
	;

identity_stms: stms		{ $$ = $1; };
defaults_stms: stms		{ $$ = $1; };
group_stms:    stms		{ $$ = $1; };	/* XXX Allow multi_stms, too */

stms:
	stm stms
		{
			printf("THERE!\n");
			assert ($2 != ERROR_PTR);

			/*
			 * Simply skip error values.  We've emitted errors
			 *  when we encountered them, anyway.
			 */
			if ($1 != ERROR_PTR) {
				$$ = bind_list_insert_uniq($2, $1);
				if ($$ == ERROR_PTR) {
					yyerror("%s adding %s to list",
						 strerror(errno), $1);
					binding_destroy($1);
					$$ = $2;
				}
			} else {
				$$ = $2;
			}
		}
	| /* Empty */
		{
			$$ = bind_list_create();
			printf("HERE!\n");
		}
	;

stm:
	';'
		{
			/*
			 * Just pretend we encountered an error binding.
			 */
			$$ = ERROR_PTR;
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
			dummy.ptr = ERROR_PTR;		/* Shut up compiler */

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

	if ((opt = option_hier_lookup(h, name)) == ERROR_PTR) {
		yyerror("option %s not recognised", name);
		bnd = ERROR_PTR;
	} else {
		if ((bnd = binding_create(opt, t, d)) == ERROR_PTR) {
			if (errno == EINVAL) {
				otype = option_get_type(opt);
				yyerror("%s is of type %s", name,
				option_type_name(otype));
			} else {
				yyerror("%s while binding option %s",
					strerror(errno), name);
			}
		}
	printf("Just bound %s\n", name);
	}
	return bnd;
}


static void
defaults_walker(binding bnd, gendata data)
/* ARGSUSED1 */
{
	if (!binding_empty(bnd))
		option_set_default(binding_get_option(bnd),
				   binding_get_value(bnd));
}
