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
 * \brief Contact list interface.
 *
 * \file contact.h
 * A contact is just a list of contact_ids with a name.
 *
 * In the address book format, it appears as if there can also be settings
 * for the contact itself, but that is just a convenient way to set the
 * options for the `primary' contact_id.
 */
#ifndef CAMILLE_CONTACT_H
#define CAMILLE_CONTACT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gune/alist.h>
#include <camille/contact_id.h>

/** \brief Contact implementation */
typedef struct contact_t {
	char *name;			/**< The contact's name */
	alist ids;			/**< contact_ids in this contact */
} contact_t, *contact;

contact contact_create(const char *, alist);
void contact_destroy(contact);
char *contact_get_name(contact);

contact contact_add_id(contact, contact_id);
contact contact_del_id(contact, char *);

#ifdef DEBUG
void contact_dump(contact);
#endif

#ifdef __cplusplus
}
#endif

#endif /* CAMILLE_CONTACT_H */
