dnl Derived from uriparser/configure.ac
dnl
dnl uriparser - RFC 3986 URI parsing library
dnl 
dnl Copyright (C) 2007, Weijia Song <songweijia@gmail.com>
dnl Copyright (C) 2007, Sebastian Pipping <webmaster@hartwork.org>
dnl All rights reserved.
dnl 
dnl Redistribution  and use in source and binary forms, with or without
dnl modification,  are permitted provided that the following conditions
dnl are met:
dnl 
dnl     * Redistributions   of  source  code  must  retain  the   above
dnl       copyright  notice, this list of conditions and the  following
dnl       disclaimer.
dnl 
dnl     * Redistributions  in  binary  form must  reproduce  the  above
dnl       copyright  notice, this list of conditions and the  following
dnl       disclaimer   in  the  documentation  and/or  other  materials
dnl       provided with the distribution.
dnl 
dnl     * Neither  the name of the <ORGANIZATION> nor the names of  its
dnl       contributors  may  be  used to endorse  or  promote  products
dnl       derived  from  this software without specific  prior  written
dnl       permission.
dnl
dnl THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
dnl "AS  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT  NOT
dnl LIMITED  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS
dnl FOR  A  PARTICULAR  PURPOSE ARE DISCLAIMED. IN NO EVENT  SHALL  THE
dnl COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
dnl INCIDENTAL,    SPECIAL,   EXEMPLARY,   OR   CONSEQUENTIAL   DAMAGES
dnl (INCLUDING,  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
dnl SERVICES;  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
dnl HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
dnl STRICT  LIABILITY,  OR  TORT (INCLUDING  NEGLIGENCE  OR  OTHERWISE)
dnl ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
dnl OF THE POSSIBILITY OF SUCH DAMAGE.
dnl
AC_DEFUN([LIBURI_CONFIG_URIPARSER],[

## Check for wprintf
AC_MSG_CHECKING(for wprintf)
AC_LINK_IFELSE([AC_LANG_SOURCE([
        #include <stdio.h>
        #include <wchar.h>
        int main() {
            wprintf((wchar_t *)0, 0);
            return 0;
        }
])],[
        AC_DEFINE([HAVE_WPRINTF],, [Define if your C runtime provides the wprintf function.])
        AC_MSG_RESULT(yes)
],[
        AC_MSG_RESULT(no)
])

## Option for URI_SIZEDOWN
AC_ARG_ENABLE(sizedown, [
==============================================================================
Extra options:
AS_HELP_STRING(
        [--enable-sizedown], [save space whereever possible])
AS_HELP_STRING(
        [], [(resulting in slower code)])], [
        URI_SIZE_DOWN_YESNO=${enableval}
        if test ${enableval} = yes ; then
                AM_CPPFLAGS="${AM_CPPFLAGS} -DURI_SIZEDOWN"
        fi
],[
        URI_SIZE_DOWN_YESNO=no
])

# Local headers must come very first. Otherwise we
## risk including headers of an already installed
## uriparser version if its path is in CPPFLAGS
AM_CPPFLAGS="-I${srcdir}/include ${AM_CPPFLAGS}"


## Character type selection
AC_ARG_ENABLE(char, [AS_HELP_STRING(
        [--disable-char], [disable code for type char *])], [
        CHAR_YESNO=${enableval}
],[
        CHAR_YESNO=yes
])
AC_ARG_ENABLE(wchar_t, [AS_HELP_STRING(
        [--disable-wchar_t], [disable code for type wchar_t *])], [
        WCHAR_T_YESNO=${enableval}
],[
        WCHAR_T_YESNO=yes
])

if test ${CHAR_YESNO} = no -a ${WCHAR_T_YESNO} = no ; then
        AC_MSG_ERROR([Parameters --disable-char and --disable-wchar_t cannot be used together.])
elif test ${CHAR_YESNO} = no -o ${WCHAR_T_YESNO} = no ; then
        if test ${CHAR_YESNO} = no ; then
                AM_CPPFLAGS="${AM_CPPFLAGS} -DURI_NO_ANSI"
                troublemaker='--disable-char'
        elif test ${WCHAR_T_YESNO} = no ; then
                AM_CPPFLAGS="${AM_CPPFLAGS} -DURI_NO_UNICODE"
                troublemaker='--disable-wchar_t'
        fi

        if test "${URI_TEST_YESNO}" = "yes"; then
                AC_MSG_ERROR([The test suite relies on code for both char * and wchar_t *.
                        Either remove ${troublemaker} or add --disable-test, please.])
        fi
fi

AM_CONDITIONAL([URI_GENERATE_DOC], [false])

AM_CONDITIONAL([URI_TEST_ENABLED], [false])

AC_CONFIG_FILES([
uriparser/Makefile:uriparser-Makefile.in
uriparser/liburiparser.pc
uriparser/doc/Makefile
uriparser/doc/Doxyfile
])

AC_CONFIG_FILES([uriparser/doc/release.sh], [chmod a+x uriparser/doc/release.sh])

])dnl
