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
	contact cnt;
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
%type <bindlist> defaults_stms
%type <bindlist> identity_stms
%type <bindlist> group_stms
%type <bindlist> stms
%type <bnd> stm
%type <assoclist> contact_body
%type <assoclist> identities
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
			addrbook a;

			a = addrbook_add_contact($2, $1);
			if (a == NULL) {
				if (errno == EINVAL)
					yyerror("Redefinition of contact %s",
						contact_get_name($1));
				else
					yyerror("Error adding contact %s; %s",
						contact_get_name($1),
						strerror(errno));
				contact_destroy($1);
				$$ = $2;
			} else {
				$$ = a;
			}
		}
	| defaults_block blocks
		{
			/* XXX TODO FIXME trein train */
			/* Add a set_defaults func to addrbook.c or soemthing */
			$$ = $2;
		}
	| group_block blocks
		{
			addrbook a;

			/* TODO: Check whether group has a members field */
			/* If so: */
			a = addrbook_add_group($2, $1);
			if (a == NULL) {
				if (errno == EINVAL)
					yyerror("Redefinition of group %s",
						group_get_name($1));
				else
					yyerror("Error adding group %s: %s",
						group_get_name($1),
						strerror(errno));
				group_destroy($1);
				$$ = $2;
			} else {
				$$ = a;
			}
		}
	| /* Empty */
		{
			$$ = curr_addrbook;
		}
	;

contact_block:
	CONTACT IDENTIFIER '{' contact_body '}'
		{
			$$ = contact_create($2, $4);
			free($2);
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
			if ($1 != NULL) {
				key.ptr = contact_id_get_name($1);
				value.ptr = $1;
				$$ = alist_insert_uniq($2, key, value, str_eq);
				if ($$ == NULL) {
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
	IDENTITY IDENTIFIER '{' identity_stms '}'
		{
			$$ = contact_id_create($2, $4);
			free($2);
			/* TODO: Check that this identity has name and address */
		}
	;

group_block:
	GROUP IDENTIFIER '{' group_stms '}'
		{
			$$ = group_create($2, $4);
			free($2);
		}
	;

defaults_block:
	DEFAULTS '{' defaults_stms '}'
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

identity_stms: stms		{ $$ = $1; };
defaults_stms: stms		{ $$ = $1; };
group_stms:    stms		{ $$ = $1; };	/* XXX Allow multi_stms, too */

stms:
	stm stms
		{
			assert ($2 != NULL);

			/*
			 * Simply skip error values.  We've emitted errors
			 *  when we encountered them, anyway.
			 */
			if ($1 != NULL) {
				$$ = bind_list_insert_uniq($2, $1);
				if ($$ == NULL) {
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
		}
	;

stm:
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


static void
defaults_walker(binding bnd, gendata data)
/* ARGSUSED1 */
{
	if (!binding_empty(bnd))
		option_set_default(binding_get_option(bnd),
				   binding_get_value(bnd));
}
