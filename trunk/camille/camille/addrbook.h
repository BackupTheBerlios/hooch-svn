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
 * \brief Address book interface.
 *
 * \file addrbook.h
 */
#ifndef CAMILLE_ADDRBOOK_H
#define CAMILLE_ADDRBOOK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gune/ht.h>
#include <camille/contact.h>
#include <camille/option.h>
#include <camille/group.h>
#include <stdio.h>

/*
 * Contact and group hash table range.  In normal situations people have
 * more contacts than groups, so reflect this here.  Maybe this option should
 * become runtime configurable in addrbook creation function or elsewhere.
 * (in creation function would be inappropriate since changing the underlying
 *  storage type for contacts & groups shouldn't break the API)
 */
#define CONTACTS_RANGE		255
#define GROUPS_RANGE		16

/** \brief Address book implementation */
typedef struct addrbook_t {
	ht contacts;				/**< List of contacts */
	ht groups;				/**< List of groups */
	bind_list defaults;			/**< Default bindings */
} addrbook_t, *addrbook;

addrbook addrbook_create(void);
void addrbook_destroy(addrbook);
addrbook addrbook_add_contact(addrbook, contact);
addrbook addrbook_del_contact(addrbook, char *);
addrbook addrbook_add_group(addrbook, group);
addrbook addrbook_del_group(addrbook, char *);

addrbook addrbook_parse_file(FILE *, option_hier);

#ifdef DEBUG
void addrbook_dump(addrbook);
#endif

#ifdef __cplusplus
}
#endif

#endif /* CAMILLE_ADDRBOOK_H */
