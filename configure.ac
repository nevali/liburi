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
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

AC_CONFIG_HEADER([config.h])

BT_PROG_XCODE
AC_PROG_CC([clang gcc c99 cc])
AC_PROG_CC_C99
AC_PROG_CXX
AC_HEADER_STDC
LT_INIT

BT_PROG_CC_WARN
BT_DEFINE_PREFIX
BT_BUILD_DOCS
BT_ENABLE_NLS

LIBURI_CONFIG_URIPARSER

AC_CONFIG_FILES([Makefile
m4/Makefile
util/Makefile
t/Makefile
liburi.pc
liburi-uninstalled.pc
])

AC_OUTPUT
