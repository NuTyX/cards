//  compile_dependencies_utils.h
// 
//  Copyright (c) 2013-2014 by NuTyX team (http://nutyx.org)
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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <locale.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include <dirent.h>

/* TODO This is a tempory trick to test the dependencies tree, implement the function to extract all the dependencies */

#define DEPENDENCIES_FILES_DIR "/mnt/nutyx/dependences/"

/***    Structures   ****/
/* itemList is a list of *char dynamically allocated */
typedef struct
{
	char **items;
	unsigned int count;
} itemList;

/* depList is a dependances List */
typedef struct
{
	unsigned int *depsIndex;
	unsigned int *niveau;	/* To show a nice incrementation */
	unsigned int count;
	unsigned int decrement; /* number of removed dep when searching them */
	int decount;
} depList;

/* pkgInfo is the name of a package and the dependencies itemList */
typedef struct
{
	unsigned int nameIndex;
	depList *dependences;
	unsigned int niveau;
} pkgInfo;

typedef struct
{
	pkgInfo **pkgs;
	unsigned int count;
} pkgList;

/***   Basic functions   ***/
void *Malloc(size_t s);
FILE *openFile(const char *fileName);

/***  itemList: Create the list, Add Item to the list, free the list  ***/
itemList *initItemList(void);
void addItemToItemList(itemList *list, const char *item);
void freeItemList(itemList *list);

/*** depList: Create the list, Add dependence to the list, free the list  ***/
depList *initDepsList(void);
void addDepToDepList(depList *list, unsigned int nameIndex, unsigned int niveau);
depList *readDependenciesList(itemList *listFiles, unsigned int nameIndex);
void freeDepList(depList *list);

/*** pkgInfo: create the pkg, Add info, free the pkgInfo  ***/
pkgInfo *initPkgInfo(void);
pkgInfo *addInfoToPkgInfo(unsigned int nameIndex);
void freePkgInfo(pkgInfo *package);

/*** pkgList: create the pkgList, add pkgs, free the pkgList ***/
pkgList *initPkgList(void);
void addPkgToPkgList(pkgList *list, pkgInfo *package);


itemList *findFile (const char *path);

#endif /*COMPILE_DEPENDENCIES_UTILS_H */

// vim:set ts=2 :
