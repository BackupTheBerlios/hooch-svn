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

/* Character classes */
digit		[0-9]
hexdigit	[A-Fa-f0-9]
alpha		[A-Za-z]
idchars		[A-Za-z0-9_-]
strdelim	['"`]
wspace		[ \t]

%{

#include <stdio.h>
#include <gune/gune.h>
#include "parser.h"

void yyerror(char *, ...);

/* Values internally used in lexicographical analysis */
int lineno = 1;		/* Keep track of line numbers to produce friendly error messages */
stack s;		/* Stack used to store strings that are in analysis */
unsigned int ssize;	/* The size of the string that is in analysis */
char delim_opened;	/* The character that opened the string in analysis */

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
{strdelim}	{
			BEGIN STR_ENV;
			delim_opened = yytext[0];
			ssize = 0;
			s = stack_create();
                }

<STR_ENV>\\(.|\n)	{
				/* Keep track of line numbers, even in STR_ENV! */
				if (yytext[1] == '\n')
					lineno++;
				push_char(yytext[1]);
			}

<STR_ENV>{strdelim}	{
				/* End of string? */
				if (yytext[0] == delim_opened) {
					yylval.string = pop_string();
	
					/* Leave the STR_ENV environment */
					BEGIN 0;
					return STRING;
				} else {
					/* Otherwise, it's a normal character */
					push_char(yytext[0]);
				}
			}

<STR_ENV>\n	{
			/* Keep track of line numbers, even in STR_ENV! */
			lineno++;
			push_char(yytext[0]);
		}

<STR_ENV>.	{	push_char(yytext[0]);	 }

	/*** Language keywords ***/
(yes|true)		{	yylval.boolean = 1; return BOOLEAN;	}
(no|false|neener)	{	yylval.boolean = 0; return BOOLEAN;	}
defaults		{	return DEFAULTS;	}
contact			{	return CONTACT;		}
id			{	return IDENTITY;	}
group			{	return GROUP;		}
empty			{	return EMPTY;		}

	/*** Numbers ***/
\-?0x{hexdigit}+	{
				yylval.integer = strtol(yytext, NULL, 16);
				return INTEGER;
			}

\-?{digit}+		{
				yylval.integer = atoi(yytext);
				return INTEGER;
			}

	/*** Interpunction ***/
[{}=,;]		{	return yytext[0];	}

	/*** Identifiers ***/
{alpha}{idchars}+	{
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
	gendata d;
	d.sym = c;	/* Can't these lines be written as one? */

	ssize++;
	stack_push(s, d);
}

char *
pop_string(void)
{
	char *str = (char *)malloc(ssize + 1);
	str[ssize] = '\0';
	while (ssize > 0)
		str[--ssize] = stack_pop(s).sym;
	return str;
}
