/*
 * $Id: parser.y 70 2004-06-06 02:26:09Z nvie $
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

/* Character classes */
digit	[0-9]
letter	[A-Za-z]
upcase	[A-Z]
lowcase	[a-z]
wspace	[ \t]

%{

#include <stdio.h>
#include <gune/gune.h>
#include "parser.h"

void yyerror(char *, ...);

/* Values internally used in lexicographical analysis */
int lineno = 1;
int defaults_found = 0;
stack s;
int ssize;

void push_char(char);
char *pop_string(void);

%}

%x STR_ENV

%%

	/*** Whitespaces and comments ***/
#.*		;		/* Skip over comments, starting with # until \n */

{wspace}+	;       	/* Skip whitespaces */

\n		lineno++;	/* Keep track of line numbers */

	/*** Strings ***/
\"		{
			BEGIN STR_ENV;
			ssize = 0;
			s = stack_create();
                }

<STR_ENV>\\(.|\n)	{
			if (yytext[1] == '\n')
				lineno++;	/* Keep track of line numbers,
						   even in STR_ENV! */
			push_char(yytext[1]);
		}

<STR_ENV>\"	{
			yylval.string = pop_string();

			BEGIN 0;	/* Leave the STR_ENV environment */
			return STRING;
		}

<STR_ENV>\n	{
			/* Keep track of line numbers, even in STR_ENV! */
			lineno++;
			push_char(yytext[0]);
		}

<STR_ENV>.	{
			push_char(yytext[0]);
		}

	/*** Language keywords ***/
defaults	{
			if (defaults_found++ > 0) {
				/* XXX: Maybe print file name, too? */
				yyerror("line %d: Block defaults multiply "
			  	        "defined (only allowed once)", lineno);
			}
			else
				return DEFAULTS;
		}
contact		return CONTACT;

	/*** Interpunction ***/
[{}=;]		return yytext[0];

	/*** Identifiers ***/
{letter}+	{
			char *buf = (char *)malloc((unsigned int)yyleng + 1);
			strcpy(buf, yytext);
			yylval.identifier = buf;
			return IDENTIFIER;
		}

.		{
			yyerror("Unknown character on line %d: ``%c''\n", lineno, yytext[0]);
			exit(1);
		}

%%

int
yywrap(void) {
        return 1;
}

void
push_char(char c)
{
	ssize++;
	/* printf("read %c... setting ssize to %d\n", c, ssize); */
	stack_push(s, (void *) (int) c);
	/* NOTE: This ----^ is rans */
}

char *
pop_string(void)
{
	char *str = (char *)malloc(ssize + 1);
	str[ssize] = '\0';
	while (ssize > 0)
		str[--ssize] = (char) (int) stack_pop(s);
		/* NOTE: This ----^ is rans */
	return str;
}
