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

AC_INIT([liburi],[master])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

AC_CONFIG_HEADER([config.h])

AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_CXX
AC_HEADER_STDC
AC_PROG_LIBTOOL

LIBURI_BUILD_DOCS
LIBURI_ENABLE_NLS
LIBURI_CC_WARN
LIBURI_CONFIG_URIPARSER

AC_CONFIG_FILES([Makefile
m4/Makefile
util/Makefile
t/Makefile
])

AC_OUTPUT
