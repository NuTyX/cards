//
// compile_dependencies_utils.h
// 
//  Copyright (c) 2013-2017 by NuTyX team (http://nutyx.org)
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

#ifndef COMPILE_DEPENDENCIES_UTILS_H
#define COMPILE_DEPENDENCIES_UTILS_H

#include "string_utils.h"
#include "file_utils.h"
#include "cards_argument_parser.h"

#include <iostream>
#include <cstdlib>
#include <set>
#include <string>
#include <list>
#include <map>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <locale.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>

/* depList is a dependances List */
typedef struct
{
	unsigned int *depsIndex;
	int *niveau; /* To show a nice incrementation */
	unsigned int count;
	unsigned int decrement; /* number of removed dep when searching them */
	int decount;
} depList;

/* pkgInfo is the name of a package and the dependencies itemList */
typedef struct
{
	unsigned int nameIndex;
	depList *dependences;
	int niveau;
} pkgInfo;

typedef struct
{
	pkgInfo **pkgs;
	unsigned int count;
} pkgList;


/*
 *  depList: Create the list, Add dependence to the list, free the list
 */
depList *initDepsList(void);
void addDepToDepList(depList *list, unsigned int nameIndex, int niveau);
void freeDepList(depList *list);

/*** pkgInfo: create the pkg, Add info, free the pkgInfo  ***/
pkgInfo *initPkgInfo(void);
pkgInfo *addInfoToPkgInfo(unsigned int nameIndex);
void freePkgInfo(pkgInfo *package);

/*** pkgList: create the pkgList, add pkgs, free the pkgList ***/
pkgList *initPkgList(void);
void addPkgToPkgList(pkgList *list, pkgInfo *package);
void freePkgList(pkgList *packagesList);

int deps_direct (itemList *filesList, pkgList *packagesList, depList *dependenciesList,unsigned int niveau);
int deps_direct (itemList *filesList, pkgList *packagesList, depList *dependenciesList,const char* pkgName, unsigned int niveau);

void generate_level ( itemList *filesList, pkgList *packagesList, unsigned int *niveau);

char *getLongPackageName(itemList *filesList, const char * packageName);
#endif /* COMPILE_DEPENDENCIES_UTILS_H */
// vim:set ts=2 :
