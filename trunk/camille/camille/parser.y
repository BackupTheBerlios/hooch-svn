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

#include <stdarg.h>
#include <camille/addrbook.h>

#ifdef DEBUG
#define TRACE printf("Reduce at line %d\n", __LINE__);
#else
#define TRACE
#endif

int read_defaults = 0;	/* Bool indicating whether we read a defaults block */

extern addrbook curr_addrbook;
extern contact curr_contact;
extern group curr_group;

extern int yylex(void);

%}

%union {
	char *identifier;
	char *string;
	int boolean;
	int integer;
	int amount;
};

%token <identifier> IDENTIFIER;
%token <string> STRING;
%token <boolean> BOOLEAN;
%token <integer> INTEGER;

%token CONTACT, IDENTITY, DEFAULTS, GROUP;
%token EMPTY;

/* Lists of blocks */
%type <amount> blocks
%type <amount> identities

/* Statement blocks */
%type <amount> defaults_stms
%type <amount> identity_stms
%type <amount> group_stms
%type <amount> stms

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
	block blocks		{ $$ = 1 + $2; }
	| /* Empty */		{ $$ = 0; }
	;

block:	contact_block		{ }
	| defaults_block	{ }
	| group_block		{ }
	;

contact_block:
	CONTACT IDENTIFIER
		{
			curr_contact = contact_create($2);
		}
	'{' contact_body '}'
		{
			printf("===> Store contact \"%s\"\n", $2);
			curr_addrbook = addrbook_add_contact(curr_addrbook,
							     curr_contact);
		}
	;

contact_body:
	identities
		{
			/* Check that there was at least a primary identity */
		}
	/*
	   XXX TODO Support direct identity field declarations in
	   contact_body, too, affecting the primary identity.  Note that in
	   this case, there may be no explicit primary identity, then.
	 */
	;

identities:
	identity identities	{ $$ = 1 + $2; }
	| /* Empty */		{ $$ = 0; }
	;

identity:
	IDENTITY IDENTIFIER
		{
			printf("===> Make new identity \"%s\"\n", $2);
			/* TODO: Check whether no identity with the same name exists */
		}
	'{' identity_stms '}'
		{
			/* TODO: Check that this identity has name and address */
			/* If so: */
			printf("===> Store identity \"%s\"\n", $2);
		}
	;

group_block:
	GROUP IDENTIFIER
		{
			printf("===> Make new group: \"%s\"\n", $2);
			/* TODO: Check whether no group with the same name exists */
			curr_group = group_create($2);
		}
	'{' group_stms '}'
		{
			/* TODO: Check whether group has a members field */
			/* If so: */
			printf("===> Store group %s.\n", $2);
			curr_addrbook = addrbook_add_group(curr_addrbook,
							  curr_group);
		}
	;

defaults_block:
	DEFAULTS
		{
			if (read_defaults) {
				yyerror("Error: More than one defaults-block is not allowed.\n");
			}
			read_defaults = 1;
			printf("===> Make new 'default settings'\n");
		}
	'{' defaults_stms '}'
		{
			/* XXX: Hmm, apparently need to use $4 here.  Why does
			 * the code block above seem to be $2?  Do some
			 * research. */
			if ($4 > 0) {
				printf("===> Found default settings! Store 'em!\n");
			} else {
				printf("===> Found no defaults settings (empty block)\n");
			}
		}
	;

identity_stms: stms		{ $$ = $1; };
defaults_stms: stms		{ $$ = $1; };
group_stms:    stms		{ $$ = $1; };	/* XXX Allow multi_stms, too */

stms:
	stm stms		{ $$ = 1 + $2; }
	| /* Empty */		{ $$ = 0; };

stm:
	';'			{ }
	| field '=' BOOLEAN ';'
		{
			printf("Storing boolean \"%s\" => %d\n", $1, $3);
		}
	| field '=' INTEGER ';'
		{
			printf("Storing integer \"%s\" => %d\n", $1, $3);
		}
	| field '=' STRING ';'
		{
			printf("Storing string \"%s\" => \"%s\"\n", $1, $3);
		}
	| field '=' EMPTY ';'
		{
			printf("Storing empty field \"%s\"\n", $1);
		}
	;

field:
	IDENTIFIER	{ $$ = $1; }
	;

start:
	blocks
		{
			printf("\n");
			printf("Summary:\n");
			printf("Number of contacts: %d\n", $1);
			printf("Found NO defaults\n");
		}

%%

void
yyerror(char *err, ...) {
	va_list ap;
	va_start(ap, err);

	printf("camille: ");
	vprintf(err, ap);
	printf("\n");

	va_end(ap);
	/*
	 * XXX TODO FIXME What should we do here, exactly?  Not exit, that's
	 * for sure.  At least, if we want to use this thing as a library.
	 */
	exit(1);
}
