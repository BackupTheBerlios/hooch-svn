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
 * \brief Option bindings interface.
 *
 * \file binding.h
 * A binding is an object which combines an option with a value.  It allows
 * us to `bind' the value to the option, so to speak.
 */
#ifndef CAMILLE_BINDING_H
#define CAMILLE_BINDING

#ifdef __cplusplus
extern "C" {
#endif

#include <camille/option.h>
#include <gune/types.h>
#include <gune/alist.h>

/** \brief Option binding implementation */
typedef struct binding_t {
	option option;		/**< The bound option */
	int empty;		/**< empty? ("override back to default") */
	gendata value;		/**< The bound value */
} binding_t, * binding;

/** \brief Option binding list implementation */
typedef alist bind_list;
typedef alist_t bind_list_t;

/** \brief Function type for walking a binding list */
typedef void (*bind_walk_func) (binding, gendata);

binding binding_create(option, option_type, gendata);
void binding_destroy(binding);
option binding_get_option(binding);
int binding_empty(binding);
gendata binding_get_value(binding);

bind_list bind_list_create(void);
void bind_list_destroy(bind_list);
int bind_list_empty(bind_list);
bind_list bind_list_insert(bind_list, binding);
bind_list bind_list_insert_uniq(bind_list, binding);
void bind_list_walk(bind_list, bind_walk_func, gendata);

bind_list bind_list_merge(bind_list, bind_list);
bind_list bind_list_merge_uniq(bind_list, bind_list);

/* Convenience functions.  May be deleted in the future */
bind_list option_bind(bind_list, option, option_type, gendata);
bind_list option_unbind(bind_list, option);

#ifdef DEBUG
void bind_list_dump(bind_list);
#endif

#ifdef __cplusplus
}
#endif

#endif /* CAMILLE_BINDING_H */
