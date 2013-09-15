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

#include "p_liburi.h"

size_t
uri_str(URI *restrict uri, char *restrict buf, size_t buflen)
{
	int bufsize;

	if(uriToStringCharsRequiredA(&(uri->uri), &bufsize) != URI_SUCCESS)
	{
		return (size_t) -1;
	}
	bufsize++;
	if(buf && buflen)
	{
		if(uriToStringA(buf, &(uri->uri), buflen, NULL) != URI_SUCCESS)
		{
			return (size_t) -1;
		}
		buf[buflen - 1] = 0;
	}
	return bufsize;
}

/* Allocate a new string using malloc() and copy the URI into it */
char *
uri_stralloc(URI *restrict uri)
{
	size_t needed;
	char *str;
	
	needed = uri_str(uri, NULL, 0);
	str = NULL;
	if(!needed ||
		(str = (char *) malloc(needed)) == NULL ||
		uri_str(uri, str, needed) != needed)
	{
		free(str);
		return NULL;
	}
	return str;
}
