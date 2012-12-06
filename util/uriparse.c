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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <errno.h>

#include "liburi.h"

static const char *short_program_name = "uriparse";
static const char *prefix = "";
static const char *uristr;
static const char *basestr;
static int verbose;
static int printuri;
static int omitempty;

static void parseargs(int argc, char **argv);
static void usage(void);
static URI *parseuris(void);
static int printcomp(URI *uri, const char *name, size_t (*fn)(URI *restrict, char *restrict, size_t), char **buffer, size_t *len);
static int printesc(const char *string);
static int print_uri(URI *uri);
static int print_components(URI *uri);

static void
parseargs(int argc, char **argv)
{
	const char *t;
	int ch;

	t = strrchr(argv[0], '/');
	if(t)
	{
		t++;
	}
	else
	{
		t = argv[0];
	}
	short_program_name = t;
	while((ch = getopt(argc, argv, "hvp:uo")) != -1)
	{
		switch(ch)
		{
		case 'h':
			usage();
			exit(EXIT_SUCCESS);
		case 'v':
			verbose = 1;
			break;
		case 'p':
			prefix = optarg;
			break;
		case 'u':
			printuri = 1;
			break;
		case 'o':
			omitempty = 1;
			break;
		default:
			usage();
			exit(EXIT_FAILURE);
		}
	}
	if(argc - optind < 1 || argc - optind > 2)
	{
		usage();
		exit(EXIT_FAILURE);
	}
	argc -= optind;
	argv += optind;
	uristr = argv[0];	
	if(argc >= 2)
	{
		basestr = argv[1];
	}
	else
	{
		basestr = NULL;
	}
}

static void
usage(void)
{
	fprintf(stderr, "Parse a URI and print its components\n\n");
	fprintf(stderr, "Usage: %s [OPTIONS] URI [BASE]\n\n", short_program_name);
	fprintf(stderr, "OPTIONS is one or more of:\n"
			"  -h                  Display this usage message and exit\n"
			"  -v                  Produce verbose output\n"
			"  -p PREFIX           Prefix output variable names with PREFIX\n"
			"  -u                  Print the parsed URI instead of components\n"
		    "  -o                  Omit printing components which are absent\n");
}

static URI *
parseuris(void)
{
	URI *uri, *rel, *base;

	if(basestr)
	{
		base = uri_create_str(basestr, NULL);		
		if(!base)
		{
			fprintf(stderr, "%s: failed to parse URI '%s': %s\n", short_program_name, basestr, strerror(errno));
			return NULL;
		}
		rel = uri_create_str(uristr, NULL);
		if(!rel)
		{
			fprintf(stderr, "%s: failed to parse URI '%s': %s\n", short_program_name, uristr, strerror(errno));
			return NULL;
		}
		uri = uri_create_uri(rel, base);
		if(!uri)
		{
			fprintf(stderr, "%s: failed to resolve '%s' against '%s': %s\n", short_program_name, uristr, basestr, strerror(errno));
		}
		uri_destroy(base);
		uri_destroy(rel);
		return uri;
	}
	uri = uri_create_str(uristr, NULL);
	if(!uri)
	{
		fprintf(stderr, "%s: failed to parse URI '%s': %s\n", short_program_name, uristr, strerror(errno));
		return NULL;
	}
	return uri;
}

static int
printcomp(URI *uri, const char *name, size_t (*fn)(URI *restrict, char *restrict, size_t), char **buffer, size_t *len)
{
	size_t r;
	char *p;

	r = fn(uri, *buffer, *len);
	if(r == (size_t) -1)
	{
		fprintf(stderr, "%s: failed to obtain %s: %s\n", short_program_name, name, strerror(errno));
		return -1;
	}
	if(r > *len)
	{
		p = (char *) realloc(*buffer, r);
		if(!p)
		{
			fprintf(stderr, "%s: failed to reallocate buffer from %lu to %lu bytes: %s\n", short_program_name, (unsigned long) *len, (unsigned long) r, strerror(errno));
			return -1;
		}
		*buffer = p;
		*len = r;
		r = fn(uri, *buffer, *len);
		if(r == (size_t) -1)
		{
			fprintf(stderr, "%s: failed to obtain %s: %s\n", short_program_name, name, strerror(errno));
			return -1;
		}
	}
	if(r == 0)
	{
		if(!omitempty)
		{
			printf("%s%s=''\n", prefix, name);
		}
		return 0;
	}	
	printf("%s%s=\"", prefix, name);
	printesc(*buffer);
	puts("\"");
	return 0;
}

static int
printesc(const char *string)
{
	const char *p;

	for(p = string; *p; p++)
	{
		switch(*p)
		{
		case '$':
		case '"':
		case '\\':
		case '`':
			putchar('\\');
			putchar(*p);
			break;
		default:
			if((unsigned char) *p < 32 || (unsigned char) *p > 127)
			{
				printf("\\%03o", *p);
				continue;
			}
			putchar(*p);
		}
	}
	return 0;
}

static int
print_uri(URI *uri)
{
	size_t len;
	char *buffer;

	len = uri_str(uri, NULL, 0);
	if(len == (size_t) -1)
	{
		fprintf(stderr, "%s: failed to recompose URI: %s\n", short_program_name, strerror(errno));
		return -1;
	}
	buffer = (char *) malloc(len);
	if(!buffer)
	{
		fprintf(stderr, "%s: failed to allocate %lu bytes: %s\n", short_program_name, (unsigned long) len, strerror(errno));
		return -1;
	}	
	len = uri_str(uri, buffer, len);
	if(len == (size_t) -1)
	{
		fprintf(stderr, "%s: failed to recompose URI: %s\n", short_program_name, strerror(errno));
		return -1;
	}
	puts(buffer);
	free(buffer);
	return 0;
}

static int
print_components(URI *uri)
{
	size_t len;
	char *buffer;
	int r;

	r = 0;
	len = 0;
	buffer = NULL;
	if(printcomp(uri, "scheme", uri_scheme, &buffer, &len) == -1)
	{
		r = -1;
	}
	if(printcomp(uri, "auth", uri_auth, &buffer, &len) == -1)
	{
		r = -1;
	}
	if(printcomp(uri, "host", uri_host, &buffer, &len) == -1)
	{
		r = -1;
	}
	if(printcomp(uri, "port", uri_port, &buffer, &len) == -1)
	{
		r = -1;
	}
	if(printcomp(uri, "path", uri_path, &buffer, &len) == -1)
	{
		r = -1;
	}
	if(printcomp(uri, "query", uri_query, &buffer, &len) == -1)
	{
		r = -1;
	}
	if(printcomp(uri, "fragment", uri_fragment, &buffer, &len) == -1)
	{
		r = -1;
	}
	free(buffer);

	return 0;
}

int
main(int argc, char **argv)
{
	URI *uri;
	int r;

	setlocale(LC_ALL, "");
	parseargs(argc, argv);
	uri = parseuris();
	if(!uri)
	{
		return 1;
	}
	if(printuri)
	{
		r = print_uri(uri);	
	}
	else
	{
		r = print_components(uri);
	}
	uri_destroy(uri);
	return (r ? 1 : 0);
}
