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

/*
 * Configuration/contact options
 */
#ifndef CAMILLE_OPTION_H
#define CAMILLE_OPTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gune/types.h>
#include <gune/alist.h>

typedef enum {
	OTYPE_HIER,		/* default=option_t *. Can't be bound */
	OTYPE_STRING,
	OTYPE_BOOL,
	OTYPE_NUMBER,
	NUM_OTYPES
} option_type;

typedef alist_t option_hier_t;
typedef alist option_hier;

/** Invalid option hierarchy, used as error in return value */
extern option_hier_t * const ERROR_OPTION_HIER;

typedef union {
	gendata def;			/* Default val if type != OTYPE_HIER */
	option_hier subhier;		/* Subhierarchy if type == OTYPE_HIER */
} option_data;

typedef struct option_t {
	option_type type;
	/* int valid_in_contact; */	/* This can be overridden in contacts */
	char *name;
	option_data data;
} option_t, * option;

/** Invalid option, used as error in return value */
extern option_t * const ERROR_OPTION;

/** Option binding */
typedef struct binding_t {
	option option;			/* The bound option */
	int empty;			/* empty, "override back to default" */
	gendata value;			/* The bound value */
} binding_t, * binding;

/** Invalid binding, used as error in return value */
extern binding_t * const ERROR_BINDING;

/** Option binding list */
typedef alist bind_list;
typedef alist_t bind_list_t;

/** Invalid bind list, used as error in return value */
extern bind_list_t * const ERROR_BIND_LIST;

char *option_type_name(option_type);

option_hier option_hier_create(void);
void option_hier_destroy(option_hier);
option_hier option_hier_insert(option_hier, option);
option option_hier_lookup(option_hier, char *);

option option_create(char *, option_type, option_data);
void option_destroy(option);
option_type option_get_type(option o);

bind_list bind_list_create(void);
void bind_list_destroy(bind_list);
bind_list option_bind(bind_list, option, option_type, int, gendata);
bind_list option_unbind(bind_list, option);

#ifdef DEBUG
void bind_list_dump(bind_list);
#endif

#ifdef __cplusplus
}
#endif

#endif /* CAMILLE_OPTION_H */
