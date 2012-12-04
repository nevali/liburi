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

static size_t
uri_wctoutf8_(int *dest, wchar_t ch)
{
	if(ch < 0x7f)
	{
		dest[0] = ch;
		return 1;
	}
	if(ch < 0x07ff)
	{
		/* 110aaaaa 10bbbbbb */
		dest[0] = 0xc0 | ((ch & 0x0007c0) >>  6);
		dest[1] = 0x80 | (ch & 0x00003f);
		return 2;
	}
	if(ch < 0xffff)
	{
		/* 1110aaaa 10bbbbbb 10cccccc */
		dest[0] = 0xe0 | ((ch & 0x00f000) >> 12);
		dest[1] = 0x80 | ((ch & 0x000fc0) >> 6);
		dest[2] = 0x80 | (ch & 0x00003f);
		return 3;
	}
	/* 11110aaa 10bbbbbb 10cccccc 10dddddd */
	dest[0] = 0xf0 | ((ch & 0x1c0000) >> 18);
	dest[1] = 0x80 | ((ch & 0x03f000) >> 12);
	dest[2] = 0x80 | ((ch & 0x000fc0) >>  6);
	dest[3] = 0x80 | (ch & 0x00003f);
	return 4;
}

static size_t
uri_encode_wide_(char *dest, wchar_t ch)
{
	static const char hexdig[16] = {
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
	};

	int utf8[6];
	size_t l, c;
	
	l = uri_wctoutf8_(utf8, ch);
	for(c = 0; c < l; c++)
	{
		*dest = '%';
		dest++;
		*dest = hexdig[utf8[c] >> 4];
		dest++;
		*dest = hexdig[utf8[c] & 15];
		dest++;
	}
	return l * 3;
}

/* Scan the URI string for wide characters and return the maximum storage
 * needed for their UTF-8 encoding
 */
static size_t
uri_widebytes_(const char *uristr, size_t nbytes)
{
	wchar_t ch;
	int r;
	const char *p;
	size_t numwide;

	mbtowc(&ch, NULL, 0);
	numwide = 0;
	for(p = uristr; *p;)
	{
		r = mbtowc(&ch, p, nbytes);
		if(r <= 0)
		{
			return (size_t) -1;
		}		
		if(ch < 33 || ch > 127)
		{
			/* Account for the full 6 bytes of UTF-8: we can't assume that
			 * the source string (and hence the return value of mbtowc()) is
			 * itself UTF-8, as it's locale-dependent.
			 */
			numwide += 6;
		}
		p += r;
	}
	return numwide;
}

/*
 * Map a potential IRI to a URI, see section 3.1 of RFC3987, converting
 * from locale-specific multibyte encoding to wide characters as we do
 */
static int
uri_preprocess_(char *restrict buf, const char *restrict uristr, size_t nbytes)
{
	wchar_t ch;
	char *bp;
	int r;

	mbtowc(&ch, NULL, 0);
	r = 0;
	for(bp = buf; nbytes && *uristr;)
	{
		r = mbtowc(&ch, uristr, nbytes);
		if(r <= 0)
		{
			return -1;
		}		
		if(ch < 33 || ch > 127)
		{			
			bp += uri_encode_wide_(bp, ch);
		}
		else
		{
			*bp = ch;
			bp++;
		}
		uristr += r;
		nbytes -= r;
	}
	*bp = 0;
	return 0;
}

URI *
uri_create_str(const char *restrict uristr, const URI *restrict base)
{
	UriParserStateA state;
	UriUriA absolute;
	URI *uri;
	size_t l, numwide;
	int r; 

	uri = (URI *) calloc(1, sizeof(URI));
	if(!uri)
	{
		return NULL;
	}
	l = strlen(uristr) + 1;
	numwide = uri_widebytes_(uristr, l);
	if(numwide == (size_t) -1)
	{
		uri_destroy(uri);
		return NULL;
	}
	uri->buf = (char *) malloc(l + numwide * 3);
	if(!uri->buf)
	{
		uri_destroy(uri);
		return NULL;
	}
	r = uri_preprocess_(uri->buf, uristr, l);
	if(r)
	{
		uri_destroy(uri);
		return NULL;
	}
	state.uri = &(uri->uri);
	if(uriParseUriA(&state, uri->buf) != URI_SUCCESS)
	{
		uri_destroy(uri);
		return NULL;
	}
	if(base)
	{
		if(uriAddBaseUriA(&absolute, &(uri->uri), &(base->uri)) != URI_SUCCESS)
		{
			uriFreeUriMembersA(&absolute);
			uri_destroy(uri);
			return NULL;
		}
		uriFreeUriMembersA(&(uri->uri));
		memcpy(&(uri->uri), &absolute, sizeof(UriUriA));
	}
	uriNormalizeSyntaxA(&(uri->uri));
	return uri;
}

URI *
uri_create_uri(const URI *restrict source, const URI *restrict base)
{
	URI *uri;
	UriParserStateA state;

	uri = (URI *) calloc(1, sizeof(URI));
	if(!uri)
	{
		return NULL;
	}
	if(base)
	{
		if(uriAddBaseUriA(&(uri->uri), &(source->uri), &(base->uri)) != URI_SUCCESS)
		{
			uri_destroy(uri);
			return NULL;
		}
	}
	else
	{
		uri->buf = strdup(source->buf);
		if(!uri->buf)
		{
			uri_destroy(uri);
			return NULL;
		}
		state.uri = &(uri->uri);
		if(uriParseUriA(&state, uri->buf) != URI_SUCCESS)
		{
			uri_destroy(uri);
			return NULL;
		}
	}
	uriNormalizeSyntaxA(&(uri->uri));
	return uri;
}

int
uri_destroy(URI *uri)
{
	if(uri)
	{
		uriFreeUriMembersA(&(uri->uri));
		free(uri->buf);
		free(uri);
	}
	return 0;
}	
