/*
 * $Id$
 * $Camille$
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

#ifdef DEBUG
#define TRACE printf("Reduce at line %d\n", __LINE__);
#else
#define TRACE
#endif

int read_defaults = 0;	/* Bool indicating whether we read a defaults block */

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
%token CONTACT, IDENTITY, DEFAULTS, EMPTY;

%type <amount> list_of_blocks list_of_identities
%type <amount> defaults_fields identity_fields fields

%start list_of_blocks

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

list_of_blocks:
	block list_of_blocks	{ $$ = 1 + $2; }
	| /* Empty */		{ $$ = 0; }
	;

block:	contact_block		{ }
	| defaults_block	{ }
	;

contact_block:
	CONTACT IDENTIFIER '{' contact_body '}'
		{
			printf("===> Check whether no contact with the same name exists.\n");
			printf("===> If not, store contact %s.\n", $2);
		}
	;

contact_body:
	list_of_identities
		{
			printf("===> Check that there was at least a primary identity.\n");
		}
	/*
	   XXX TODO Support direct identity field declarations in
	   contact_body, too, affecting the primary identity.  Note that in
	   this case, there may be no explicit primary identity, then.
	 */
	;

list_of_identities:
	identity list_of_identities	{ $$ = 1 + $2; }
	| /* Empty */			{ $$ = 0; }
	;

identity:
	IDENTITY IDENTIFIER '{' identity_fields '}'
		{
			printf("===> Check whether no identity with the same name exists.\n");
			printf("===> Check that identity %s has \"name\" and \"address\".\n", $2);
			printf("===> If both, store identity %s\n", $2);
		}
	;

defaults_block:
	DEFAULTS '{' defaults_fields '}'
		{
			if (read_defaults) {
				yyerror("Error: More than one defaults-block is not allowed.\n");
			}
			read_defaults = 1;

			if ($3 > 0) {
				printf("===> Found defaults (with fields)\n");
				printf("===> Clean and store above fields in buffer for contact defaults\n");
			} else {
				printf("===> Found empty defaults\n");
			}
		}
	;

identity_fields: fields { $$ = $1; };
defaults_fields: fields { $$ = $1; };

fields:
	field_assignment fields { /* Return the number of fields */ $$ = 1 + $2; }
	| /* Empty */ { $$ = 0; };

field_assignment:
	IDENTIFIER '=' BOOLEAN ';'	/* XXX Should be fieldname '=' boolean */
		{
			printf("Storing boolean \"%s\" => %d in contact buffer.\n",
			       $1, $3);
		}
	| IDENTIFIER '=' INTEGER ';'	/* XXX Should be fieldname '=' integer */
		{
			printf("Storing integer \"%s\" => %d in contact buffer.\n",
			       $1, $3);
		}
	| IDENTIFIER '=' STRING ';'	/* XXX Should be fieldname '=' string */
		{
			printf("Storing string \"%s\" => \"%s\" in contact buffer.\n",
			       $1, $3);
		}
	| IDENTIFIER '=' EMPTY ';'	/* XXX Should be fieldname '=' empty */
		{
			printf("Storing empty field \"%s\" in contact buffer.\n",
			       $1);
		}
	;

start:	list_of_blocks
		{
			printf("\n");
			printf("Summary:\n");
			printf("Number of contacts: %d\n", $1);
			printf("Found NO defaults\n");
		}

%%

void yyerror(char *err, ...) {
	va_list ap;
	va_start(ap, err);

	printf("camille: ");
	vprintf(err, ap);
	printf("\n");

	va_end(ap);
	exit(1);
}
