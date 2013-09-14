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

static int testlen(const char *restrict file, const char *name, URI *restrict uri, struct urimatch *restrict test, size_t (*fn)(URI *restrict, char *restrict, size_t), size_t expected);

static int teststr(const char *restrict file, const char *name, URI *restrict uri, char *restrict buf, size_t buflen, struct urimatch *restrict test, size_t (*fn)(URI *restrict, char *restrict, size_t), const char *restrict expected);

int
test_urimatch(const char *file, struct urimatch *tests)
{
	URI *uri, *base;
	int c;
	int failed, didfail;
	char *buffer, *p;
	size_t bufsize, bl;
	int r;

	buffer = NULL;
	bufsize = 0;
	failed = 0;
	for(c = 0; tests[c].uri; c++)
	{
		base = NULL;
		bl = 0;
		if(tests[c].base)
		{
			base = uri_create_str(tests[c].base, NULL);
			bl = strlen(tests[c].base);
			if(!base)
			{
				fprintf(stderr, "%s: failed to construct base URI '%s': %s\n", file, tests[c].base, strerror(errno));
				failed++;
				continue;
			}			
		}
		bl += strlen(tests[c].uri);
		uri = uri_create_str(tests[c].uri, base);
		if(!uri)
		{
			fprintf(stderr, "%s: failed to construct URI '%s': %s\n", file, tests[c].uri, strerror(errno));
			uri_destroy(base);
			failed++;
			continue;
		}
		bl = (bl * 3) + 1;
		if(bl > bufsize)
		{
			p = (char *) realloc(buffer, bl);
			if(!p)
			{
				fprintf(stderr, "%s: failed to resize buffer from %u to %u bytes: %s\n", file, (unsigned) bufsize, (unsigned) bl, strerror(errno));
				exit(2);
			}
			buffer = p;
			bufsize = bl;
		}
		didfail = 0;
#define TESTLEN(name, mask) \
		if(tests[c].testmask & mask) \
		{ \
			didfail += testlen(file, #name, uri, &(tests[c]), uri_##name, tests[c].name##len); \
		}
		TESTLEN(scheme, UM_SCHEME_LEN);
		TESTLEN(auth, UM_AUTH_LEN);
		TESTLEN(host, UM_HOST_LEN);
		TESTLEN(port, UM_PORT_LEN);
		TESTLEN(path, UM_PATH_LEN);
		TESTLEN(query, UM_QUERY_LEN);
		TESTLEN(fragment, UM_FRAGMENT_LEN);
#undef TESTLEN
		if(tests[c].testmask & UM_RECOMPOSED_LEN)
		{
			didfail += testlen(file, "recomposed string", uri, &(tests[c]), uri_str, tests[c].recomposedlen);
		}
		if(tests[c].testmask & UM_ABSOLUTE)
		{
			if((r = uri_absolute(uri)) != tests[c].absolute)
			{
				didfail++;
				if(tests[c].absolute)
				{
					fprintf(stderr, "%s: URI expected to be absolute, but return value was %d for %s\n", file, r, tests[c].uri);
				}
				else
				{
					fprintf(stderr, "%s: URI expected to not be absolute, but return value was %d for %s\n", file, r, tests[c].uri);
				}
			}
		}
		if(tests[c].testmask & UM_PORT)
		{
			if((r = uri_portnum(uri)) != tests[c].portnum)
			{
				didfail++;
				fprintf(stderr, "%s: parsed port number (%d) does not match expected port number (%d) for URI %s\n", file, r, tests[c].portnum, tests[c].uri);
			}
		}
#define TESTSTR(name, mask) \
		if(tests[c].testmask & mask) \
		{ \
			didfail += teststr(file, #name, uri, buffer, bufsize, &(tests[c]), uri_##name, tests[c].name); \
		}
		TESTSTR(scheme, UM_SCHEME);
		TESTSTR(auth, UM_AUTH);
		TESTSTR(host, UM_HOST);
		TESTSTR(port, UM_PORT);
		TESTSTR(path, UM_PATH);
		TESTSTR(query, UM_QUERY);
		TESTSTR(fragment, UM_FRAGMENT);
#undef TESTSTR
		if(tests[c].testmask & UM_RECOMPOSED)
		{
			didfail += teststr(file, "recomposed string", uri, buffer, bufsize, &(tests[c]), uri_str, tests[c].recomposed);
		}
		if(didfail)
		{
			failed++;
		}
		uri_destroy(uri);
		uri_destroy(base);
	}
	free(buffer);
	return failed ? FAIL : PASS;
}

static int
testlen(const char *restrict file, const char *name, URI *restrict uri, struct urimatch *restrict test, size_t (*fn)(URI *restrict, char *restrict, size_t), size_t expected)
{
	size_t r;

	r = fn(uri, NULL, 0);
	if(r != expected)
	{
		fprintf(stderr, "%s: unexpected buffer size requested for %s (%u, expected %u) for URI '%s'\n", file, name, (unsigned) r, (unsigned) expected, test->uri);
		return 1;
	}
	return 0;
}

static int
teststr(const char *restrict file, const char *name, URI *restrict uri, char *restrict buffer, size_t buflen, struct urimatch *restrict test, size_t (*fn)(URI *restrict, char *restrict, size_t), const char *restrict expected)
{
	size_t r;

	memset(buffer, 0, buflen);
	r = fn(uri, buffer, buflen);
	if(r == (size_t) -1)
	{
		fprintf(stderr, "%s: failed to obtain %s for URI '%s'\n", file, name, test->uri);
		return 1;
	}
	if(r == 0 && expected)
	{
		fprintf(stderr, "%s: expected %s '%s', but result was NULL for URI '%s'\n", file, name, expected, test->uri);
		return 1;
	}
	if(r && !expected)
	{
		fprintf(stderr, "%s: expected NULL %s, but result was '%s' for URI '%s'\n", file, name, buffer, test->uri);
		return 1;
	}
	if(!expected)
	{
		return 0;
	}
	if(strcmp(buffer, expected))
	{
		fprintf(stderr, "%s: expected '%s' for %s, but result was '%s' for URI '%s'\n", file, expected, name, buffer, test->uri);
		return 1;
	}
	return 0;
}
