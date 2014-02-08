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

/* Parse a selection of URIs and test the results */

static struct urimatch tests[] = {
	{ "http://user:pass@foo:21/bar;par?b#c", NULL,
	  "http", 5,
	  "user:pass", 10,
	  "foo", 4,
	  "21", 3,
	  "/bar;par", 9,
	  "b", 2,
	  "c", 2, 
	  21,
	  1,
	  "http://user:pass@foo:21/bar;par?b#c", 36,
	  UM_ALL },

	{ "http://foo/bar;par?b#c", NULL,
	  "http", 5,
	  NULL, 0,
	  "foo", 4,
	  NULL, 0,
	  "/bar;par", 9,
	  "b", 2,
	  "c", 2, 
	  0,
	  1,
	  "http://foo/bar;par?b#c", 23,
	  UM_ALL },


	{ NULL, NULL, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, 0, 0, NULL, 0, 0 }
};

int
test_parse_http_1(void)
{
	return test_urimatch(__FILE__, tests);
}
