/*
 * Copyright 2012 Mo McRoberts.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "p_testsuite.h"

static struct test
{
	const char *name;
	int (*fn)(void);
	int expected;
} tests[] = {
	{ "test-parse-http-1.c", test_parse_http_1, PASS },

	{ NULL, NULL, 0 }
};

static const char *
passfail(int n)
{
	return n ? "FAIL" : "PASS";
}

int
main(int argc, char **argv)
{
	int c, r, unexpected, passed, failed;

	(void) argc;
	(void) argv;
	
	unexpected = 0;
	passed = 0;
	failed = 0;

	for(c = 0; tests[c].name; c++)
	{
		setlocale(LC_ALL, "C");
		printf("%40s ... ", tests[c].name);
		r = tests[c].fn();
		if(r == tests[c].expected)
		{
			printf("%s -- %d\n", passfail(r), r);
		}
		else
		{
			printf("%s -- %d (expected %s -- %d)\n", passfail(r), r, passfail(tests[c].expected), tests[c].expected);
			unexpected++;
		}
		if(r)
		{
			failed++;
		}
		else
		{
			passed++;
		}
	}
	printf("\n%c%c %d tests, %d passed, %d failed, %d unexpected result%c\n",
		   (unexpected ? '*' : '+'),  (unexpected ? '*' : '+'),
		   c, passed, failed, unexpected, unexpected == 1 ? ' ' : 's');
	return unexpected ? 1 : 0;
}

