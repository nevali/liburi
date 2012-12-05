## Copyright 2012 Mo McRoberts.
##
##  Licensed under the Apache License, Version 2.0 (the "License");
##  you may not use this file except in compliance with the License.
##  You may obtain a copy of the License at
##
##      http://www.apache.org/licenses/LICENSE-2.0
##
##  Unless required by applicable law or agreed to in writing, software
##  distributed under the License is distributed on an "AS IS" BASIS,
##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
##  See the License for the specific language governing permissions and
##  limitations under the License.

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = uriparser . t util m4

doc_DATA = README.md LICENSE-2.0 TODO \
	liburi.3.html \
	uri_create_str.3.html uri_create_uri.3.html uri_destroy.3.html \
	uri_scheme.3.html uri_auth.3.html uri_host.3.html uri_port.3.html \
	uri_path.3.html uri_query.3.html uri_fragment.3.html uri_portnum.3.html \
	uri_absolute.3.html uri_absolute_path.3.html \
	uri_str.3.html

dist_man3_MANS = liburi.3 \
	uri_create_str.3 uri_create_uri.3 uri_destroy.3 \
	uri_scheme.3 uri_auth.3 uri_host.3 uri_port.3 \
	uri_path.3 uri_query.3 uri_fragment.3 uri_portnum.3 \
	uri_absolute.3 uri_absolute_path.3 \
	uri_str.3

lib_LTLIBRARIES = liburi.la

include_HEADERS = liburi.h

liburi_la_SOURCES = p_liburi.h \
	parse.c props.c recompose.c

liburi_la_CPPFLAGS = -I$(srcdir)/uriparser/include/uriparser

liburi_la_LDFLAGS = -avoid-version -export-symbols-regex '^uri_[a-z]'

liburi_la_LIBADD = uriparser/liburiparser.la

SUFFIXES = .3.xml .3.html .3 .1.xml .1.html .1

.1.xml.1.html:
	$(XML2HTML) $< > $@

.3.xml.3.html:
	$(XML2HTML) $< > $@

.1.xml.1:
	$(XML2MAN) $<
.3.xml.3:
	$(XML2MAN) $<

test:
	$(MAKE) -C t
	t/run-tests
