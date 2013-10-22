//  file_utils.h
// 
//  Copyright (c) 2002 by Johannes Winkelmann
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


#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include "string_utils.h"
#include <iostream>
#include <fstream>
#include <iterator>
#include <stdlib.h>

#include <set>
#include <map>
#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <cstring>
#include <ext/stdio_filebuf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/param.h>
#include <libgen.h>
#include <unistd.h>
#include <dirent.h>

#define S_CARD_MODE S_ISVTX|S_IRWXU|S_IXGRP|S_IXOTH|S_IRGRP|S_IROTH

using namespace std;
void * get_data ( void * var, FILE * file, long offset, size_t size, size_t nmemb);
bool checkFileExist(const string& filename);
bool checkFileEmpty(const string& filename);
bool checkFilesEqual(const string& file1, const string& file2);
bool checkPermissionsEqual(const string& file1, const string& file2);
bool createRecursiveDirs(const string& pathname);
void removeFile(const string& basedir, const string& filename);
set<string>  findFile(const string& basedir);


#endif /* FILE_UTILS_H */
// vim:set ts=2 :
