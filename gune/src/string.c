/*
 * $Id$
 *
 * Copyright (c) 2003 Peter Bex and Vincent Driessen
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
 * \file string.c
 * String manipulation
 */

#include <stdlib.h>
#include <string.h>

#include "string.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/**
 * Concatenate two strings in a newly allocated string.
 * Original strings are left intact and a new string is malloc()ed.
 *
 * \param a  The first part of the new string.
 * \param b  The second part of the new string.
 *
 * \return  If all went right, a pointer to the newly allocated string.
 *          Otherwise, NULL.
 */
char *
str_cat(const char *a, const char *b)
{
	size_t l;
	char *x;

	if ((x = malloc(strlen(a) + strlen(b) + 1)) == NULL)
		return NULL;
	l = strlen(a);
	memcpy(x, a, l);
	strcpy(x + l, b);
	return x;
}


/**
 * Copy a string, allocating memory for the new string.
 *
 * \param s  The string to copy.
 *
 * \return   The copy of the original string, or NULL if out of memory.
 */
char *
str_cpy(const char *s)
{
	char *x;

	if ((x = malloc(strlen(s) + 1)) == NULL)
		return NULL;
	return strcpy(x, s);
}
