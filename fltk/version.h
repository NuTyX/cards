/*
 * version.h
 *
 * Copyright 2015-2017 Thierry Nuttens <tnut@nutyx.org>
 * Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#ifndef VERSION_H
#define VERSION_H

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

#define VERSION_MAJOR				1
#define VERSION_MINOR				0
#define VERSION_REVISION			0
#define VERSION_BUILD				0

#define APP_NAME_STR				"flcards"
#define VER_FILE_VERSION			VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD
#define VER_FILE_VERSION_STR		STRINGIZE(VERSION_MAJOR)        \
									"." STRINGIZE(VERSION_MINOR)    \
									"." STRINGIZE(VERSION_REVISION)
#ifndef VERSION
	#define APP_NAME_VERSION_STR		APP_NAME_STR \
										" " VER_FILE_VERSION_STR
#else
	#define APP_NAME_VERSION_STR		STRINGIZE(VERSION)
#endif

#endif
