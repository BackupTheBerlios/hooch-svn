/*
 * $Id: contact.h 122 2004-10-24 17:25:52Z sjamaan $
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
 * \brief Option interface
 *
 * \file option.h
 * An option is a setting which is available to the user to set as
 * default or as a property of a contact id or group.
 *
 * Options are stored in a hierarchy.  A hierarchy is a list of options.
 * An option can be either a subhierarchy or a `real' option.  Subhierarchies
 * don't have a default value bound to them, since they can't be set.  Only
 * their suboptions can have values (unless those are hierarchies themselves).
 */
#ifndef CAMILLE_OPTION_H
#define CAMILLE_OPTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gune/types.h>
#include <gune/alist.h>

/**
 * \brief Option types.
 *
 * An option has a type associated with it, which the instantiations of the
 * option (bindings) must obey.
 */
typedef enum {
	OTYPE_HIER,		/**< \c default = option_t *. Can't be bound */
	OTYPE_STRING,		/**< String */
	OTYPE_BOOL,		/**< Boolean */
	OTYPE_NUMBER,		/**< Integer (hex or bin input allowed) */
	OTYPE_EMPTY,		/**< Not really an option.  Empty binding */
	NUM_OTYPES
} option_type;

typedef alist_t option_hier_t;
typedef alist option_hier;

/**
 * \brief Option data type implementation
 *
 * Options either have a default value, or a pointer to a list of
 * sub-options (an option hierarchy).
 */
typedef union {
	gendata def;		/**< Default value if type != \c OTYPE_HIER */
	option_hier subhier;	/**< Subhierarchy  if type == \c OTYPE_HIER */
} option_data;

/** \brief Option implementation */
typedef struct option_t {
	option_type type;		/**< Type of option */
	/* int valid_in_contact; */	/* This can be overridden in contacts */
	char *name;			/**< The name of the option */
	option_data data;		/**< Data (default value or subhier) */
} option_t, * option;

option_hier option_hier_create(void);
void option_hier_destroy(option_hier);
option_hier option_hier_insert(option_hier, option);
option option_hier_lookup(option_hier, char *);

char *option_type_name(option_type);
option option_create(char *, option_type, option_data);
void option_destroy(option);
option_type option_get_type(option);
gendata option_get_default(option);
void option_set_default(option, gendata);

#ifdef __cplusplus
}
#endif

#endif /* CAMILLE_OPTION_H */
