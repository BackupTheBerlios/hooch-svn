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
 * \brief Group implementation.
 *
 * \file group.c
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <gune/string.h>
#include <camille/group.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * \brief Create a group.
 *
 * \param name  The (symbolic, identifier) name of the group.  This will get
 *		  copied, so you can free the original or pass const strings.
 * \param bl    The initial bindings for the group.
 *
 * \return  The new group, or \c NULL if there was an error.
 *
 * \par Errno values:
 * - \b ENOMEM if out of memory.
 *
 * \sa group_destroy
 */
group
group_create(const char *name, bind_list bl)
{
	group_t *gr;

	assert(name != NULL);
	assert(bl != NULL);

	if ((gr = malloc(sizeof(group_t))) == NULL)
		return NULL;

	if ((gr->name = str_cpy(name)) == NULL) {
		free(gr);
		return NULL;
	}

	gr->bindings = bl;

	return (group)gr;
}


/**
 * \brief Destroy a group.
 *
 * \param gr  The group to destroy.
 *
 * \sa group_create
 */
void
group_destroy(group gr)
{
	assert(gr != NULL);
	bind_list_destroy(gr->bindings);
	free(gr->name);
	free(gr);
}


/**
 * \brief Retrieve the name of a group.
 *
 * \param gr  The group to get the name of.
 *
 * \sa group_get_bindings
 */
/*
 * XXX Actually we would like this to return a const ptr... We can't unless we
 *  we want to cast around while using the name as hash key.
 */
char *
group_get_name(group gr)
{
	assert(gr != NULL);
	return gr->name;
}


/**
 * \brief Retrieve the bindings of a group.
 *
 * \note
 * The bindings may be modified directly.
 *
 * \param gr  The group to get the bindings of.
 *
 * \sa group_get_name
 */
bind_list
group_get_bindings(group gr)
{
	assert(gr != NULL);
	return gr->bindings;
}


#ifdef DEBUG
/**
 * \brief Print a dump of a group.
 *
 * \param gr  The group to print.
 */
void
group_dump(group gr)
{
	assert(gr != NULL);
	printf("Group %s:\n", gr->name);
	bind_list_dump(gr->bindings);
}
#endif /* DEBUG */
