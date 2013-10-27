//  dependencies_utils.h
//
//  Copyright (c) 2013 by NuTyX team (http://nutyx.org)
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

#ifndef DEPENDENCIES_UTILS_H
#define DEPENDENCIES_UTILS_H

#include <string>
#include <set>
#include <map>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "elf_common.h"
#include "elf.h"

using namespace std;

int getRuntimeLibrairiesList (const string& filename);


// int getDependencies(const pkgList *avail_pkgs, pkgList *dep, const pkgInfo *pkg);
// int getDependencies(const packages_t& list_of_availables_packages, packages_t& dep, const pkginfo_t& pkg);

#endif /* DEPENDENCIES_UTILS_H */
// vim:set ts=2 :
