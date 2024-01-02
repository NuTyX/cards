//
//  runtime_dependencies_utils.h
//
//  Copyright (c) 2013 - 2024 by NuTyX team (http://nutyx.org)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
//  USA.
//

#ifndef RUNTIME_DEPENDENCIES_UTILS_H
#define RUNTIME_DEPENDENCIES_UTILS_H

#include "string_utils.h"
#include "file_utils.h"
#include "elf_common.h"
#include "elf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>


int getRuntimeLibrariesList (std::set<std::string>& runtimeLibrariesList,
		const std::string& filename);

#endif /* RUNTIME_DEPENDENCIES_UTILS_H */
// vim:set ts=2 :
