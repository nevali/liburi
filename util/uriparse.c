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
static int verbose;
static int printuri;

static void
usage(void)
{
	fprintf(stderr, "Parse a URI and print its components\n\n");
	fprintf(stderr, "Usage: %s [OPTIONS] URI [BASE]\n\n", short_program_name);
	fprintf(stderr, "OPTIONS is one or more of:\n"
			"  -h                  Display this usage message and exit\n"
			"  -v                  Produce verbose output\n"
			"  -p PREFIX           Prefix output variable names with PREFIX\n"
			"  -u                  Print the parsed URI instead of components\n");
}

static int
printcomp(const char *name, char *buffer, ssize_t len)
{
	if(len < 0)
	{
		fprintf(stderr, "%s: failed to obtain %s\n", short_program_name, name);
		return -1;
	}
	if(len == 0)
	{
		if(verbose)
		{
			fprintf(stderr, "%s: URI has no %s\n", short_program_name, name);
		}
		return 0;
	}	
	printf("%s%s='%s'\n", prefix, name, buffer);
	return 0;
}

int
main(int argc, char **argv)
{
	const char *t;
	URI *uri, *base;
	char buffer[256], *bp;
	size_t buflen, len;
	int port, ch;

	setlocale(LC_ALL, "");
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
	while((ch = getopt(argc, argv, "hvp:u")) != -1)
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
	buflen = strlen(argv[0]);
	if(argc >= 2)
	{
		base = uri_create_str(argv[1], NULL);
		buflen += strlen(argv[1]);
	}
	else
	{
		base = NULL;
	}
	buflen = (buflen * 3) + 1;	
	uri = uri_create_str(argv[0], base);
	if(!uri)
	{
		fprintf(stderr, "%s: failed to parse URI '%s'\n", short_program_name, argv[0]);
		exit(EXIT_FAILURE);
	}
	if(printuri)
	{
		bp = (char *) calloc(1, buflen);
		if(!bp)
		{
			perror(short_program_name);
			exit(EXIT_FAILURE);
		}
		len = uri_str(uri, bp, buflen);
		if(len == (size_t) -1)
		{
			fprintf(stderr, "%s: failed to recompose URI: %s\n", short_program_name, strerror(errno));
			exit(EXIT_FAILURE);
		}
		if(len > buflen)
		{
			fprintf(stderr, "%s: URI too long to recompose\n", short_program_name);
			exit(EXIT_FAILURE);
		}
		puts(bp);
		free(bp);
		return 0;
	}	
	len = uri_scheme(uri, buffer, sizeof(buffer));
	printcomp("scheme", buffer, len);

	len = uri_auth(uri, buffer, sizeof(buffer));
	printcomp("auth", buffer, len);

	len = uri_host(uri, buffer, sizeof(buffer));
	printcomp("host", buffer, len);

	port = uri_portnum(uri);
	if(port < 0)
	{
		printcomp("port", NULL, 0);
	}
	else
	{
		sprintf(buffer, "%d", port);
		printcomp("port", buffer, strlen(buffer));
	}

	len = uri_path(uri, buffer, sizeof(buffer));
	printcomp("path", buffer, len);

	len = uri_query(uri, buffer, sizeof(buffer));
	printcomp("query", buffer, len);

	len = uri_fragment(uri, buffer, sizeof(buffer));
	printcomp("fragment", buffer, len);
	uri_destroy(uri);
	return 0;
}
