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
 * THIS SOFTWARE IS PROVIDED BY PETER BEX AND VINCENT DRESSEN AND CONTRIBUTORS
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gune/error.h>
#include <camille/camille.h>

void
usage(void)
{
	printf("usage: test [options]\n");
	printf("options:\n");
	printf("-h    Print this help message.\n");
}


option_hier
add_builtin_options(void)
{
	option_hier h;
	option_hier hnew;
	option o;
	option_data d;

	if ((h = option_hier_create()) == ERROR_PTR)
		log_entry(WARN_ERROR, "Cannot create option hierarchy");

	/* Add some simple options to demonstrate that it works */
	/*
	 * NOTE: Maybe it is useful to add a callback which can check the
	 * string's format.  This way we can verify if the entered data is not
	 * nonsense.  OTOH, it can also be checked later when using it.
	 */

	d.def.ptr = "";			/* Empty string as default addr */

	if ((o = option_create("address", OTYPE_STRING, d)) == ERROR_PTR) {
		option_hier_destroy(h);
		log_entry(WARN_ERROR, "Cannot create option!");
	}

	if ((hnew = option_hier_insert(h, o)) == ERROR_PTR) {
		option_hier_destroy(h);
		log_entry(WARN_ERROR, "Cannot insert option in hierarchy!");
	}

	h = hnew;

	return h;
}


int
main(int argc, char **argv)
{
	extern char *optarg;
	int ch;
	addrbook a;
	option_hier hier;

	printf("$Id$\n");
	printf("Running test... Go your gang.\n");

	while ((ch = getopt(argc, argv, "h")) != -1)
		switch (ch) {
		case 'h':
			usage();
			return 0;
		}

	/* Perform tests */
	hier = add_builtin_options();
	a = addrbook_parse_file(stdin, hier);
	printf("Done parsing.  Resulting addressbook:\n");
	addrbook_dump(a);

	return 0;
}
