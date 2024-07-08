/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "file_utils.h"

#include <cstdlib>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>

#include <dirent.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>

/* depList is a dependances List */
struct depList {
    unsigned int* depsIndex;
    int* level; /* To show a nice incrementation */
    unsigned int count;
    unsigned int decrement; /* number of removed dep when searching them */
    int decount;
};

/* pkgInfo is the nameindex of a package and the dependencies itemList */
struct pkgInfo {
    unsigned int nameIndex;
    depList* dependences;
    int level;
};

struct pkgList {
    pkgInfo** pkgs;
    unsigned int count;
};

/*
 *  depList: Create the list, Add dependence to the list, free the list
 */
depList* initDepsList(void);

void addDepToDepList(depList* list,
    unsigned int nameIndex,
    int level);

void freeDepList(depList* list);

/*** pkgInfo: create the pkg, Add info, free the pkgInfo  ***/
pkgInfo* initPkgInfo(void);

pkgInfo* addInfoToPkgInfo(unsigned int nameIndex);

void freePkgInfo(pkgInfo* package);

/*** pkgList: create the pkgList, add pkgs, free the pkgList ***/
pkgList* initPkgList(void);

void addPkgToPkgList(pkgList* list,
    pkgInfo* package);

void freePkgList(pkgList* packagesList);

int deps_direct(itemList* filesList,
    pkgList* packagesList,
    depList* dependenciesList,
    unsigned int level);

int deps_direct(itemList* filesList,
    pkgList* packagesList,
    depList* dependenciesList,
    const char* pkgName,
    unsigned int level);

void generate_level(itemList* filesList, pkgList* packagesList,
    unsigned int& level);

char* getLongPackageName(itemList* filesList, const char* packageName);
