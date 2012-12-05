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

#ifndef P_LIBURI_H_
# define P_LIBURI_H_                    1

# include <stdio.h>
# include <stdlib.h>
# include <wchar.h>
# include <errno.h>

# include "Uri.h"

# include "liburi.h"

struct uri_struct
{
	UriUriA uri;
	char *buf;
};

#endif /*!P_LIBURI_H_*/
