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
#include "../src/gune.h"

void
strcat_tester(char *s)
{
	/* Try it */
	printf("%s...\n", str_cat(s, s));
}


void
err_tester(warnlvl wrn)
{
	/* How lame, eh? */
	log_entry(wrn, "This is a test message...");
}


void
stress_test_stack(int num)
{
	int i, *x;
	stack s;
	
	x = (int *)malloc(sizeof(int));
	*x = 13;

	/* Create a new stack */
	s = stack_new();

	/* Fill it up, and remove items again */
	printf("Filling a stack with %d items...\n", num);
	for (i = 0; i < num; ++i)
		stack_push(s, x);
	printf("Emptying stack of %d items...\n", num);
	for (i = 0; i < num; ++i)
		*x = *(int *)stack_pop(s);

	assert(stack_is_empty(s) == true);

	/* Now, again, fill up the stack... */
	for (i = 0; i < num; ++i)
		stack_push(s, x);

	assert(stack_is_empty(s) == false);

	/* ...and free it */
	stack_free(s);
	free(x);
}


void
usage(void)
{
	printf("usage: test [-n num] [-l log] -s num | -e lvl\n");
	printf("-n num	Perform each test `num' times. Default is 10.\n");
	printf("-s num	Do a stack stress test on `num' stack items.\n");
	printf("-e lvl  Print an error on the specified level (0-3).\n");
	printf("-l log  Use log as a file to write messages to.\n");
	printf("-c      Test string copy routines.\n");
}


int
main(int argc, char **argv)
{
	extern char *optarg;
	char *str = NULL;
	char ch;

	warnlvl wrn = WARN_NOTIFY;

	/* Default options */
	int i, loop, stack_test, err_test, strcat_test, idle;
	stack_test = 0;
	err_test = 0;
	strcat_test = 0;
	loop = 10;
	idle = 1;	/* Set idle, unless a test should be run */

	if (argc <= 1) {
		usage();
		return 1;
	}

	while ((ch = getopt(argc, argv, "n:s:l:e:c:")) != -1)
		switch (ch) {
		case 'n':
			loop = atoi(optarg);
			break;
		case 's':
			stack_test = atoi(optarg);
			idle = 0;
			break;
		case 'l':
			set_logfile(fopen(optarg, "a"));
			break;
		case 'e':
			if (atoi(optarg) >= 0 && atoi(optarg) < NUM_WARNLVLS)
				wrn = atoi(optarg);
			else {
				fprintf(stderr, "Please specifify a number between 0 and 4.\n");
				exit(1);
			}
			idle = 0;
			err_test = 1;
			break;
		case 'c':
			strcat_test = 1;
			str = optarg;
			idle = 0;
			break;
		default:
			usage();
			return 1;
		}

	/* Perform `n' tests of each kind */
	if (idle == 0)
		for (i = 0; i < loop; ++i) {
			printf("========= RUNNING TEST %d =========\n", i + 1);
			if (stack_test > 0)
				stress_test_stack(stack_test);
			printf("\n");
			if (err_test > 0)
				err_tester(wrn);
			if (strcat_test > 0)
				strcat_tester(str);
		}

	printf("Done\n");

	return 0;
}
