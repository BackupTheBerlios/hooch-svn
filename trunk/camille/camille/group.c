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
 * \file group.c
 * Addressbook groups functionality
 */

#include <assert.h>
#include <stdlib.h>
#include <gune/error.h>
#include <gune/string.h>
#include <camille/group.h>

group_t * const ERROR_GROUP = (void *)error_dummy_func;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Create a group with a given name, initialising all data to the defaults.
 *
 * \param name  The (symbolic, identifier) name of the group.  This will get
 *		  copied, so you can free the original or pass const strings.
 *
 * \return  The new group, or ERROR_GROUP if there was an error.
 *
 * \sa group_destroy
 */
group
group_create(const char *name)
{
	group_t *gr;

	assert(name != NULL);

	if ((gr = malloc(sizeof(group_t))) == NULL)
		return ERROR_GROUP;

	if ((gr->name = str_cpy(name)) == NULL) {
		free(gr);
		return ERROR_GROUP;
	}

	if ((gr->bindings = bind_list_create()) == ERROR_BIND_LIST) {
		free(gr->name);
		free(gr);
		return ERROR_GROUP;
	}

	return (group)gr;
}


/**
 * Destroy a group.
 *
 * \param gr  The group to destroy.
 *
 * \sa group_create
 */
void
group_destroy(group gr)
{
	assert(gr != ERROR_GROUP);
	assert(gr != NULL);
	bind_list_destroy(gr->bindings);
	free(gr->name);
	free(gr);
}


/**
 * Retrieve the name of a group.
 *
 * \param gr  The group to get the name of.
 */
/*
 * XXX Actually we would like this to return a const ptr... We can't unless we
 *  we want to cast around while using the name as hash key.
 */
char *
group_get_name(group gr)
{
	assert(gr != ERROR_GROUP);
	assert(gr != NULL);
	return gr->name;
}


/**
 * Retrieve the bindings of a group.  This may be modified directly.
 *
 * \param gr  The group to get the bindings of.
 */
bind_list
group_get_bindings(group gr)
{
	assert(gr != ERROR_GROUP);
	assert(gr != NULL);
	return gr->bindings;
}


#ifdef DEBUG
/**
 * Prints a dump of a group.
 *
 * \param gr  The group to print.
 */
void
group_dump(group gr)
{
	assert(gr != ERROR_GROUP);
	assert(gr != NULL);
	printf("Group %s:\n", gr->name);
	bind_list_dump(gr->bindings);
}
#endif /* DEBUG */
