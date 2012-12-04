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

#ifndef P_TESTSUITE_H_
# define P_TESTSUITE_H_                 1

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <locale.h>
# include <errno.h>

# include "liburi.h"

# define PASS                           0
# define FAIL                           1

# define UM_NONE                        0
# define UM_SCHEME_LEN                  (1<<0)
# define UM_AUTH_LEN                    (1<<1)
# define UM_HOST_LEN                    (1<<2)
# define UM_PORT_LEN                    (1<<3)
# define UM_PATH_LEN                    (1<<4)
# define UM_QUERY_LEN                   (1<<5)
# define UM_FRAGMENT_LEN                (1<<6)
# define UM_RECOMPOSED_LEN              (1<<7)
# define UM_PORTNUM                     (1<<8)
# define UM_SCHEME                      (1<<9)
# define UM_AUTH                        (1<<10)
# define UM_HOST                        (1<<11)
# define UM_PORT                        (1<<12)
# define UM_PATH                        (1<<13)
# define UM_QUERY                       (1<<14)
# define UM_FRAGMENT                    (1<<15)
# define UM_RECOMPOSED                  (1<<16)
# define UM_ABSOLUTE                    (1<<17)

# define UM_ALL                         ((unsigned long) -1)

struct urimatch
{
	const char *uri;
	const char *base;
	const char *scheme;
	size_t schemelen;
	const char *auth;
	size_t authlen;
	const char *host;
	size_t hostlen;
	const char *port;
	size_t portlen;
	const char *path;
	size_t pathlen;
	const char *query;
	size_t querylen;
	const char *fragment;
	size_t fragmentlen;
	int portnum;
	int absolute;
	const char *recomposed;
	size_t recomposedlen;
	unsigned long testmask;
};

extern int test_urimatch(const char *file, struct urimatch *matches);

extern int test_parse_http_1(void);

#endif /*!P_TESTSUITE_H_*/
