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

static ssize_t uri_get_(UriTextRangeA *restrict range, char *restrict buf, size_t bufsize);
static int uri_addch_(int ch, char *restrict *restrict buf, size_t *restrict buflen);

size_t
uri_scheme(URI *restrict uri, char *restrict buf, size_t buflen)
{
	return uri_get_(&(uri->uri.scheme), buf, buflen);
}

size_t
uri_auth(URI *restrict uri, char *restrict buf, size_t buflen)
{
	return uri_get_(&(uri->uri.userInfo), buf, buflen);
}

size_t
uri_host(URI *restrict uri, char *restrict buf, size_t buflen)
{
	return uri_get_(&(uri->uri.hostText), buf, buflen);
}

size_t
uri_port(URI *restrict uri, char *restrict buf, size_t buflen)
{
	return uri_get_(&(uri->uri.portText), buf, buflen);
}

size_t
uri_query(URI *restrict uri, char *restrict buf, size_t buflen)
{
	return uri_get_(&(uri->uri.query), buf, buflen);
}

size_t
uri_fragment(URI *restrict uri, char *restrict buf, size_t buflen)
{
	return uri_get_(&(uri->uri.fragment), buf, buflen);
}

int
uri_absolute_path(URI *uri)
{
	if(uri->uri.absolutePath == URI_TRUE)
	{
		return 1;
	}
	if(uri->uri.hostText.first)
	{
		return 1;
	}
	if(uri->uri.scheme.first)
	{
		return 1;
	}
	return 0;
}

int
uri_absolute(URI *uri)
{
	if(uri->uri.scheme.first)
	{
		return 1;
	}
	return 0;
}

size_t
uri_path(URI *restrict uri, char *restrict buf, size_t buflen)
{
	size_t total, len;
	char *bp;
	UriPathSegmentA *p;
	
	if(!uri->uri.pathHead && !uri->uri.absolutePath)
	{
		if(buf && buflen)
		{
			*buf = 0;
		}
		return 0;
	}
	total = 0;
	bp = buf;
	if(uri_absolute_path(uri))
	{
		uri_addch_('/', &buf, &buflen);
		total++;
	}
	if(buf && buflen)
	{
		*buf = 0;
	}
	for(p = uri->uri.pathHead; p; p = p->next)
	{
		if(p != uri->uri.pathHead)
		{
			uri_addch_('/', &buf, &buflen);
			total++;
		}
		len = uri_get_(&(p->text), buf, buflen);
		if(len == (size_t) -1)
		{
			return (size_t) -1;
		}
		if(!len)
		{
			continue;
		}
		len--;
		total += len;
		if(buflen < len)
		{
			buflen = 0;
			buf = NULL;
		}
		else
		{
			if(buf)
			{
				buf += len;
			}
			buflen -= len;
		}
	}
	return total + 1;
}

int
uri_portnum(URI *uri)
{
	char buffer[32], *t;
	size_t len;
	unsigned long l;
	
	len = uri_port(uri, buffer, sizeof(buffer));
	if(len == (size_t) -1)
	{
		return -1;
	}
	if(len == 0 || len > sizeof(buffer) || !buffer[0])
	{
		return 0;
	}
	l = strtoul(buffer, &t, 10);
	if(t && *t)
	{
		return 0;
	}
	return (int) l;
}

URI_INFO *
uri_info(URI *uri)
{
	URI_INFO *p;
	char *buf;
	size_t buflen, r;

	buflen = 0;
#define getbuf(component) \
	r = uri_get_(&(uri->uri.component), NULL, 0);	  \
	if(r == (size_t) -1)							  \
	{												  \
		return NULL;								  \
	}												  \
	buflen += r;

	getbuf(scheme);
	getbuf(userInfo);
	getbuf(hostText);
	r = uri_path(uri, NULL, 0);
	if(r == (size_t) -1)
	{
		return NULL;
	}
	buflen += r;
	getbuf(query);
	getbuf(fragment);

#undef getbuf
	p = (URI_INFO *) calloc(1, sizeof(URI_INFO));
	if(!p)
	{
		return NULL;
	}
	buf = (char *) calloc(1, buflen);
	if(!buf)
	{
		free(p);
		return NULL;
	}
	p->internal = buf;
#define getbuf(component, member)						\
	r = uri_get_(&(uri->uri.component), buf, buflen);	\
	if(r)												\
	{													\
		p->member = buf;								\
		buf += r;										\
		buflen -= r;									\
	}

	getbuf(scheme, scheme);
	getbuf(userInfo, auth);
	getbuf(hostText, host);
	p->port = uri_portnum(uri);
	r = uri_path(uri, buf, buflen);
	if(r)
	{
		p->path = buf;
		buf += r;
		buflen -= r;
	}
	getbuf(query, query);
	getbuf(fragment, fragment);

#undef getbuf  
	return p;
}
	
/* Free a URI_INFO structure */
int
uri_info_destroy(URI_INFO *info)
{
	free(info->internal);
	free(info);
	return 0;
}

/* Compare two URIs and test for equality */
int
uri_equal(URI *a, URI *b)
{
	return uriEqualsUriA(&(a->uri), &(b->uri));
}

static ssize_t
uri_get_(UriTextRangeA *restrict range, char *restrict buf, size_t bufsize)
{
	size_t len, slen;

	if(!range->first)
	{
		if(buf && bufsize)
		{
			*buf = 0;
		}
		return 0;
	}
	if(range->afterLast)
	{
		len = range->afterLast - range->first;
	}
	else
	{
		len = strlen(range->first);
	}
	if(buf && bufsize)
	{
		/* Copy at most len or (bufsize - 1) characters, whichever
		 * is fewer.
		 */
		bufsize--;
		slen = bufsize < len ? bufsize : len;
		strncpy(buf, range->first, slen);
		buf[slen] = 0;
	}
	return len + 1;	
}

static int
uri_addch_(int ch, char *restrict *restrict buf, size_t *restrict buflen)
{
	if(*buf && *buflen)
	{
		**buf = ch;
		(*buf)++;
		**buf = 0;
		(*buflen)--;
	}
	return 1;
}
